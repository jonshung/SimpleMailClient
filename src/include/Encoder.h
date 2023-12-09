#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

namespace Encoder {
    std::vector<std::string> encode_quoted_printable(std::string);
    std::string encode_base64(const std::string&);
    std::string encode_base64(unsigned char const*, size_t);
    int encodeStringToInt(const std::string&);
}

#endif