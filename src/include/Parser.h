#pragma once

/*

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <map>


class ContentType {
private:
    std::string _type;
    std::string _subtype;
    std::map<std::string, std::string> _params;
public:
    ContentType();
    ContentType(std::string, std::string = "", std::map<std::string, std::string> = std::map<std::string, std::string>());

    std::string type();
    std::string subtype();
    std::string param(std::string);
    void type(std::string);
    void subtype(std::string);
    void param(std::string, std::string);
};

class ContentDispotition {
private:
    std::string _type;
    std::map<std::string, std::string> _params;
public:
    ContentDispotition(std::string, std::map<std::string, std::string> = std::map<std::string, std::string>());

    std::string type();
    std::string param(std::string);
    void type(std::string);
    void param(std::string, std::string);
};

class MIMEHeader {
private:
    std::map<std::string, std::string> _headerFields;
    ContentType _content_type;
public:
    MIMEHeader();
    MIMEHeader(std::string, std::string);
    void parse();
    void parseContentType(const std::string& = "");
    void parseContentDispotition(const std::string& = "");
    void parseField(const std::string&, const std::string& = "");
    ContentType& contentType();
    void contentType(ContentType);
    void contentType(std::string);

    std::string get(std::string);
    void set(std::string, std::string);
    bool empty();
    int count();
};

class MIMESegment {
private:
    MIMEHeader _header;
    std::string _content;
    void headers(MIMEHeader);
public:
    MIMESegment(std::vector<MIMEHeader> = std::vector<MIMEHeader>(), std::string = "", std::string = "");
    
    MIMEHeader& headers();
    std::string content();
    void content(std::string);
};

class MIMEParser {
public:
    MIMEParser(const std::string&);
    std::vector<MIMESegment> parse();
private:
    std::string _mime_string;
};
*/