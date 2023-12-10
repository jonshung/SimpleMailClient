#ifndef _SOCKET_API_H_
#define _SOCKET_API_H_

#include <stdexcept>
#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>

class SocketClient;

#ifdef __unix
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class SocketClient {
private:
    bool _connected;
    std::string _error;
    int _fd;
public:
    SocketClient(const std::string&, const std::string&);
    ~SocketClient();
    int socSend(const std::string&, const int& = 0);
    std::vector<std::string> socRecv(int = 0);
    bool connected();
};
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment (lib, "ws2_32.lib") // for Visual Studio WinSock 2 link

class SocketClient {
private:
    bool _connected;
    std::string _error;
    WSADATA _wsaData;
    SOCKET _client;
public:
    SocketClient(const std::string&, const std::string&);
    ~SocketClient();
    int socSend(const std::string&, const int& = 0);
    std::vector<std::string> socRecv(const int& = 0);
    bool connected();
};

#endif
#endif