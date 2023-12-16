#include "Encoder.h"

// https://www.w3.org/Tools/Mail-Transcode/mail-transcode.c
std::vector<std::string> Encoder::encode_quoted_printable(std::string _in) {
    std::vector<std::string> _out;
    std::stringstream out;
    int n = 0;
    for (const char& c : _in) {
        if (n >= 75 && c != 10 && c != 13) {
            out << "=";
            _out.push_back(out.str());
            out.str("");
            out.clear();
            n = 0;
        }
        if (c == 10 || c == 13) { out << c; n = 0; }
        else if (c < 32 || c == 61 || c > 126) {
            out << "=" << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (int)c;
            n++;
        }
        else if (c != 32 || (c != 9 && c != 12)) { out << c; n++; }
        else {
            out << "=20";
            n++;
        }
    }
    std::string _pad = out.str();
    if (_pad.length() > 0) _out.push_back(_pad);
    return _out;
}

std::string Encoder::decode_quoted_printable(const std::string& input) {
    QByteArray br = QString::fromStdString(input).toUtf8();
    QByteArray output;
 
    for (int i = 0; i < br.length(); ++i) {
        if (br.at(i) == '=' && (i + 2) < br.length()) {
            if(br.at(i + 1) == '\n') {
                i += 1;
                continue;
            }
            QString strValue = br.mid((++i)++, 2);
            bool converted;
            char character = strValue.toUInt(&converted, 16);

            if (converted)
                output.append(character);
            else
                output.append(QString("=" + strValue).toUtf8());
        }
        else {
            output.append(br.at(i));
        }
    }

    return QString::fromUtf8(output).toStdString();
}