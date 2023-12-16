#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <map>
#include <memory>

class MIMESegment;
class MIMEHeader;
class MIMEHeaderField;
class MIMEBody;


class MIMEHeaderField {
private:
    std::string _key;
    std::string _vl;
    std::string _full;
    std::map<std::string, std::string> _params;
public:
    MIMEHeaderField();
    MIMEHeaderField(std::string, std::string);
    std::string key();
    std::string full();
    std::string value();
    std::string param(std::string);
    void key(std::string);
    void full(std::string);
    void value(std::string);
    void param(std::string, std::string);

    void parseValue(std::string);
};

class MIMEHeader {
private:
    std::map<std::string, MIMEHeaderField> _headerFields{};
public:
    MIMEHeader();
    
    void parseLine(std::string);

    MIMEHeaderField get(std::string);
    void set(std::string, std::string);
    bool empty();
    int count();
};

class MIMEBody {
private:
    std::vector<MIMESegment> _parts;
    std::string _rawContent;
public:
    MIMEBody();
    std::vector<MIMESegment>& parts();
    void part(MIMESegment);

    std::string rawContent();
    void rawContent(std::string);
};

class MIMESegment {
private:
    MIMEHeader _header;
    MIMEBody _body;
    std::string _boundary;
    bool _isMultipart;

public:
    //MIMESegment(std::vector<MIMEHeader> = std::vector<MIMEHeader>(), std::string = "", std::string = "");
    MIMESegment(std::string = "");
    void parse(std::string = "");

    MIMEBody& body();
    void body(MIMESegment*);
    
    MIMEHeader& header();

    bool isMultipart();
    void setMultipart(bool);
};