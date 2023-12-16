#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <QString>
#include <QByteArray>

namespace Encoder {
    std::vector<std::string> encode_quoted_printable(std::string);
    std::string decode_quoted_printable(const std::string& input);
    std::string encode_base64(const std::string&);
    std::string encode_base64(unsigned char const*, size_t);
    std::string decode_base64(const std::string&, bool);
    int encodeStringToInt(const std::string&);
}

#endif