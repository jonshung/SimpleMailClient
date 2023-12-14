#ifndef _CONFIG_PROVIDER_H
#define _CONFIG_PROVIDER_H

#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>

using namespace nlohmann;

class ConfigProvider {
private:
    std::string _error{ "" };
    std::filesystem::path _configPath{ "" };
    ordered_json _configData;
public:
    inline static std::unique_ptr<ConfigProvider> _configProvider;
    static void init();
    ConfigProvider(const std::string & = "");
    void dataIntegrity();
    std::string user();
    std::string address();
    std::string password();
    std::string SMTPServer();
    std::string SMTPPort();
    std::string POP3Server();
    std::string POP3Port();
    std::string customFilter();
    bool autofetch();
    int autofetchTime();
    unsigned int maxFileSize();

    void save();

    void setUser(std::string);
    void setAddress(std::string);
    void setPassword(std::string);
    void setSMTPServer(std::string);
    void setSMTPPort(std::string);
    void setPOP3Server(std::string);
    void setPOP3Port(std::string);
    void setCustomFilter(std::string);
    void setAutofetch(bool);
    void setAutofetchTime(int);
    void setMaxFileSize(unsigned int);

    ~ConfigProvider();
};
#endif