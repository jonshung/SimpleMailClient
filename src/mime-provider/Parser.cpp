#include "Parser.h"

/*
MIMEParser::MIMEParser(const std::string& mime_string) : _mime_string(mime_string) {
    parse();
}

std::vector<MIMESegment> MIMEParser::parse() {
    std::vector<MIMESegment> segments;
    std::istringstream mimeStream(_mime_string);
    std::string line;
    std::string boundary;

    MIMESegment segment;
    while (std::getline(mimeStream, line)) {
        if (line.empty()) {
            if (!segment.headers().empty() && !segment.content().empty()) {
                segment.headers().parse();
                segments.push_back(segment);
                segment = MIMESegment();
            }
            continue;
        }
        if (line.find("Content-Type: multipart") != std::string::npos) {
            size_t pos = line.find("boundary=");
            if (pos != std::string::npos) {
                boundary = line.substr(pos + 9); // 9 is the length of "boundary="
                if (boundary.front() == '"' && boundary.back() == '"') {
                    boundary = boundary.substr(1, boundary.length() - 2); // Remove surrounding quotes
                }
            }
        }
        else if (line.find("--" + boundary) != std::string::npos) {
            if (!segment.headers().empty() && !segment.content().empty()) {
                segment.headers().parseContentType();
                segments.push_back(segment);
                segment = MIMESegment();
            }
        }
        else if (line.find("Content-Type:") != std::string::npos) {
            size_t pos = line.find(":");
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            segment.headers().set(key, value);
        }
        else {
            segment.content(segment.content() + line + "\n");
        }
    }
    return segments;
}

void MIMEHeader::parse() {
    parseContentType();
    parseContentDispotition();
}

void MIMEHeader::parseField(const std::string& k, const std::string& vl) {
    ContentType content_type;
    std::string contentTypeValue = vl;
    if (vl.length() < 0) {
        for (const auto& header : _headerFields) {
            if (std::get<0>(header) == "Content-Type") {
                contentTypeValue = std::get<1>(header);
            }
        }
    }

    std::stringstream iss(contentTypeValue);
    std::string tContentType;
    iss >> tContentType;
    int idx = -1;
    if ((idx = tContentType.find_first_of("/")) != std::string::npos) {
        content_type.type(tContentType.substr(0, idx));
        content_type.subtype(tContentType.substr(idx + 1));
    }
    else {
        content_type.type(tContentType);
        content_type.subtype("");
    }
    std::string param;
    while (iss >> param) {
        if (param != ";") {
            size_t eqPos = param.find('=');
            if (eqPos != std::string::npos) {
                content_type.param(param.substr(0, eqPos), param.substr(eqPos + 1));
            }
        }
    }
}

void MIMEHeader::parseContentType(const std::string& vl) {
    
    contentType(content_type);
}

ContentType::ContentType() {
    _type = "";
    _subtype = "";
    _params = std::map<std::string, std::string>();
}

ContentType::ContentType(std::string type, std::string subtype, std::map<std::string, std::string> params) : _type(type), _subtype(subtype), _params(params) {}

std::string ContentType::type() {
    return _type;
}

std::string ContentType::subtype() {
    return _subtype;
}

std::string ContentType::param(std::string key) {
    try {
        return _params.at(key);
    }
    catch (const std::exception& e) {
        return "";
    }
}

void ContentType::type(std::string vl) {
    _type = vl;
}

void ContentType::subtype(std::string vl) {
    _subtype = vl;
}

void ContentType::param(std::string k, std::string vl) {
    _params[k] = vl;
}


bool MIMEHeader::empty() {
    return count() == 0;
}

int MIMEHeader::count() {
    _headerFields.size();
}


void MIMEHeader::contentType(ContentType vl) {
    _content_type = vl;
}

void MIMEHeader::contentType(std::string vl) {
    parseContentType(vl);
}


std::string ContentDispotition::type() {
    return _type;
}

std::string ContentDispotition::param(std::string k) {
    try {
        return _params.at(k);
    } catch(const std::exception& e) {
        return "";
    }
}

void ContentDispotition::type(std::string vl) {
    _type = vl;
}

void ContentDispotition::param(std::string k, std::string vl) {
    _params[k] = vl;
}*/