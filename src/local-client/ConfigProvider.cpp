#include "ConfigProvider.h"
#include <QDebug>

void ConfigProvider::init() {
    if(_configProvider) return;
    _configProvider = std::make_unique<ConfigProvider>();
}

ConfigProvider::ConfigProvider(const std::string& filepath) {
    qDebug() << "configuring...";
    std::filesystem::path syspath = filepath;
    if (filepath.length() > 0) {
        if (!std::filesystem::exists(syspath) && (!syspath.has_filename() ||
            syspath.filename() == "." || syspath.filename() == "..")) {
            std::filesystem::create_directories(syspath);
        }
    }
    if (syspath.filename() != ".mail-client-config.json") {
        syspath.append(".mail-client-config.json");
    }
    std::fstream configHandler;
    _configPath = syspath;
    if (!std::filesystem::is_regular_file(syspath)) {
        qDebug() << "creating configure file at: " << QString::fromStdString(_configPath);
        configHandler.open(syspath, std::fstream::out);
        ordered_json defaultConfig;
        defaultConfig["User"] = "";
        defaultConfig["Address"] = "";
        defaultConfig["Password"] = "";
        defaultConfig["SMTP Server"] = "localhost";
        defaultConfig["SMTP Port"] = "2225";
        defaultConfig["POP3 Server"] = "localhost";
        defaultConfig["POP3 Port"] = "2226";
        defaultConfig["Custom filter"] = "";
        defaultConfig["Autofetch"] = true;
        defaultConfig["Autofetch time"] = 3 * 60;
        defaultConfig["Max file size"] = 20 * 1024 * 1024;

        _configData = defaultConfig;
        configHandler << std::setw(4) << defaultConfig << std::endl;
        configHandler.close();
    }
    else {
        configHandler.open(syspath, std::fstream::in);
        if (!configHandler.is_open()) {
            _error = "Cannot open config file";
            return;
        }
        configHandler >> _configData;
        configHandler.close();
        dataIntegrity();
    }

}

void ConfigProvider::save() {
    std::ofstream configFile(_configPath);
    if (!configFile.is_open()) {
        _error = "Cannot open config file";
        return;
    }
    configFile << std::setw(4) << _configData << std::endl;
    configFile.close();
}

void ConfigProvider::dataIntegrity() {
    if (!_configData["User"].is_string()) {
        _error = "field user is in incorrect type, expected: string";
        _configData["User"] = "";
    }
    if (!_configData["Address"].is_string()) {
        _error = "field address is in incorrect type, expected: string";
        _configData["Address"] = "";
    }
    if (!_configData["Password"].is_string()) {
        _error = "field password is in incorrect type, expected: string";
        _configData["Password"] = "";
    }
    if (!_configData["SMTP Server"].is_string()) {
        _error = "field SMTP Server is in incorrect type, expected: string";
        _configData["SMTP Server"] = "";
    }
    if (!_configData["SMTP Port"].is_string()) {
        _error = "field SMTP Port is in incorrect type, expected: string";
        _configData["SMTP Port"] = "";
    }
    if (!_configData["POP3 Server"].is_string()) {
        _error = "field POP3 Server is in incorrect type, expected: string";
        _configData["POP3 Server"] = "";
    }
    if (!_configData["POP3 Port"].is_string()) {
        _error = "field POP3 Port is in incorrect type, expected: string";
        _configData["POP3 Port"] = "";
    }
    if (!_configData["Autofetch"].is_boolean()) {
        _error = "field Autofetch is in incorrect type, expected: string";
        _configData["Autofetch"] = true;
    }
    if (!_configData["Autofetch time"].is_number()) {
        _error = "field Autofetch time is in incorrect type, expected: string";
        _configData["Autofetch time"] = 5 * 60;
    }
    if (!_configData["Max file size"].is_number()) {
        _error = "field Max file size is in incorrect type, expected: string";
        _configData["Max file size"] = 5 * 60;
    }
}

std::string ConfigProvider::user() {
    return _configData["User"];
}

std::string ConfigProvider::address() {
    return _configData["Address"];
}

std::string ConfigProvider::password() {
    return _configData["Password"];
}

std::string ConfigProvider::SMTPServer() {
    return _configData["SMTP Server"];
}

std::string ConfigProvider::SMTPPort() {
    return _configData["SMTP Port"];
}

std::string ConfigProvider::POP3Server() {
    return _configData["POP3 Server"];
}

std::string ConfigProvider::customFilter() {
    return _configData["Custom filter"];
}

std::string ConfigProvider::POP3Port() {
    return _configData["POP3 Port"];
}

bool ConfigProvider::autofetch() {
    return _configData["Autofetch"];
}

int ConfigProvider::autofetchTime() {
    return _configData["Autofetch time"];
}

unsigned int ConfigProvider::maxFileSize() {
    return _configData["Max file size"];
}


void ConfigProvider::setUser(std::string _vl) {
    _configData["User"] = _vl;
}

void ConfigProvider::setAddress(std::string _vl) {
    _configData["Address"] = _vl;
}

void ConfigProvider::setPassword(std::string _vl) {
    _configData["Password"] = _vl;
}

void ConfigProvider::setSMTPServer(std::string _vl) {
    _configData["SMTP Server"] = _vl;
}

void ConfigProvider::setSMTPPort(std::string _vl) {
    _configData["SMTP Port"] = _vl;
}

void ConfigProvider::setPOP3Server(std::string _vl) {
    _configData["POP3 Server"] = _vl;
}

void ConfigProvider::setPOP3Port(std::string _vl) {
    _configData["POP3 Port"] = _vl;
}

void ConfigProvider::setCustomFilter(std::string _vl) {
    _configData["Custom filter"] = _vl;
}

void ConfigProvider::setAutofetch(bool _vl) {
    _configData["Autofetch"] = _vl;
}

void ConfigProvider::setAutofetchTime(int _vl) {
    _configData["Autofetch time"] = _vl;
}

void ConfigProvider::setMaxFileSize(unsigned int _vl) {
    _configData["Max file size"] = _vl;
}

ConfigProvider::~ConfigProvider() {
    save();
}