#include "MIMEParser.h"

std::string fileToBase64(const std::string& filePath, const int& _limit) {
    std::fstream fileHandler(filePath, std::fstream::binary | std::fstream::in);
    if (!fileHandler.is_open()) {
        return "";
    }

    fileHandler.seekg(0, fileHandler.end);
    int length = fileHandler.tellg();
    fileHandler.seekg(0, fileHandler.beg);

    if (length > _limit) {
        return "";
    }

    char* _buffer = new char[length];
    if (!fileHandler.read(_buffer, length)) {
        return "";
    }

    std::string encoded = Encoder::encode_base64(reinterpret_cast<const unsigned char*>(_buffer), length);
    delete _buffer;
    fileHandler.close();
    return encoded;
}

MIMEAttachment mimeFromFile(const std::string& _filePath, const std::string& _desc, const int& _limit) {
    std::size_t foundPos = _filePath.find_last_of("/\\");
    std::string path = _filePath.substr(0, foundPos);
    std::string fileFullName = _filePath.substr(foundPos + 1);
    std::size_t extPos = fileFullName.find_last_of(".");
    std::string ext = fileFullName.substr(extPos + 1);
    std::string fileName = fileFullName.substr(0, extPos);
    if (__supported.find(ext) == __supported.end()) {
        ext = "";
    }

    std::string encoded = fileToBase64(_filePath, _limit);
    if (encoded == "") {
        return MIMEAttachment("");
    }

    MIMEAttachment _mime(fileFullName, _desc, fileName, __supported.at(ext), encoded);
    return _mime;
}

MIMESegment parseMIMEEntity(std::string _buffer) {
    MIMESegment e(_buffer);
    return e;
}

bool getMIMETextEntity(MIMESegment* e, MIMESegment& res) {
    bool found = false;
    if (e->isMultipart()) {
        std::vector<MIMESegment> bodyParts = e->body().parts();
        std::vector<MIMESegment>::iterator mbeg = bodyParts.begin(), mend = bodyParts.end();
        for (; mbeg != mend; ++mbeg) {
            bool foundTemp = getMIMETextEntity(&(*mbeg), res);
            if(!found) found = foundTemp;
        }
    } else {
        if (e->header().get("Content-Type").value().find("text") != std::string::npos) {
            std::string prevType;
            std::string currType;

            std::string currCtType = e->header().get("Content-Type").value();
            size_t s1Pos = currCtType.find("/");
            currType = "";
            if(s1Pos != std::string::npos) {
                currType = currCtType.substr(s1Pos + 1);
            }
            std::string prevCtType = res.header().get("Content-Type").value();
            size_t s2Pos = prevCtType.find("/");
            prevType = "";
            if (s2Pos != std::string::npos) {
                prevType = prevCtType.substr(s2Pos + 1);
            }
            if(prevType.length() == 0 ||  currType == "html") res = *e;     // prioritize newest content and html
            return true;
        }
    }
    return found;
}

std::string extractSubject(MIMESegment* e) {
    return e->header().get("Subject").full();
}

void getMIMEAttachments(MIMESegment e, std::vector<MIMESegment>& res) {
    if (e.isMultipart()) {
        std::vector<MIMESegment> bodyParts = e.body().parts();
        std::vector<MIMESegment>::iterator mbeg = bodyParts.begin(), mend = bodyParts.end();

        for (MIMESegment seg : bodyParts) {
            getMIMEAttachments(seg, res);
        }
    }
    else {
        if (e.header().get("Content-Disposition").value() == "attachment") {
            res.push_back(e);
        }
    }
}