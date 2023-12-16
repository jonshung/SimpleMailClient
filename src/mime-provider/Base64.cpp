#include "Encoder.h"

static char encodeChar[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static unsigned int pos_of_char(const unsigned char chr) {
    if (chr >= 'A' && chr <= 'Z') return chr - 'A';
    else if (chr >= 'a' && chr <= 'z') return chr - 'a' + ('Z' - 'A') + 1;
    else if (chr >= '0' && chr <= '9') return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
    else if (chr == '+' || chr == '-') return 62;
    else if (chr == '/' || chr == '_') return 63;
    return 0;
}

// https://www.w3.org/Tools/Mail-Transcode/mail-transcode.c
std::string Encoder::encode_base64(const std::string& in) {
    std::stringstream out;
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

std::string Encoder::decode_base64(const std::string& encoded_string, bool remove_linebreaks) {
    if (encoded_string.empty()) return std::string();
    if (remove_linebreaks) {
        std::string copy(encoded_string);
        copy.erase(std::remove(copy.begin(), copy.end(), '\n'), copy.end());
        return decode_base64(copy, false);
    }

    size_t length_of_string = encoded_string.length();
    size_t pos = 0;

    size_t approx_length_of_decoded_string = length_of_string / 4 * 3;
    std::string ret;
    ret.reserve(approx_length_of_decoded_string);

    while (pos < length_of_string) {
        size_t pos_of_char_1 = pos_of_char(encoded_string.at(pos + 1));

        ret.push_back(static_cast<std::string::value_type>(((pos_of_char(encoded_string.at(pos + 0))) << 2) + ((pos_of_char_1 & 0x30) >> 4)));

        if ((pos + 2 < length_of_string) &&  // Check for data that is not padded with equal signs (which is allowed by RFC 2045)
            encoded_string.at(pos + 2) != '=' &&
            encoded_string.at(pos + 2) != '.'
            ) {
            unsigned int pos_of_char_2 = pos_of_char(encoded_string.at(pos + 2));
            ret.push_back(static_cast<std::string::value_type>(((pos_of_char_1 & 0x0f) << 4) + ((pos_of_char_2 & 0x3c) >> 2)));

            if ((pos + 3 < length_of_string) &&
                encoded_string.at(pos + 3) != '=' &&
                encoded_string.at(pos + 3) != '.'
                ) {
                ret.push_back(static_cast<std::string::value_type>(((pos_of_char_2 & 0x03) << 6) + pos_of_char(encoded_string.at(pos + 3))));
            }
        }
        pos += 4;
    }
    return ret;
}