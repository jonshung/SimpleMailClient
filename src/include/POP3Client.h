#ifndef _POP3_CLIENT_H_
#define _POP3_CLIENT_h_

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <sstream>
#include <stdexcept>
#include <exception>

#include "SocketAPI.h"

class POP3Client {
private:
    SocketClient* _sk;
    POP3Client();
public:
    std::string _error;
    std::vector<std::tuple<std::string, int>> _cacheList;
    std::vector<std::tuple<std::string, std::string>> _cacheUIDL;

    POP3Client(std::string, std::string, std::string, std::string);
    ~POP3Client();
    bool connected();
    bool line(std::string&, int = 0, bool = false);
    bool lineCheck(const std::string&);
    bool multiline(std::vector<std::string>&, int = 0, bool = false, int = -1);
    bool checkStatus(std::string);
    void command(std::string, int = 0);
    void quit();

    void greet();
    bool authorize(std::string, std::string);
    bool del(int);
    std::tuple<int, int> stat();
    std::vector<std::tuple<std::string, int>> list();
    std::vector<std::tuple<std::string, std::string>> uidl();
    void cache();                                           // cache uidl and octet size
    bool get(std::vector<std::string>&, int);
    bool get(std::vector<std::string>&, std::string);
    int getMessageSize(std::string);

    static int getOctetFromStatus(std::string);
    static std::string serializeString(std::vector<std::string>);
};

#endif