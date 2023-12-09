#include "ClientMailbox.h"

Credential::Credential(MailboxAddress _addrInfo, std::string _pwd) {
    _addr = _addrInfo;
    _password = _pwd;
}

Credential::Credential() {
    _addr = MailboxAddress();
    _password = "";
}

std::string Credential::getPassword() {
    return _password;
}

MailboxAddress& Credential::getAddress() {
    return _addr;
}

void Credential::setPassword(std::string _vl) {
    _password = _vl;
}

void Credential::setAddress(MailboxAddress _vl) {
    _addr = _vl;
}

ClientMailbox::ClientMailbox(Credential _info) {
    _cred = _info;
    std::filesystem::path localDataPath = std::string("mailboxes/" + _info.getAddress().getReceiptAddress());
    if (!std::filesystem::is_directory(localDataPath)) {
        std::filesystem::create_directories(localDataPath);
    }
    init(_info);
}

void ClientMailbox::init(Credential _credInfo) {
    MailboxAddress _info = _credInfo.getAddress();
    std::filesystem::path localDataPath = std::string("mailboxes/" + _info.getReceiptAddress());
    if (!std::filesystem::is_regular_file(localDataPath / ".mailboxCfg.json")) {
        std::ofstream configFile(localDataPath / ".mailboxCfg.json");
        json defaultCfgData;
        defaultCfgData["user"] = _info.getReceiptName();
        defaultCfgData["address"] = _info.getReceiptAddress();
        defaultCfgData["password"] = _credInfo.getPassword();
        defaultCfgData["folders"] = {
            {"Inbox", {
                {"enabled", true},
                {"from", json::array({})},
                {"subjectKeyword", json::array({})},
                {"bodyKeyword", json::array({})}
            }},
            {"Spam", {
                {"enabled", true},
                {"from", json::array({})},
                {"subjectKeyword", json::array({})},
                {"bodyKeyword", json::array({})}
            }},
            {"Project", {
                {"enabled", true},
                {"from", json::array({})},
                {"subjectKeyword", json::array({})},
                {"bodyKeyword", json::array({})}
            }},
            {"Work", {
                {"enabled", true},
                {"from", json::array({})},
                {"subjectKeyword", json::array({})},
                {"bodyKeyword", json::array({})}
            }},
            {"Important", {
                {"enabled", true},
                {"from", json::array({})},
                {"subjectKeyword", json::array({})},
                {"bodyKeyword", json::array({})}
            }}
        };
        defaultCfgData["mailboxData"] = json({});
        _data = defaultCfgData;
        configFile << std::setw(4) << defaultCfgData << std::endl;
        configFile.close();
    }
    else {
        std::ifstream configFile(localDataPath / ".mailboxCfg.json");
        if (!configFile.is_open()) {
            throw std::runtime_error("Error cannot open file");
        }
        configFile >> _data;
        configFile.close();
        dataIntegrity();
    }
}

void ClientMailbox::dataIntegrity() {
    if (!_data["user"].is_string()) {
        throw std::runtime_error("field user is in incorrect type, expected: string");
    }
    if (!_data["address"].is_string()) {
        throw std::runtime_error("field address is in incorrect type, expected: string");
    }
    if (!_data["password"].is_string()) {
        throw std::runtime_error("field password is in incorrect type, expected: string");
    }
    if (_cred.getAddress().getReceiptName() != _data["user"]) _cred.getAddress().setReceiptName(_data["user"]);
    if (_cred.getAddress().getReceiptAddress() != _data["address"]) _cred.getAddress().setReceiptAddress(_data["address"]);
    if (_cred.getPassword() != _data["password"]) _cred.setPassword(_data["password"]);

    if (!_data["folders"].is_object()) {
        throw std::runtime_error("field folders is in incorrect type, expected: object");
    }
    if (!_data["folders"].contains("Inbox")) {
        _data["folders"]["Inbox"] =
        { "Inbox", {
            {"enabled", true},
            {"from", json::array({})},
            {"subjectKeyword", json::array({})},
            {"bodyKeyword", json::array({})}
        } };
    }

    for (auto& folderData : _data["folders"].items()) {
        if (!folderData.value().is_object()) {
            throw std::runtime_error("Corrupted folder data");
        }
        if (!folderData.value().contains("enabled") ||
            !folderData.value().contains("from") ||
            !folderData.value().contains("subjectKeyword") ||
            !folderData.value().contains("bodyKeyword")) {
            throw std::runtime_error("Missing field at folderData: " + folderData.key());
        }
        if (!folderData.value()["enabled"].is_boolean() ||
            !folderData.value()["from"].is_array() ||
            !folderData.value()["subjectKeyword"].is_array() ||
            !folderData.value()["bodyKeyword"].is_array()) {
            throw std::runtime_error("Error type of one of " + folderData.key() + " field");
        }
    }

    if (!_data["mailboxData"].is_object()) {
        throw std::runtime_error("field mailboxData is in incorrect type, expected: object");
    }

    for (auto& mailData : _data["mailboxData"].items()) {
        if (!mailData.value().is_object()) {
            throw std::runtime_error("Corrupted mailbox data");
        }
        if (!mailData.value().contains("uidl") || !mailData.value().contains("isRead")) {
            throw std::runtime_error("Missing uidl or isRead field at maildata: " + mailData.key());
        }
        if (!mailData.value()["uidl"].is_string() || !mailData.value()["isRead"].is_boolean()) {
            throw std::runtime_error("Error type of uidl field or isRead field");
        }
    }
}

Credential ClientMailbox::getCredential() {
    return _cred;
}

json ClientMailbox::getFolders() {
    return _data["json"];
}

bool ClientMailbox::mailIsRead(std::string _uidl) {
    if (!mailDownloaded(_uidl)) return false;
    return _data["mailBoxData"][_uidl]["isRead"];
}

bool ClientMailbox::mailDownloaded(std::string _uidl) {
    return _data["mailboxData"].contains(_uidl);
}

bool ClientMailbox::folderExists(std::string _folder) {
    return getFolders().contains(_folder);
}

void ClientMailbox::addFolder(std::string _folder) {
    if (folderExists(_folder)) return;
    _data["json"].push_back(_folder);
}

void ClientMailbox::setMailIsRead(std::string _id) {
    if (!mailDownloaded(_id)) return;
    _data["mailboxData"][_id]["isRead"] = true;
}

void ClientMailbox::fetch(std::string _host, std::string _port, const QString& _filterEval) {

    POP3Client pop3Client(_host, _port, getCredential().getAddress().getReceiptAddress(), getCredential().getPassword());
    if (!pop3Client.connected()) {
        throw std::runtime_error("Cannot connect to pop3 server");
    }
    for (auto it : pop3Client._cacheUIDL) {
        std::string id = std::get<0>(it);
        std::string uidl = std::get<1>(it);
        if (mailDownloaded(uidl)) continue;

        std::filesystem::path localDataPath = std::string("mailboxes/" + getCredential().getAddress().getReceiptAddress());

        std::vector<std::string> bufferVector;
        if (!pop3Client.get(bufferVector, id)) {
            throw std::runtime_error("error: " + pop3Client._error);
        }
        std::string bufferString = POP3Client::serializeString(bufferVector);
        localDataPath.append(filter(bufferString, _filterEval));
        if (!std::filesystem::is_directory(localDataPath)) {
            std::filesystem::create_directories(localDataPath);
        }
        localDataPath.append(uidl);
        if (localDataPath.extension() == "") localDataPath.replace_extension(".msg");

        std::ofstream mailMessageFile(localDataPath);
        if (!mailMessageFile.is_open()) {
            // error handling
        }
        mailMessageFile << bufferString;
        mailMessageFile.close();

        _data["mailboxData"][uidl] = { {"uidl", uidl}, {"isRead", false} };
    }
}

std::string ClientMailbox::filter(const std::string& _content, const QString& _filterEval) {
    mimetic::MimeEntity parsedData = parseMIMEEntity(_content);
    QString from = QString::fromStdString(parsedData.header().from().str());
    QString subject = QString::fromStdString(parsedData.header().subject());
    mimetic::MimeEntity textEntity;
    getMIMETextEntity(&parsedData, textEntity);
    QString bodyText = QString::fromStdString(textEntity.body());

    if (_filterEval.length() > 0) {
        QJSEngine engine;
        QJSValueList args;
        args << from << subject << bodyText;
        QJSValue filterRes = engine.evaluate(_filterEval).call(args);
        if (!filterRes.isString()) return defaultFilter(from, subject, bodyText);     // guard here because QString toString() and toStdString() are demanding operations
        std::string res = filterRes.toString().toStdString();
        if (folderExists(res) && getFolders()[res] == true) return res;
    }
    return defaultFilter(from, subject, bodyText);
}

std::string ClientMailbox::defaultFilter(const QString& _from, const QString& _subject, const QString& _bodyText) {
    std::stringstream regExpr;
    std::regex p_r;
    for(auto& folderObject : getFolders().items()) {
        if(folderObject.value()["enabled"] == false) continue;

        for (std::string match : folderObject.value()["from"]) {
            regExpr << match << "|";
        }
        p_r = std::regex(regExpr.str());
        if (std::regex_search(_from.toStdString(), p_r)) {
            return folderObject.key();
        }
        regExpr.str("");
        regExpr.clear();

        for (std::string match : folderObject.value()["subjectKeyword"]) {
            regExpr << match << "|";
        }
        p_r = std::regex(regExpr.str());
        if (std::regex_search(_subject.toStdString(), p_r)) {
            return folderObject.key();
        }
        regExpr.str("");
        regExpr.clear();

        for (std::string match : folderObject.value()["bodyKeyword"]) {
            regExpr << match << "|";
        }
        p_r = std::regex(regExpr.str());
        if (std::regex_search(_bodyText.toStdString(), p_r)) {
            return folderObject.key();
        }
    }
    return "Inbox";
}

void ClientMailbox::sendContent(std::string _host, std::string _port, MailContent& _mailContent) {
    SMTPClient smtpClient(_host, _port);
    if (!smtpClient.connected()) {
        throw std::runtime_error("Cannot connect to smtp server");
    }
    smtpClient.sequence(_mailContent);
}


ClientMailbox::~ClientMailbox() {
    std::filesystem::path localDataPath = std::string("mailboxes/" + _cred.getAddress().getReceiptAddress());
    std::ofstream configFile(localDataPath.append(".mailboxCfg.json"));
    configFile << std::setw(4) << _data << std::endl;
    configFile.close();
}