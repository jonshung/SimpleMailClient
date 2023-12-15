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
    qDebug() << "Creating client profile...";
    std::filesystem::path localDataPath = std::filesystem::current_path();
    localDataPath.append(std::string("mailboxes/" + _info.getAddress().getReceiptAddress()));
    if (!std::filesystem::is_directory(localDataPath)) {
        std::filesystem::create_directories(localDataPath);
    }
    if(_cred.getAddress().getReceiptAddress().length() > 0) init(_info);
}

void ClientMailbox::init(Credential _credInfo) {
    MailboxAddress _info = _credInfo.getAddress();
    std::filesystem::path localDataPath = std::string("mailboxes/" + _info.getReceiptAddress());
    if (!std::filesystem::is_regular_file(localDataPath / ".mailboxCfg.json")) {
        std::ofstream configFile(localDataPath / ".mailboxCfg.json");
        ordered_json defaultCfgData;
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
            _error = "Cannot open config file";
            return;
        }
        configFile >> _data;
        configFile.close();
        dataIntegrity();
    }
}

void ClientMailbox::dataIntegrity() {
    MailboxAddress info = _cred.getAddress();
    if (!_data["user"].is_string()) {
        _error = "field user is in incorrect type, expected: string";
        _data["user"] = info.getReceiptName();
    }
    if (!_data["address"].is_string()) {
        _error = "field address is in incorrect type, expected: string";
        _data["address"] = info.getReceiptAddress();
    }
    if (!_data["password"].is_string()) {
        _error = "field password is in incorrect type, expected: string";
        _data["password"] = _cred.getPassword();
    }
    if (_cred.getAddress().getReceiptName() != _data["user"]) _data["user"] = _cred.getAddress().getReceiptName();
    if (_cred.getAddress().getReceiptAddress() != _data["address"]) _data["address"] = _cred.getAddress().getReceiptAddress();
    if (_cred.getPassword() != _data["password"]) _data["password"] = _cred.getPassword();

    if (!_data["folders"].is_object()) {
        _error = "field folders is in incorrect type, expected: object";
        _data["folders"] = {
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
    }
    if (!_data["folders"].contains("Inbox")) {
        _data["folders"]["Inbox"] =
        {
            {"enabled", true},
            {"from", json::array({})},
            {"subjectKeyword", json::array({})},
            {"bodyKeyword", json::array({})}
        };
    }

    for (auto& folderData : _data["folders"].items()) {
        if (!folderData.value().is_object()) {
            _error = "Corrupted folder data";
        }
        if (!folderData.value().contains("enabled") ||
            !folderData.value().contains("from") ||
            !folderData.value().contains("subjectKeyword") ||
            !folderData.value().contains("bodyKeyword")) {
            _error = "Missing field at folderData: " + folderData.key();
        }
        if (!folderData.value()["enabled"].is_boolean() ||
            !folderData.value()["from"].is_array() ||
            !folderData.value()["subjectKeyword"].is_array() ||
            !folderData.value()["bodyKeyword"].is_array()) {
            _error = "Error type of one of " + folderData.key() + " field";
        }
    }

    if (!_data["mailboxData"].is_object()) {
        _error = "field mailboxData is in incorrect type, expected: object";
        _data["mailboxData"] = json({});
    }

    for (auto& mailData : _data["mailboxData"].items()) {
        if (!mailData.value().is_object()) {
            _error = "Corrupted mailbox data";
        }
        if (!mailData.value().contains("uidl") || !mailData.value().contains("isRead")) {
            _error = "Missing uidl or isRead field at maildata: " + mailData.key();
        }
        if (!mailData.value()["uidl"].is_string() || !mailData.value()["isRead"].is_boolean()) {
            _error = "Error type of uidl field or isRead field";
        }
    }
}

Credential ClientMailbox::getCredential() {
    return _cred;
}

ordered_json& ClientMailbox::getFolders() {
    return _data["folders"];
}

bool ClientMailbox::mailIsRead(std::string _uidl) {
    if (!mailDownloaded(_uidl)) return false;
    return _data["mailboxData"][_uidl]["isRead"];
}

bool ClientMailbox::mailDownloaded(std::string _uidl) {
    return _data["mailboxData"].contains(_uidl);
}

bool ClientMailbox::folderExists(std::string _folder) {
    return getFolders().contains(_folder);
}

void ClientMailbox::addFolder(std::string _folder) {
    if (folderExists(_folder)) return;
    _data["folders"][_folder] =
    {
        {"enabled", true},
        {"from", json::array({})},
        {"subjectKeyword", json::array({})},
        {"bodyKeyword", json::array({})}
    };;
}

void ClientMailbox::setMailIsRead(std::string _id) {
    if (!mailDownloaded(_id)) return;
    _data["mailboxData"][_id]["isRead"] = true;
}

void ClientMailbox::fetch(std::string _host, std::string _port, const std::string& _filterEval) {

    POP3Client pop3Client(_host, _port, getCredential().getAddress().getReceiptAddress(), getCredential().getPassword());
    if (!pop3Client.connected()) {
        _error = "-Cannot connect to POP3 Server";;
        return;
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
        std::string filterRes = filter(bufferString, _filterEval);
        localDataPath.append(filterRes);
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

std::string ClientMailbox::filter(const std::string& _content, const std::string& _filterEval) {
#if ( defined (LINUX) || defined (__linux__) )
    mimetic::MimeEntity parsedData = parseMIMEEntity(_content);
    std::string from = parsedData.header().from().str();
    std::string subject = parsedData.header().subject();
    mimetic::MimeEntity textEntity;
    getMIMETextEntity(&parsedData, textEntity);
    std::string bodyText = textEntity.body();

    if (_filterEval.length() > 0) {
        QJSEngine engine;
        engine.installExtensions(QJSEngine::ConsoleExtension);
        QJSValueList args;
        args << QString::fromStdString(from) << QString::fromStdString(subject) << QString::fromStdString(bodyText);
        QJSValue filterRes = engine.evaluate("(function(from, subject, bodyText) {" + QString::fromStdString(_filterEval) + " })").call(args);
        if (!filterRes.isString() || filterRes.isError()) return defaultFilter(from, subject, bodyText);     // guard here because QString toString() and toStdString() are demanding operations
        std::string res = filterRes.toString().toStdString();
        if (folderExists(res) && getFolders()[res] == true) return res;
    }
#endif
    return defaultFilter(from, subject, bodyText);
}

std::string ClientMailbox::defaultFilter(const std::string& _from, const std::string& _subject, const std::string& _bodyText) {
    std::stringstream regExpr;
    std::regex p_r;
    ordered_json& folders = getFolders();
    for (auto& folderObject : folders.items()) {
        if (folderObject.value()["enabled"] == false) continue;

        if (folderObject.value()["from"].size() > 0) {
            for (int i = 0; i < folderObject.value()["from"].size(); i++) {
                std::string match = folderObject.value()["from"][i];
                regExpr << match << (i != folderObject.value()["from"].size() - 1) ? "|" : "";
            }
            p_r = std::regex(regExpr.str());
            if (std::regex_search(_from, p_r)) {
                return folderObject.key();
            }
            regExpr.str("");
            regExpr.clear();
        }
        if (folderObject.value()["subjectKeyword"].size() > 0) {
            for (int i = 0; i < folderObject.value()["subjectKeyword"].size(); i++) {
                std::string match = folderObject.value()["subjectKeyword"][i];
                regExpr << match << ((i != folderObject.value()["subjectKeyword"].size() - 1) ? "|" : "");
            }
            p_r = std::regex(regExpr.str());
            if (std::regex_search(_subject, p_r)) {
                return folderObject.key();
            }
            regExpr.str("");
            regExpr.clear();
        }
        if (folderObject.value()["bodyKeyword"].size() > 0) {
            for (int i = 0; i < folderObject.value()["bodyKeyword"].size(); i++) {
                std::string match = folderObject.value()["bodyKeyword"][i];
                regExpr << match << (i != folderObject.value()["bodyKeyword"].size() - 1) ? "|" : "";
            }
            p_r = std::regex(regExpr.str());
            if (std::regex_search(_bodyText, p_r)) {
                return folderObject.key();
            }
        }
    }
    return "Inbox";
}

void ClientMailbox::sendContent(std::string _host, std::string _port, MailContent& _mailContent) {
    SMTPClient smtpClient(_host, _port);
    if (!smtpClient.connected()) {
        _error = "-Cannot connect to SMTP Server";
        return;
    }
    smtpClient.sequence(_mailContent);
    _error = smtpClient._error;
}

void ClientMailbox::save() {
    std::filesystem::path localDataPath = std::string("mailboxes/" + _cred.getAddress().getReceiptAddress());
    std::ofstream configFile(localDataPath.append(".mailboxCfg.json"));
    configFile << std::setw(4) << _data << std::endl;
    configFile.close();
}

ClientMailbox::~ClientMailbox() {
    save();
}