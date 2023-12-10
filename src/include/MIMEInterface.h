#ifndef _MIME_INTERFACE_
#define _MIME_INTERFACE_

#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <chrono>

#include "Encoder.h"

static const std::unordered_map<std::string, std::string> __supported = {
    std::make_pair("", "application/octet-stream"),
    std::make_pair("jpg", "image/jpeg"),
    std::make_pair("png", "image/png"),
    std::make_pair("gif", "image/gif")
};

static std::string encodeToString(int);

class MailboxAddress {
private:
    std::string _rcptName{ "" };
    std::string _rcptAddress{ "" };
public:
    MailboxAddress();
    MailboxAddress(std::string, std::string);
    std::string getReceiptName();
    std::string getReceiptAddress();

    void setReceiptName(std::string);
    void setReceiptAddress(std::string);
};

class MIMEPart {
private:
    std::unordered_map<std::string, std::string> _headers;
    std::string _dataPart{ "" };
public:
    MIMEPart();
    MIMEPart(std::string, std::string = "7bit", std::string = "");
    std::string getContentType();
    std::string getEncodeType();
    std::string getData();

    std::string getHeader(std::string);
    std::unordered_map<std::string, std::string> getHeaders();
    void setHeader(std::string, std::string);

    void setContentType(std::string);
    void setEncodeType(std::string);
    void setData(std::string);

    virtual std::string buildHeader();
    virtual std::string build();
};

class MIMEAttachment : public MIMEPart {
private:
    std::string _trueName{ "" };
    std::string _description{ "" };
    std::string _filename{ "" };
public:
    MIMEAttachment();
    MIMEAttachment(std::string, std::string = "", std::string = "", std::string = "application/octet-stream", std::string = "");
    std::string getName();
    std::string getFilename();
    std::string getDescription();

    void setName(std::string);
    void setFilename(std::string);
    void setDescription(std::string);

    std::string build() override;
};

class MIMEText : public MIMEPart {
private:
    std::string _charset{ "" };
public:
    MIMEText();
    MIMEText(std::string, std::string, std::string, std::string = "");
    
    std::string getCharset();
    void setCharset(std::string);

    std::string build() override;
};

class MIMEMultipart : public MIMEPart {
private:
    inline static const std::string rand_chars = std::string("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    std::vector<std::shared_ptr<MIMEPart>> _parts;
    std::string _partType{ "" };
    std::string _boundary{ "" };
public:
    MIMEMultipart();
    MIMEMultipart(std::string, std::string);

    std::vector<std::shared_ptr<MIMEPart>> getParts();
    inline std::string getPartType() { return _partType; }
    std::string getBoundary();

    void setBoundary(std::string);
    void addPart(std::shared_ptr<MIMEPart>);

    std::string build() override;
};

class MailContent {
private:
    std::vector<MailboxAddress> _to;
    MailboxAddress _from;
    std::vector<MailboxAddress> _cc;
    std::vector<MailboxAddress> _bcc;
    std::string _subject{ "" };
    std::string _htmlContent{ "" };
    std::string _plainContent{ "" };
    std::vector<MIMEAttachment> _attachments;

public:
    MailContent();
    MailContent(std::string, std::vector<MailboxAddress> = std::vector<MailboxAddress>(), std::string = "", std::string = "");

    std::vector<MailboxAddress> getTo();
    MailboxAddress getFrom();
    std::vector<MailboxAddress> getCC();
    std::vector<MailboxAddress> getBCC();
    std::string getSubject();
    std::string getHTMLContent();
    std::string getPlainContent();
    std::vector<MIMEAttachment> getAttachments();

    void addTo(std::string, std::string);
    void addTo(MailboxAddress);
    void setTo(std::vector<MailboxAddress>);
    void setFrom(std::string, std::string);
    void setFrom(MailboxAddress);
    void addCC(std::string, std::string);
    void addCC(MailboxAddress);
    void setCC(std::vector<MailboxAddress>);
    void addBCC(std::string, std::string);
    void addBCC(MailboxAddress);
    void setBCC(std::vector<MailboxAddress>);

    void setSubject(std::string);
    void setHTMLContent(std::string);
    void setPlainContent(std::string);
    void addAttachment(MIMEAttachment);
    void setAttachments(std::vector<MIMEAttachment>);

    static MailContent parseFromFile(std::string);
    static void packHeader(MailContent&, MIMEMultipart&);
    static std::string buildMessage(MailContent&);
};

#endif