#include "POP3Client.h"

POP3Client::POP3Client(std::string _hostname, std::string _port, std::string _username, std::string _password) {
    _sk = new SocketClient(_hostname, _port);
    if(!connected()) return;
    greet();
    if (!authorize(_username, _password)) {
        _error = "cannot authorize " + _username + ", error: " + _error;
        return;
    }
    cache();
}

bool POP3Client::connected() {
    return _sk->connected();
}

POP3Client::~POP3Client() {
    quit();
    delete _sk;
}

void POP3Client::command(std::string cmd, int _timeout) {
    std::stringstream builder;
    builder << cmd << "\r\n";
    std::string final = builder.str();
    _sk->socSend(final, _timeout);
}

bool POP3Client::line(std::string& _ret, int _timeout, bool _expectingStatus) {
    std::vector<std::string> _socResponse = _sk->socRecv(_timeout);
    if (_socResponse.size() == 0) {
        _error = "Empty response";
        return false;
    }
    if (_expectingStatus && !lineCheck(_socResponse[0])) return false;
    _ret = _socResponse[0];
    if (_expectingStatus) {
        std::size_t pos = _ret.find_first_of(' ');
        if (pos == std::string::npos) pos = -1;
        _ret = _ret.substr(pos + 1);
    }
    return true;
}

bool POP3Client::lineCheck(const std::string& _in) {
    std::stringstream ss(_in);
    _error = ss.str();
    std::string rcode;
    ss >> rcode;
    if (!checkStatus(rcode)) {
        return false;
    }
    return true;
}

bool POP3Client::checkStatus(std::string rcode) {
    return rcode == "+OK";
}

bool POP3Client::multiline(std::vector<std::string>& _ret, int _timeout, bool _expectingStatus, int _limit) {
    std::vector<std::string> _temp = _sk->socRecv(_timeout);
    int _total = 0;
    if (_temp.size() < 0) {
        _error = "Empty response";
        return false;
    }
    if (_expectingStatus) {
        std::string _statLine = _temp[0];
        _temp.erase(_temp.begin());
        if (!lineCheck(_statLine))   return false;
    }
    for (int i = 0; i < _temp.size(); i++) {
        std::string _buffer = _temp[i];
        _total += _buffer.length();
        if ((_buffer == ".\r\n") && ((_total - 3) >= _limit || _limit == -1)) {      //  _total - 3: unaccounting the message terminator ".<CRLF>"
            _ret.insert(_ret.end(), _temp.begin(), _temp.begin() + i);
            break;
        }

        if (i == _temp.size() - 1) {
            int _app = 0;
            //  since buffer size is maxed out at 1024, so it might cut a line unexpectedly.
            //  this block is used to see if the last line is ending in a <CRLF> sequence.
            //  if not, then we can be sure it was ended unexpectedly.
            if (_ret.size() > 0) {
                std::string _lastLine = _ret[_ret.size() - 1];
                while (_lastLine.length() > 1 && _lastLine.substr(_lastLine.length() - 2) != "\r\n") {
                    if (_app >= _temp.size()) break;
                    _ret[_ret.size() - 1].append(_temp[_app]);
                    _app++;
                    _lastLine = _ret[_ret.size() - 1];
                }
            }

            _ret.insert(_ret.end(), _temp.begin() + _app, _temp.end());
            //  checking for max size, limit should be defined elsewhere.
            if (_total > 25 * 1024 * 1024) {
                break; // max 25mb
            }
            //  continuously checking for new incoming message, if timeout, _temp will be 0-sized.
            //  terminating the sequence with incomplete response.
            _temp = _sk->socRecv();
            if (_temp.size() < 0) {
                _error = "Incomplete response";
                return false;
            }
            i = -1;
            continue;
        }
    }
    return true;
}

void POP3Client::greet() {
    try {
        std::string buffer;
        line(buffer, 300, true);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << ", on greet command: " << _error << std::endl;
    }
}

bool POP3Client::authorize(std::string username, std::string password) {
    try {
        std::string buffer;
        command("USER " + username);
        if (!line(buffer, 180, true)) {
            return false;
        }
        command("PASS " + password);
        return line(buffer, 180, true);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << ", on authorize command: " << _error << std::endl;
    }
    return false;
}

bool POP3Client::del(int id) {
    try {
        std::string buffer;
        command("DELE " + id);
        return line(buffer, 180, true);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << ", on delete command: " << _error << std::endl;
    }
    return false;
}

std::tuple<int, int> POP3Client::stat() {
    std::tuple<int, int> ret = std::make_tuple<int, int>(0, 0);
    try {
        std::string buffer;
        command("STAT");
        if (!line(buffer, 180, true)) {
            return ret;
        }
        std::stringstream parser(buffer);
        parser >> std::get<0>(ret);
        parser >> std::get<1>(ret);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << ", on stat command: " << _error << std::endl;
    }
    return ret;
}

std::vector<std::tuple<std::string, int>> POP3Client::list() {
    std::vector< std::tuple<std::string, int> > ret;
    try {
        std::vector<std::string> bufferArray;
        command("LIST");
        if (!multiline(bufferArray, 300, true)) {
            return ret;
        }
        std::stringstream parser;
        for (std::string buffer : bufferArray) {
            parser.clear();
            parser.str(buffer);
            std::tuple<std::string, int> t = std::make_tuple<std::string, int>("", 0);
            if (!(parser >> std::get<0>(t))) {
                continue;
            }
            if (!(parser >> std::get<1>(t))) {
                continue;
            }
            ret.push_back(t);
        }
    }
    catch (const std::exception& e) {
        std::cout << e.what() << ", on list command: " << _error << std::endl;
    }
    _cacheList = ret;
    return ret;
}

std::vector<std::tuple<std::string, std::string>> POP3Client::uidl() {
    std::vector< std::tuple<std::string, std::string> > ret;
    try {
        std::vector<std::string> bufferArray;
        command("UIDL");
        if (!multiline(bufferArray, 300, true)) {
            return ret;
        }
        std::stringstream parser;
        for (std::string buffer : bufferArray) {
            parser.clear();
            parser.str(buffer);
            std::tuple<std::string, std::string> t = std::make_tuple<std::string, std::string>("", "");
            if (!(parser >> std::get<0>(t))) {
                continue;
            }
            if (!(parser >> std::get<1>(t))) {
                continue;
            }
            ret.push_back(t);
        }
    }
    catch (const std::exception& e) {
        std::cout << e.what() << ", on list command: " << _error << std::endl;
    }
    _cacheUIDL = ret;
    return ret;
}

void POP3Client::cache() {
    list();
    uidl();
}

int POP3Client::getMessageSize(std::string id) {
    for (auto i : _cacheList) {
        if (std::get<0>(i) == id) {
            return std::get<1>(i);
        }
    }
    list();
    for (auto i : _cacheList) {
        if (std::get<0>(i) == id) {
            return std::get<1>(i);
        }
    }
    return 0;
}

int POP3Client::getOctetFromStatus(std::string in) {
    std::stringstream ss(in);
    int data = 0;
    ss >> data;
    return data;
}

bool POP3Client::get(std::vector<std::string>& _ret, std::string _msgid) {
    try {
        command("RETR " + _msgid);
        int msg_size = getMessageSize(_msgid);
        if (msg_size == 0) return false;
        return multiline(_ret, 300, true, msg_size);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << ", on get command: " << _error << std::endl;
    }
    return false;
}

bool POP3Client::get(std::vector<std::string>& _ret, int _msgid) {
    return get(_ret, std::to_string(_msgid));
}

std::string POP3Client::serializeString(std::vector<std::string> _in) {
    std::stringstream _ret;
    for (std::string _line : _in) {
        _ret << _line;
    }
    return _ret.str();
}

void POP3Client::quit() {
    try {
        command("QUIT");
        std::string buffer;
        line(buffer, 180, true);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << ", on quit command: " << _error << std::endl;
    }
}