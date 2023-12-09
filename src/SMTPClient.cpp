#include "SMTPClient.h"

SMTPClient::SMTPClient(std::string _hostname, std::string _port) {
    _sk = new SocketClient(_hostname, _port);
    if (!connected()) return;
    greet();
}

bool SMTPClient::connected() {
    return _sk->connected();
}

bool SMTPClient::line(std::string& _ret, int _timeout, bool _expectingStatus) {
    std::vector<std::string> socResponse = _sk->socRecv(_timeout);
    if (socResponse.size() == 0) {
        _error = "Empty response";
        return false;
    }
    if (_expectingStatus && !lineCheck(socResponse[0])) return false;
    _ret = socResponse[0];
    if (_expectingStatus) {
        std::size_t pos = _ret.find_first_of(' ');
        if (pos == std::string::npos) pos = -1;
        _ret = _ret.substr(pos + 1);
    }
    return true;
}

bool SMTPClient::lineCheck(const std::string& _in) {
    std::stringstream ss(_in);
    _error = ss.str();
    int rcode;
    ss >> rcode;
    if (!checkStatus(rcode)) {
        return false;
    }
    return true;
}

void SMTPClient::command(std::string cmd, int _timeout) {
    std::stringstream builder;
    builder << cmd << "\r\n";
    std::string final = builder.str();
    _sk->socSend(final, _timeout);
}

bool SMTPClient::checkStatus(int code) {
    switch (code) {
    case 452:           // too many recipents
    case 500:           // line too long, max 1000 including <CR><LF>
    case 503:           // cmd out of sequence
    case 550:           // mailbox not found, typically send if recipent check occurs after data section sending
    case 553:           // ambiguous user (user not found but server have a pretty close match)
    case 554: {         // no valid recipient
        return false;
    }
    case 220: // connection accept
    case 221: // quit
    case 354:
    default:
        return true;
    }
}

SMTPClient::~SMTPClient() {
    quit();
    delete _sk;
}

void SMTPClient::sequence(MailContent& _mailContent) {
    address(_mailContent);
    data(_mailContent);
}

void SMTPClient::greet() {
    std::string buffer;
    line(buffer, 300, true);                // 220 connection established
    char hostname[100];
    gethostname(hostname, 100);
    command(std::string("EHLO ") + hostname);
    line(buffer, 300, true);                // 250 greets
}

void SMTPClient::address(MailContent& _mailContent) {
    try {
        std::string buffer;
        command("MAIL FROM:<" + _mailContent.getFrom().getReceiptAddress() + ">");
        if (!line(buffer, 300, true)) {
            throw std::runtime_error(_error);
        }

        for (MailboxAddress rcpt : _mailContent.getTo()) {
            command("RCPT TO:<" + rcpt.getReceiptAddress() + ">");
            if (!line(buffer, 300, true)) {
                throw std::runtime_error(_error);
            }
        }

        for (MailboxAddress rcpt : _mailContent.getCC()) {
            command("RCPT TO:<" + rcpt.getReceiptAddress() + ">");
            if (!line(buffer, 300, true)) {
                throw std::runtime_error(_error);
            }
        }

        for (MailboxAddress rcpt : _mailContent.getBCC()) {
            command("RCPT TO:<" + rcpt.getReceiptAddress() + ">");
            if (!line(buffer, 300, true)) {
                throw std::runtime_error(_error);
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << e.what() << ", at address command: " << _error << std::endl;
    }
}

//  internal. this method should only be called after calling the DATA command to the server
void SMTPClient::packHeader(MailContent& _mailContent, MIMEMultipart& _body) {
    // required header date and address fiel
    _body.setHeader("MIME-Version", "1.0");

    if (_mailContent.getTo().size() > 0) {
        std::stringstream rcptList;
        int n = 4;
        for (MailboxAddress rcpt : _mailContent.getTo()) {
            n += 7 + rcpt.getReceiptName().length() + rcpt.getReceiptAddress().length();
            // should also precheck line and cut it but a limit imposed on mailbox address should be enough to
            // deal with this issue.
            std::string pad = "";
            if (n > 76) {
                pad = "\r\n ";
                n = 1;
            }
            rcptList << "\"" << rcpt.getReceiptName() << "\" <" << rcpt.getReceiptAddress() << ">, " << pad;
        }
        _body.setHeader("To", rcptList.str());
    }

    _body.setHeader("From", "\"" + _mailContent.getFrom().getReceiptName() + "\" <" + _mailContent.getFrom().getReceiptAddress() + ">");

    /**                              COPY                                    */
    if (_mailContent.getCC().size() > 0) {
        std::stringstream rcptList;
        int n = 4;
        for (MailboxAddress rcpt : _mailContent.getCC()) {
            n += 7 + rcpt.getReceiptName().length() + rcpt.getReceiptAddress().length();
            std::string pad = "";
            if (n > 76) {
                pad = "\r\n ";
                n = 1;
            }
            rcptList << "\"" << rcpt.getReceiptName() << "\" <" << rcpt.getReceiptAddress() << ">, " << pad;
        }
        _body.setHeader("Cc", rcptList.str());
    }

    /**                              DATE                                    */
    const auto currentTime = std::chrono::system_clock::now();
    const auto timer = std::chrono::system_clock::to_time_t(currentTime);
    std::tm timeRep{};

#if defined(__unix__)
    localtime_r(&timer, &timeRep);
#elif defined(_MSC_VER)
    localtime_s(&timeRep, &timer);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    timeRep = *std::localtime(&timer);
#endif

    std::stringstream dateVl;
    dateVl << std::put_time(&timeRep, "%d %b %Y %H:%M %z");
    _body.setHeader("Date", dateVl.str());

    /**                             Subject                                   */
    _body.setHeader("Subject", _mailContent.getSubject());
}

void SMTPClient::data(MailContent& _mailContent) {
    try {
        std::string buffer;
        command("DATA");
        if (!line(buffer, 120, true)) {
            throw std::runtime_error(_error);
        }
        MIMEMultipart mainBody("multipart/mixed", "main");
        packHeader(_mailContent, mainBody);

        /**                         PLAIN TEXT AND HTML DATA                            */
        if (_mailContent.getPlainContent().length() > 0 || _mailContent.getHTMLContent().length() > 0) {
            std::shared_ptr<MIMEMultipart> textPart = std::make_shared<MIMEMultipart>("multipart/alternative", "text-boundary");
            // plain text
            if (_mailContent.getPlainContent().length() > 0) {
                std::shared_ptr<MIMEText> plainContent = std::make_shared<MIMEText>("text/plain", "quoted-printable", "utf-8", _mailContent.getPlainContent());
                textPart->addPart(plainContent);
            }

            // html
            if (_mailContent.getHTMLContent().length() > 0) {
                std::shared_ptr<MIMEText> htmlContent = std::make_shared<MIMEText>("text/html", "quoted-printable", "utf-8", _mailContent.getHTMLContent());
                textPart->addPart(htmlContent);
            }
            mainBody.addPart(textPart);
        }

        /**                                ATTACHMENTS                                  */
        for(MIMEAttachment attachment : _mailContent.getAttachments()) {
            mainBody.addPart(std::make_shared<MIMEAttachment>(attachment));
        }
        std::string msg = mainBody.build();
        if(msg.substr(msg.length() - 2) == "\r\n") {        //multipart final trailing.
            msg.pop_back();
            msg.pop_back();
        }

        command(msg, 300);
        // end data sequence
        command(".");
        if (!line(buffer, 600, true)) {
            std::cout << "error: " << _error << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cout << e.what() << ", at data command: " << _error << std::endl;
    }
}

void SMTPClient::quit() {
    try {
        std::string buffer;
        command("QUIT");
        if (!line(buffer, 300, true)) {
            throw std::runtime_error(_error);
        }
    }
    catch (const std::exception& e) {
        std::cout << e.what() << ", at quit command: " << _error << '\n';
    }
}