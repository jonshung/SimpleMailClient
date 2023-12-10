#include "MIMEInterface.h"

/*

 /$$      /$$           /$$ /$$ /$$                            /$$$$$$        /$$       /$$
| $$$    /$$$          |__/| $$| $$                           /$$__  $$      | $$      | $$
| $$$$  /$$$$  /$$$$$$  /$$| $$| $$$$$$$   /$$$$$$  /$$   /$$| $$  \ $$  /$$$$$$$  /$$$$$$$  /$$$$$$   /$$$$$$   /$$$$$$$ /$$$$$$$
| $$ $$/$$ $$ |____  $$| $$| $$| $$__  $$ /$$__  $$|  $$ /$$/| $$$$$$$$ /$$__  $$ /$$__  $$ /$$__  $$ /$$__  $$ /$$_____//$$_____/
| $$  $$$| $$  /$$$$$$$| $$| $$| $$  \ $$| $$  \ $$ \  $$$$/ | $$__  $$| $$  | $$| $$  | $$| $$  \__/| $$$$$$$$|  $$$$$$|  $$$$$$
| $$\  $ | $$ /$$__  $$| $$| $$| $$  | $$| $$  | $$  >$$  $$ | $$  | $$| $$  | $$| $$  | $$| $$      | $$_____/ \____  $$\____  $$
| $$ \/  | $$|  $$$$$$$| $$| $$| $$$$$$$/|  $$$$$$/ /$$/\  $$| $$  | $$|  $$$$$$$|  $$$$$$$| $$      |  $$$$$$$ /$$$$$$$//$$$$$$$/
|__/     |__/ \_______/|__/|__/|_______/  \______/ |__/  \__/|__/  |__/ \_______/ \_______/|__/       \_______/|_______/|_______/

*/

MailboxAddress::MailboxAddress() {
    _rcptName = "";
    _rcptAddress = "";
}

MailboxAddress::MailboxAddress(std::string _rName, std::string _rAddress) {
    _rcptName = _rName;
    _rcptAddress = _rAddress;
}

std::string MailboxAddress::getReceiptName() {
    return _rcptName;
}

std::string MailboxAddress::getReceiptAddress() {
    return _rcptAddress;
}

void MailboxAddress::setReceiptName(std::string _vl) {
    _rcptName = _vl;
}

void MailboxAddress::setReceiptAddress(std::string _vl) {
    _rcptAddress = _vl;
}


/*

 /$$      /$$ /$$$$$$ /$$      /$$ /$$$$$$$$        /$$$$$$   /$$$$$$  /$$      /$$ /$$$$$$$   /$$$$$$  /$$   /$$ /$$$$$$$$ /$$   /$$ /$$$$$$$$ /$$$$$$
| $$$    /$$$|_  $$_/| $$$    /$$$| $$_____/       /$$__  $$ /$$__  $$| $$$    /$$$| $$__  $$ /$$__  $$| $$$ | $$| $$_____/| $$$ | $$|__  $$__//$$__  $$
| $$$$  /$$$$  | $$  | $$$$  /$$$$| $$            | $$  \__/| $$  \ $$| $$$$  /$$$$| $$  \ $$| $$  \ $$| $$$$| $$| $$      | $$$$| $$   | $$  | $$  \__/
| $$ $$/$$ $$  | $$  | $$ $$/$$ $$| $$$$$         | $$      | $$  | $$| $$ $$/$$ $$| $$$$$$$/| $$  | $$| $$ $$ $$| $$$$$   | $$ $$ $$   | $$  |  $$$$$$
| $$  $$$| $$  | $$  | $$  $$$| $$| $$__/         | $$      | $$  | $$| $$  $$$| $$| $$____/ | $$  | $$| $$  $$$$| $$__/   | $$  $$$$   | $$   \____  $$
| $$\  $ | $$  | $$  | $$\  $ | $$| $$            | $$    $$| $$  | $$| $$\  $ | $$| $$      | $$  | $$| $$\  $$$| $$      | $$\  $$$   | $$   /$$  \ $$
| $$ \/  | $$ /$$$$$$| $$ \/  | $$| $$$$$$$$      |  $$$$$$/|  $$$$$$/| $$ \/  | $$| $$      |  $$$$$$/| $$ \  $$| $$$$$$$$| $$ \  $$   | $$  |  $$$$$$/
|__/     |__/|______/|__/     |__/|________/       \______/  \______/ |__/     |__/|__/       \______/ |__/  \__/|________/|__/  \__/   |__/   \______/

*/

MIMEPart::MIMEPart() {
    setContentType("");
    setEncodeType("");
    _dataPart = "";
}

MIMEPart::MIMEPart(std::string _cType, std::string _eType, std::string _dat) {
    setContentType(_cType);
    setEncodeType(_eType);
    _dataPart = _dat;
}

std::string MIMEPart::getContentType() {
    return getHeader("Content-Type");
}

std::string MIMEPart::getEncodeType() {
    return getHeader("Content-Transfer-Encoding");
}

std::string MIMEPart::getData() {
    return _dataPart;
}

void MIMEPart::setContentType(std::string _vl) {
    _headers["Content-Type"] = _vl;
}

void MIMEPart::setEncodeType(std::string _vl) {
    _headers["Content-Transfer-Encoding"] = _vl;
}

void MIMEPart::setData(std::string _vl) {
    _dataPart = _vl;
}

void MIMEPart::setHeader(std::string _key, std::string _vl) {
    _headers[_key] = _vl;
}

std::string MIMEPart::getHeader(std::string _key) {
    auto it = _headers.find(_key);
    if(it == _headers.end()) return "";
    else return (*it).second;
}

std::unordered_map<std::string, std::string> MIMEPart::getHeaders() {
    return _headers;
}

std::string MIMEPart::buildHeader() {
    std::stringstream builder;
    for (std::pair<std::string, std::string> field : getHeaders()) {
        if(field.second.length() == 0) continue;
        builder << field.first << ": " << field.second << "\r\n";
    }
    return builder.str();
}

std::string MIMEPart::build() {
    std::stringstream builder;
    builder << buildHeader();
    builder << "\r\n";

    if (_dataPart.length() > 0) {
        builder << _dataPart;
        if (_dataPart.substr(_dataPart.length() - 2) != "\r\n") {
            builder << "\r\n";
        }
    }
    else {
        builder << "\r\n";
    }
    builder << "\r\n";
    return builder.str();
}



MIMEAttachment::MIMEAttachment() : MIMEPart("application/octet-stream; name=\"\"", "base64") {
    _filename = "";
    _description = "";
    _trueName = "";
    setHeader("Content-Disposition", "attachment; filename=\"\"");
    setHeader("Content-Description", "");
}

MIMEAttachment::MIMEAttachment(std::string _fileName, std::string _fileDescription,
    std::string _fileTrueName, std::string _cType,
    std::string _dat
) : MIMEPart(_cType + "; name=\"" + _fileTrueName + "\"", "base64", _dat) {
    _filename = _fileName;
    _description = _fileDescription;
    _trueName = _fileTrueName;
    setHeader("Content-Disposition", "attachment; filename=\"" + _fileName + "\"");
    setHeader("Content-Description", _fileDescription);
}

std::string MIMEAttachment::getName() {
    return _trueName;
}

std::string MIMEAttachment::getFilename() {
    return _filename;
}

std::string MIMEAttachment::getDescription() {
    return _description;
}

void MIMEAttachment::setName(std::string _vl) {
    _trueName = _vl;
    setContentType(getContentType() + "; name=\"" + _trueName+ "\"");
}

void MIMEAttachment::setFilename(std::string _vl) {
    _filename = _vl;
    setHeader("Content-Disposition", "attachment; filename=\"" + _filename + "\"");
}

void MIMEAttachment::setDescription(std::string _vl) {
    _description = _vl;
    setHeader("Content-Description", _description);
}

std::string MIMEAttachment::build() {
    std::stringstream builder;
    builder << buildHeader();
    builder << "\r\n";

    int _sub = 0;
    while (_sub <= getData().length()) {
        std::string subLine = getData().substr(_sub, 76);
        builder << subLine << "\r\n";
        _sub += 76;
    }
    if (getData().length() == 0) builder << "\r\n";
    builder << "\r\n";
    return builder.str();
}



MIMEText::MIMEText() : MIMEPart("text/plain; charset=\"us-ascii\"", "7bit") {
    _charset = "us-ascii";
}

MIMEText::MIMEText(std::string _tType, std::string _eType, std::string _charType, std::string _dat) : 
    MIMEPart(_tType + "; charset=\"" + _charType + "\"", _eType, _dat) {
    _charset = _charType;
}

std::string MIMEText::getCharset() {
    return _charset;
}

void MIMEText::setCharset(std::string _vl) {
    _charset = _vl;
    setContentType(getContentType() + "; charset=\"" + _charset + "\"");
}

std::string MIMEText::build() {
    std::stringstream builder;
    builder << buildHeader();
    builder << "\r\n";

    size_t pos = 0;
    std::string buffer = getData();
    // content sanitization
    if (getEncodeType() != "base64") {                                          // base64 already encode into a sequence not violating the data terminator
        while ((pos = buffer.find("\n", pos)) != std::string::npos) {
            if (pos > 0 && buffer.at(pos - 1) != '\r') {
                buffer.insert(pos, "\r");
                pos++;
            }
            if ((pos < buffer.length() - 1) && buffer[pos + 1] == '.') buffer.insert(pos + 1, ".");
            pos++;
        }
    }

    if (buffer.size() == 0) {
        builder << "\r\n";
    } else {
        if (getEncodeType() == "quoted-printable") {
            std::vector<std::string> encoded;
            encoded = Encoder::encode_quoted_printable(buffer);
            for (std::string line : encoded) {
                builder << line << "\r\n";
            }
        }
        else {
            int _sub = 0;
            // support by very few user agent, exists purely for experimental usage. Please do not use :)
            if (getEncodeType() == "base64") buffer = Encoder::encode_base64(buffer);
            // subtracting input to fit in with 76 char rule.
            while (_sub <= buffer.length()) {
                std::string subLine = buffer.substr(_sub, 76);
                builder << subLine << "\r\n";
                _sub += 76;
            }
        }
    }
    if (buffer.length() >= 2 && buffer.substr(buffer.length() - 2) != "\r\n") {     // only for plain text
        builder << "\r\n";
    }
    return builder.str();
}



MIMEMultipart::MIMEMultipart() : MIMEPart("", "") {
    setBoundary("");
}

MIMEMultipart::MIMEMultipart(std::string _multipartType, std::string _b) : MIMEPart("", "") {
    _partType = _multipartType;
    setBoundary(_b);
}

std::vector<std::shared_ptr<MIMEPart>> MIMEMultipart::getParts() {
    return _parts;
}

std::string MIMEMultipart::getBoundary() {
    return _boundary;
}

void MIMEMultipart::setBoundary(std::string _vl) {
    _boundary = _vl;
    int i = 0;
    int len = rand_chars.size();
    srand(time(NULL));
    while (i < 16) {
        int idx = rand() % len;
        _boundary.push_back(rand_chars[idx]);
        ++i;
    }
    setContentType(getPartType() + "; boundary=\"" + _boundary + "\"");
}

void MIMEMultipart::addPart(std::shared_ptr<MIMEPart> _vl) {
    _parts.push_back(_vl);
}

std::string MIMEMultipart::build() {
    std::stringstream builder;
    builder << buildHeader();
    builder << "\r\n";

    if (getParts().size() == 0) {
        builder << "\r\n";
    }
    else {
        for (std::shared_ptr<MIMEPart> part : getParts()) {
            builder << "--" << getBoundary() << "\r\n";
            builder << part->build();
        }
        builder << "--" << getBoundary() << "--\r\n";
    }
    return builder.str();
}



/*

 /$$      /$$           /$$ /$$        /$$$$$$                        /$$                           /$$
| $$$    /$$$          |__/| $$       /$$__  $$                      | $$                          | $$
| $$$$  /$$$$  /$$$$$$  /$$| $$      | $$  \__/  /$$$$$$  /$$$$$$$  /$$$$$$    /$$$$$$  /$$$$$$$  /$$$$$$
| $$ $$/$$ $$ |____  $$| $$| $$      | $$       /$$__  $$| $$__  $$|_  $$_/   /$$__  $$| $$__  $$|_  $$_/
| $$  $$$| $$  /$$$$$$$| $$| $$      | $$      | $$  \ $$| $$  \ $$  | $$    | $$$$$$$$| $$  \ $$  | $$
| $$\  $ | $$ /$$__  $$| $$| $$      | $$    $$| $$  | $$| $$  | $$  | $$ /$$| $$_____/| $$  | $$  | $$ /$$
| $$ \/  | $$|  $$$$$$$| $$| $$      |  $$$$$$/|  $$$$$$/| $$  | $$  |  $$$$/|  $$$$$$$| $$  | $$  |  $$$$/
|__/     |__/ \_______/|__/|__/       \______/  \______/ |__/  |__/   \___/   \_______/|__/  |__/   \___/

*/

MailContent::MailContent() {
    _from = MailboxAddress("", "");
    _subject = "";
    _htmlContent = "";
    _plainContent = "";
}

MailContent::MailContent(std::string _fromAddr, std::vector<MailboxAddress> _toAddrs, std::string _fromName, std::string _toName) {
    _to = _toAddrs;
    _from = MailboxAddress(_fromName, _fromAddr);
}

std::vector<MailboxAddress> MailContent::getTo() {
    return _to;
}

MailboxAddress MailContent::getFrom() {
    return _from;
}

std::vector<MailboxAddress> MailContent::getCC() {
    return _cc;
}

std::vector<MailboxAddress> MailContent::getBCC() {
    return _bcc;
}

std::string MailContent::getSubject() {
    return _subject;
}

std::string MailContent::getHTMLContent() {
    return _htmlContent;
}

std::string MailContent::getPlainContent() {
    return _plainContent;
}

std::vector<MIMEAttachment> MailContent::getAttachments() {
    return _attachments;
}

void MailContent::addTo(std::string _name, std::string _addr) {
    _to.push_back(MailboxAddress(_name, _addr));
}

void MailContent::addTo(MailboxAddress _mb) {
    _to.push_back(_mb);
}

void MailContent::setTo(std::vector<MailboxAddress> _l) {
    _to = _l;
}

void MailContent::setFrom(std::string _name, std::string _addr) {
    _from = MailboxAddress(_name, _addr);
}

void MailContent::setFrom(MailboxAddress _mb) {
    _from = _mb;
}

void MailContent::addCC(std::string _name, std::string _addr) {
    _cc.push_back(MailboxAddress(_name, _addr));
}

void MailContent::addCC(MailboxAddress _mb) {
    _cc.push_back(_mb);
}

void MailContent::setCC(std::vector<MailboxAddress> _mbl) {
    _cc = _mbl;
}

void MailContent::addBCC(std::string _name, std::string _addr) {
    _bcc.push_back(MailboxAddress(_name, _addr));
}

void MailContent::addBCC(MailboxAddress _mb) {
    _bcc.push_back(_mb);
}

void MailContent::setBCC(std::vector<MailboxAddress> _mbl) {
    _bcc = _mbl;
}

void MailContent::setSubject(std::string _vl) {
    _subject = _vl;
}

void MailContent::setHTMLContent(std::string _vl) {
    _htmlContent = _vl;
}

void MailContent::setPlainContent(std::string _vl) {
    _plainContent = _vl;
}

void MailContent::addAttachment(MIMEAttachment _att) {
    _attachments.push_back(_att);
}

void MailContent::setAttachments(std::vector<MIMEAttachment> _attl) {
    _attachments = _attl;
}

void MailContent::packHeader(MailContent& _mailContent, MIMEMultipart& _body) {

    /**                             Subject                                   */
    _body.setHeader("Subject", _mailContent.getSubject());

    _body.setHeader("From", "\"" + _mailContent.getFrom().getReceiptName() + "\" <" + _mailContent.getFrom().getReceiptAddress() + ">");
    
    if (_mailContent.getTo().size() > 0) {
        std::stringstream rcptList;
        int n = 4;
        for (int i = 0; i < _mailContent.getTo().size(); i++) {
            MailboxAddress rcpt = _mailContent.getTo()[i];
            std::string sep = "";
            if ((i + 1) == _mailContent.getTo().size()) {
                n += 5;
            } else {
                n += 7;
                sep = ", ";
            }
            n += rcpt.getReceiptName().length() + rcpt.getReceiptAddress().length();

            // should also precheck line and cut it but a limit imposed on mailbox address should be enough to
            // deal with this issue.
            std::string pad = "";
            if (n > 76) {
                pad = "\r\n ";
                n = 1;
            }
            rcptList << "\"" << rcpt.getReceiptName() << "\" <" << rcpt.getReceiptAddress() << ">" << sep << pad;
        }
        _body.setHeader("To", rcptList.str());
    }

    /**                              COPY                                    */
    if (_mailContent.getCC().size() > 0) {
        std::stringstream rcptList;
        int n = 4;
        for (int i = 0; i < _mailContent.getCC().size(); i++) {
            MailboxAddress rcpt = _mailContent.getCC()[i];
            std::string sep = "";
            if ((i + 1) == _mailContent.getCC().size()) {
                n += 5;
            }
            else {
                n += 7;
                sep = ", ";
            }
            n += rcpt.getReceiptName().length() + rcpt.getReceiptAddress().length();

            // should also precheck line and cut it but a limit imposed on mailbox address should be enough to
            // deal with this issue.
            std::string pad = "";
            if (n > 76) {
                pad = "\r\n ";
                n = 1;
            }
            rcptList << "\"" << rcpt.getReceiptName() << "\" <" << rcpt.getReceiptAddress() << ">" << sep << pad;
        }
        _body.setHeader("Cc", rcptList.str());
    }

    /**                              DATE                                    */
    const auto currentTime = std::chrono::system_clock::now();
    const auto timer = std::chrono::system_clock::to_time_t(currentTime);
    std::tm timeRep{};

#if defined(__unix__)
    localtime_r(&timer, &timeRep);
#elif defined(_MSC_VER)
    localtime_s(&timeRep, &timer);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    timeRep = *std::localtime(&timer);
#endif

    std::stringstream dateVl;
    dateVl << std::put_time(&timeRep, "%d %b %Y %H:%M %z");
    _body.setHeader("Date", dateVl.str());
    // required header date and address field
    _body.setHeader("MIME-Version", "1.0");
}


std::string MailContent::buildMessage(MailContent& _mailContent) {
    MIMEMultipart mainBody("multipart/mixed", "main");
    MailContent::packHeader(_mailContent, mainBody);

    /**                         PLAIN TEXT AND HTML DATA                            */
    if (_mailContent.getPlainContent().length() > 0 || _mailContent.getHTMLContent().length() > 0) {
        std::shared_ptr<MIMEMultipart> textPart = std::make_shared<MIMEMultipart>("multipart/alternative", "text-boundary");
        // plain text
        if (_mailContent.getPlainContent().length() > 0) {
            std::shared_ptr<MIMEText> plainContent = std::make_shared<MIMEText>("text/plain", "quoted-printable", "utf-8", _mailContent.getPlainContent());
            textPart->addPart(plainContent);
        }

        // html
        if (_mailContent.getHTMLContent().length() > 0) {
            std::shared_ptr<MIMEText> htmlContent = std::make_shared<MIMEText>("text/html", "quoted-printable", "utf-8", _mailContent.getHTMLContent());
            textPart->addPart(htmlContent);
        }
        mainBody.addPart(textPart);
    }

    /**                                ATTACHMENTS                                  */
    for (MIMEAttachment attachment : _mailContent.getAttachments()) {
        mainBody.addPart(std::make_shared<MIMEAttachment>(attachment));
    }
    std::string msg = mainBody.build();
    if (msg.substr(msg.length() - 2) == "\r\n") {        //multipart final trailing.
        msg.pop_back();
        msg.pop_back();
    }
    return msg;
}

/*

 /$$   /$$ /$$$$$$$$ /$$$$$$ /$$       /$$$$$$ /$$$$$$$$ /$$     /$$
| $$  | $$|__  $$__/|_  $$_/| $$      |_  $$_/|__  $$__/|  $$   /$$/
| $$  | $$   | $$     | $$  | $$        | $$     | $$    \  $$ /$$/
| $$  | $$   | $$     | $$  | $$        | $$     | $$     \  $$$$/
| $$  | $$   | $$     | $$  | $$        | $$     | $$      \  $$/
| $$  | $$   | $$     | $$  | $$        | $$     | $$       | $$
|  $$$$$$/   | $$    /$$$$$$| $$$$$$$$ /$$$$$$   | $$       | $$
 \______/    |__/   |______/|________/|______/   |__/       |__/

*/

MailContent MailContent::parseFromFile(std::string _filePath) {
    std::fstream fileHandler(_filePath, std::fstream::in);
    if (!fileHandler.is_open()) {
        throw std::runtime_error("Cannot open file " + _filePath);
    }
    // to be done
    return MailContent();
}