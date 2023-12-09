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

/*
void printMIMEStructure(mimetic::MimeEntity* e, int tabIndent = 0) {
    for (int c = tabIndent; c > 0; --c) {
        std::cout << " ";
    }
    mimetic::Header& header = e->header();
    std::cout << header.contentType() << std::endl;;
    std::cout << header.contentDisposition() << ((header.contentDisposition().str().length() == 0) ? "" : "\r\n");

    if (header.hasField("X-My-Custom-Header")) {
        mimetic::Field field = header.field("X-My-Custom-Header");
        std::cout << "Custom field: " << field.name()  << ": " << field.value() << std::endl;
    }
    if (header.contentDisposition().type() != "attachment" && !header.contentType().isMultipart()) {
        std::cout << "Body: " << std::endl;
        std::cout << e->body() << std::endl;
    }
    mimetic::MimeEntityList& bodyParts = e->body().parts();
    mimetic::MimeEntityList::iterator mbeg = bodyParts.begin(), mend = bodyParts.end();
    for (; mbeg != mend; ++mbeg) {
        printMIMEStructure(*mbeg);
    }
}*/

mimetic::MimeEntity parseMIMEEntity(std::string _buffer) {
    std::stringstream bufferStream(_buffer);
    mimetic::MimeEntity e(bufferStream);
    return e;
}

bool getMIMETextEntity(mimetic::MimeEntity* e, mimetic::MimeEntity& res) {
    bool found = false;
    if (e->header().contentType().isMultipart()) {
        mimetic::MimeEntityList& bodyParts = e->body().parts();
        mimetic::MimeEntityList::iterator mbeg = bodyParts.begin(), mend = bodyParts.end();
        for (; mbeg != mend; ++mbeg) {
            bool foundTemp = getMIMETextEntity(*mbeg, res);
            if(!found) found = foundTemp;
        }
    } else {
        if(e->header().contentType().type() == "text") {
            std::string prevType = res.header().contentType().subtype();
            std::string currType = e->header().contentType().subtype();
            if(prevType.length() == 0 ||  currType == "html") res = *e;     // prioritize newest content and html
            return true;
        }
    }
    return found;
}

void getMIMEAttachments(mimetic::MimeEntity* e, std::vector<mimetic::MimeEntity*>& res) {
    if (e->header().contentType().isMultipart()) {
        mimetic::MimeEntityList& bodyParts = e->body().parts();
        mimetic::MimeEntityList::iterator mbeg = bodyParts.begin(), mend = bodyParts.end();
        for (; mbeg != mend; ++mbeg) {
            getMIMEAttachments(*mbeg, res);
        }
    }
    else {
        if (e->header().contentDisposition().type() == "attachment") {
            res.push_back(e);
        }
    }
}