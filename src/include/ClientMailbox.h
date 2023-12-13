#ifndef _CLIENT_MAILBOX_H_
#define _CLIENT_MAILBOX_H_

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <regex>

#include "nlohmann/json.hpp"
#include "MIMEInterface.h"
#include "MailNetworkWrapper.h"
#include "MIMEParser.h"

#include <QtQml/QJSEngine>

using json = nlohmann::json;

class Credential {
private:
    MailboxAddress _addr;
    std::string _password;
public:
    Credential(MailboxAddress, std::string);
    Credential();

    std::string getPassword();
    MailboxAddress& getAddress();

    void setPassword(std::string);
    void setAddress(MailboxAddress);
};

class ClientMailbox {
private:
    json _data;
    Credential _cred;
public:
    std::string _error{ "" };
    ClientMailbox(Credential);
    ~ClientMailbox();

    Credential getCredential();
    json& getFolders();
    bool folderExists(std::string);
    bool mailIsRead(std::string UIDL);
    bool mailDownloaded(std::string UIDL);
    void addFolder(std::string);
    void setMailIsRead(std::string);

    void dataIntegrity();

    void fetch(std::string, std::string, const QString& = "");
    std::string filter(const std::string&, const QString& = "");
    std::string defaultFilter(const QString&, const QString&, const QString&);
    void sendContent(std::string, std::string, MailContent&);

    void init(Credential);
};

#endif