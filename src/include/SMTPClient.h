#ifndef _SMTP_CLIENT_H_
#define _SMTP_CLIENT_H_

#include "SocketAPI.h"
#include "MIMEInterface.h"

#include <sstream>
#include <vector>
#include <chrono>
#include <mutex>

class SMTPClient {
private:
    SocketClient* _sk;
    SMTPClient();
public:
    std::string _error;

    // kernel level
    SMTPClient(std::string, std::string);
    ~SMTPClient();
    bool connected();
    bool line(std::string&, int, bool = false);
    bool lineCheck(const std::string&);
    void command(std::string, int = 0);
    bool checkStatus(int);

    // data level
    void sequence(MailContent&);
    void greet();
    void data(MailContent&);
    void address(MailContent&);
    void packHeader(MailContent&, MIMEMultipart&);

    //utility

    void quit();
};

#endif