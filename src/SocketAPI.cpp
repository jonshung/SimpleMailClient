#include "SocketAPI.h"

#ifdef __unix
SocketClient::SocketClient(const std::string& destinationHost, const std::string& port) {
    int status;
    _connected = false;

    // Retrieve address from domain name
    addrinfo hint, * p;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(destinationHost.c_str(), port.c_str(), &hint, &p)) < 0) {
        _connected = false;
        _error = "cannot retrieve hostname info";
        return;
    }

    addrinfo* _server_addr;
    for (_server_addr = p; _server_addr != NULL; _server_addr = _server_addr->ai_next) {
        if (_server_addr->ai_family == AF_INET || _server_addr->ai_family == AF_INET6) {
            break;
        }
    }

    freeaddrinfo(p);

    // creating the socket
    if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        _connected = false;
        _error = "cannot create socket";
        return;
    }

    if ((status = connect(_fd, _server_addr->ai_addr, _server_addr->ai_addrlen)) < 0) {
        _connected = false;
        _error = ": Cannot connect to host " + destinationHost + " on port: " + port;
        return;
    }
    _connected = true;
}

bool SocketClient::connected() {
    return _connected;
}

std::vector<std::string> SocketClient::socRecv(int _timeout) {
    timeval _interval;
    _interval.tv_sec = _timeout;
    _interval.tv_usec = 0;
    int err = 0;
    err = setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &_interval, sizeof(_interval));

    const std::size_t _maxbuf = 1024;
    char _buffer[_maxbuf];
    std::vector<std::string> _ret;

    int _read = recv(_fd, _buffer, _maxbuf, 0);
    if (_read == 0) {
        _error = "Connection stopped unexpectedly";
        return _ret;
    }
    int _total = 0;
    std::stringstream ss(std::string(_buffer, _read));
    std::string _temp;

    while (std::getline(ss, _temp)) {
        if (_temp.length() > 0 && _temp[_temp.length() - 1] == '\r') {
            _temp.append("\n");
        }
        _ret.push_back(_temp);
        _total += _temp.length();
    }
    return _ret;
}

int SocketClient::socSend(const std::string& msg, const int& _timeout) {
    timeval _interval;
    _interval.tv_sec = _timeout;
    _interval.tv_usec = 0;
    int err = 0;
    err = setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, &_interval, sizeof(_interval));

    const char* c = msg.c_str();
    int total = 0;
    int bytesleft = msg.length();
    int n;

    while (total < msg.length()) {
        n = send(_fd, c + total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    return total;
}

SocketClient::~SocketClient() {
    // cleanup
    close(_fd);
}
#endif

#ifdef _WIN32
SocketClient::SocketClient(const std::string& destinationHost, const std::string& port) {
    int status;

    if ((status = WSAStartup(MAKEWORD(2, 2), &_wsaData)) != 0) {
        _error = "Cannot start socket api";
    }

    addrinfo hint, * p;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(destinationHost.c_str(), port.c_str(), &hint, &p)) != 0) {
        WSACleanup();
        _connected = false;
        _error = "cannot retrieve hostname info";
        return;
    }
    _client = INVALID_SOCKET;

    addrinfo* _server_addr;
    for (_server_addr = p; _server_addr != NULL; _server_addr = _server_addr->ai_next) {
        if (_server_addr->ai_family == AF_INET || _server_addr->ai_family == AF_INET6) {
            break;
        }
    }

    freeaddrinfo(p);

    _client = socket(_server_addr->ai_family, _server_addr->ai_socktype, _server_addr->ai_protocol);

    if (_client == INVALID_SOCKET) {
        WSACleanup();
        _connected = false;
        _error = "cannot create socket";
        return;
    }

    if ((status = connect(_client, _server_addr->ai_addr, (int)_server_addr->ai_addrlen)) == SOCKET_ERROR) {
        closesocket(_client);
        _client = INVALID_SOCKET;
    }

    if (_client == INVALID_SOCKET) {
        WSACleanup();
        _connected = false;
        _error = ": Cannot connect to host " + destinationHost + " on port: " + port;
        return;
    }
}

bool SocketClient::connected() {
    return _connected;
}

std::vector<std::string> SocketClient::socRecv(const int& _timeout) {

    timeval _interval;
    _interval.tv_sec = _timeout;
    _interval.tv_usec = 0;
    int err = 0;
    err = setsockopt(_client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&_interval, sizeof(_interval));

    const std::size_t _maxbuf = 1024;
    char _buffer[_maxbuf];

    int _read = recv(_client, _buffer, _maxbuf, 0);
    if (_read == 0) {
        _error = "Connection closed unexpectedly";
        return;
    }
    int _total = 0;
    std::vector<std::string> _ret;
    std::stringstream ss(std::string(_buffer, _read));
    std::string _temp;

    while (std::getline(ss, _temp)) {
        if (_temp.length() > 0 && _temp[_temp.length() - 1] == '\r') {
            _temp.append("\n");
        }
        _ret.push_back(_temp);
        _total += _temp.length();
    }
    return _ret;
}

int SocketClient::socSend(const std::string& msg, const int& _timeout) {
    timeval _interval;
    _interval.tv_sec = _timeout;
    _interval.tv_usec = 0;
    int err = 0;
    err = setsockopt(_client, SOL_SOCKET, SO_SNDTIMEO, (const char*)&_interval, sizeof(_interval));
    
    const char* c = msg.c_str();
    int total = 0;
    int bytesleft = msg.length();
    int n;

    while (total < msg.length()) {
        n = send(_client, c + total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    return total;
}

SocketClient::~SocketClient() {
    // cleanup
    closesocket(_client);
    WSACleanup();
}
#endif