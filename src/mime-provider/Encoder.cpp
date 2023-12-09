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
            out << "=" << std::setfill('0') << std::setw(2) << std::hex << c;
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


// https://www.w3.org/Tools/Mail-Transcode/mail-transcode.c
std::string Encoder::encode_base64(const std::string& in) {
    std::stringstream out;
    static char encodeChar[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i;
    for (i = 0; i < (in.length() - 2); i += 3) {
        char buffer[5];
        snprintf(buffer, sizeof(buffer), "%c%c%c%c", encodeChar[in[i] >> 2], encodeChar[((in[i] & 0x03) << 4) | (in[i + 1] >> 4)],
            encodeChar[((in[i + 1] & 0x0F) << 2) | (in[i + 2] >> 6)], encodeChar[in[i + 2] & 0x3F]);
        out << buffer;
    }
    if (i >= in.length()) {
        /* No padding needed */
    }
    else if (i == (in.length() - 1)) {
        char buffer[5];
        snprintf(buffer, sizeof(buffer), "%c%c==", encodeChar[in[i] >> 2], encodeChar[((in[i] & 0x03) << 4)]);
        out << buffer;
    }
    else if (i < (in.length() - 1)) {
        char buffer[5];
        snprintf(buffer, sizeof(buffer), "%c%c%c=", encodeChar[in[i] >> 2], encodeChar[((in[i] & 0x03) << 4) | (in[i + 1] >> 4)],
            encodeChar[((in[i + 1] & 0x0F) << 2)]);
        out << buffer;
    }
    return out.str();
}

// http://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp/
std::string Encoder::encode_base64(unsigned char const* bytes_to_encode, size_t in_len) {

    size_t len_encoded = (in_len + 2) / 3 * 4;
    static char encodeChar[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    unsigned char trailing_char = '=';

    std::string ret;
    ret.reserve(len_encoded);

    unsigned int pos = 0;

    while (pos < in_len) {
        ret.push_back(encodeChar[(bytes_to_encode[pos + 0] & 0xfc) >> 2]);

        if (pos + 1 < in_len) {
            ret.push_back(encodeChar[((bytes_to_encode[pos + 0] & 0x03) << 4) + ((bytes_to_encode[pos + 1] & 0xf0) >> 4)]);

            if (pos + 2 < in_len) {
                ret.push_back(encodeChar[((bytes_to_encode[pos + 1] & 0x0f) << 2) + ((bytes_to_encode[pos + 2] & 0xc0) >> 6)]);
                ret.push_back(encodeChar[bytes_to_encode[pos + 2] & 0x3f]);
            }
            else {
                ret.push_back(encodeChar[(bytes_to_encode[pos + 1] & 0x0f) << 2]);
                ret.push_back(trailing_char);
            }
        }
        else {

            ret.push_back(encodeChar[(bytes_to_encode[pos + 0] & 0x03) << 4]);
            ret.push_back(trailing_char);
            ret.push_back(trailing_char);
        }

        pos += 3;
    }

    return ret;
}