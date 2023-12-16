#include "Parser.h"

MIMESegment::MIMESegment(std::string mime_string) {
    _boundary = "";
    _isMultipart = false;
    _body = MIMEBody();
    parse(mime_string);
}

MIMEBody& MIMESegment::body() {
    return _body;
}

MIMEHeader& MIMESegment::header() {
    return _header;
}

void MIMESegment::parse(std::string in_seg) {
    std::istringstream mimeStream(in_seg);
    std::string line;

    bool setHeader = false;
    std::stringstream tmpRaw;

    while (std::getline(mimeStream, line)) {
        if (line.length() > 0 && line.at(line.length() - 1) == '\r') {
            line.pop_back();
        }
        if (line.empty() && !setHeader) {
            setHeader = true;
            continue;
        }

        if (!setHeader) {
            header().parseLine(line);
            if (line.find("Content-Type: multipart") != std::string::npos) {
                _isMultipart = true;
                size_t pos = line.find("boundary=");
                if (pos != std::string::npos) {
                    _boundary = line.substr(pos + 9); // 9 is the length of "boundary="
                    if (_boundary.front() == '"' && _boundary.back() == '"') {
                        _boundary = _boundary.substr(1, _boundary.length() - 2); // Remove surrounding quotes
                    }
                }
            }
        }
        else {
            tmpRaw << line << "\n";
            if(_isMultipart) {
                size_t pos = line.find("--" + _boundary + "--");
                if(pos != std::string::npos) break;
            }
        }
    }
    std::string rawFin = tmpRaw.str();
    if(rawFin.length() > 0 && rawFin.at(rawFin.length() - 1) == '\n') {
        rawFin.pop_back();
    }
    body().rawContent(rawFin);
    tmpRaw.str("");
    tmpRaw.clear();

    if(_isMultipart) {
        std::stringstream rawstream(body().rawContent());

        bool isSetBoundary = false;
        while (std::getline(rawstream, line)) {
            if(!isSetBoundary) {
                if(line.find("--" + _boundary) == std::string::npos) continue;
                else {
                    isSetBoundary = true;
                }
            } else {
                if(line.find("--" + _boundary) == std::string::npos) {
                    tmpRaw << line << "\n";
                }
                else {
                    rawFin = tmpRaw.str();
                    if (rawFin.length() > 0 && rawFin.at(rawFin.length() - 1) == '\n') {
                        rawFin.pop_back();
                    }
                    MIMESegment childSeg(rawFin);
                    _body.part(childSeg);
                    tmpRaw.str("");
                    tmpRaw.clear();
                }
            }
        }
    }
}

bool MIMESegment::isMultipart() {
    return _isMultipart;
}

void MIMESegment::setMultipart(bool vl) {
    _isMultipart = vl;
}


MIMEBody::MIMEBody() {
    _rawContent = "";
    _parts = std::vector<MIMESegment>();
}

std::vector<MIMESegment>& MIMEBody::parts() {
    return _parts;
}

void MIMEBody::part(MIMESegment part) {
    _parts.push_back(part);
}

std::string MIMEBody::rawContent() {
    return _rawContent;
}

void MIMEBody::rawContent(std::string vl) {
    _rawContent = vl;
}



MIMEHeaderField::MIMEHeaderField() {
    _key = "";
    _vl = "";
    _params = std::map<std::string, std::string>();
}

MIMEHeaderField::MIMEHeaderField(std::string k, std::string vl) {
    _key = k;
    parseValue(vl);
}

std::string MIMEHeaderField::key() {
    return _key;
}

std::string MIMEHeaderField::value() {
    return _vl;
}

std::string MIMEHeaderField::full() {
    return _full;
}

std::string MIMEHeaderField::param(std::string k) {
    auto pos = _params.find(k);
    if(pos == _params.end()) return "";
    return pos->second;
}

void MIMEHeaderField::key(std::string vl) {
    _key = vl;
}

void MIMEHeaderField::value(std::string vl) {
    parseValue(vl);
}

void MIMEHeaderField::full(std::string vl) {
    _full = vl;
}

void MIMEHeaderField::param(std::string k, std::string vl) {
    _params[k] = vl;
}

#include <QDebug>
void MIMEHeaderField::parseValue(std::string vl) {
    std::stringstream iss(vl);
    _full = vl;
    iss >> _vl;
    if(_vl.length() > 0 && _vl.at(_vl.length() - 1) == ';') {
        _vl.pop_back();
    }

    /*
    std::string tValue;
    iss >> tValue;
    int idx = -1;
    if ((idx = tContentType.find_first_of("/")) != std::string::npos) {
        content_type.type(tContentType.substr(0, idx));
        content_type.subtype(tContentType.substr(idx + 1));
    }
    else {
        content_type.type(tContentType);
        content_type.subtype("");
    }
    */

    std::string param_dat;
    while (iss >> param_dat) {
        if (param_dat.length() > 0 && param_dat.at(param_dat.length() - 1) == ';') {
            param_dat.pop_back();
        }
        if (param_dat != ";") {
            size_t eqPos = param_dat.find('=');
            if (eqPos != std::string::npos) {
                std::string vl_t = param_dat.substr(eqPos + 1);
                if(vl_t.length() >= 2 && vl_t[0] == '\"' && vl_t[vl_t.length() - 1] == '\"') {
                    vl_t = vl_t.substr(1, vl_t.length() - 2);
                }
                param(param_dat.substr(0, eqPos), vl_t);
            }
        }
    }
}

void MIMEHeader::parseLine(std::string in_head) {
    size_t pos = in_head.find(":");
    if(pos == std::string::npos) return;
    std::string k = in_head.substr(0, pos);
    std::string vl = in_head.substr(pos + 1);
    set(k, vl);
}

MIMEHeader::MIMEHeader() {
    _headerFields = std::map<std::string, MIMEHeaderField>();
}

void MIMEHeader::set(std::string k, std::string vl) {
    MIMEHeaderField field(k, vl);
    _headerFields[k] = field;
}

MIMEHeaderField MIMEHeader::get(std::string k) {
    auto pos = _headerFields.find(k);
    if(pos == _headerFields.end()) return MIMEHeaderField();
    return pos->second;
}
bool MIMEHeader::empty() {
    return count() == 0;
}

int MIMEHeader::count() {
    return _headerFields.size();
}