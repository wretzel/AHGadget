#include <Arduino.h>

extern "C" {

// -----------------------------
// Base64 encoding table
// -----------------------------
static const char b64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// -----------------------------
// Compute decoded length
// -----------------------------
int base64_dec_len(const char* input, int length) {
    int padding = 0;
    if (length >= 1 && input[length - 1] == '=') padding++;
    if (length >= 2 && input[length - 2] == '=') padding++;
    return (length * 3) / 4 - padding;
}

// -----------------------------
// Decode Base64 → bytes
// -----------------------------
int base64_decode(char* output, const char* input, int length) {
    int outIndex = 0;
    int i = 0;

    while (i < length) {
        uint32_t val = 0;
        int valb = -8;

        for (int n = 0; n < 4 && i < length; n++, i++) {
            char c = input[i];
            int d;

            if (c >= 'A' && c <= 'Z') d = c - 'A';
            else if (c >= 'a' && c <= 'z') d = c - 'a' + 26;
            else if (c >= '0' && c <= '9') d = c - '0' + 52;
            else if (c == '+') d = 62;
            else if (c == '/') d = 63;
            else if (c == '=') { d = 0; valb = -100; }
            else continue;

            val = (val << 6) | d;
            valb += 6;

            if (valb >= 0) {
                output[outIndex++] = char((val >> valb) & 0xFF);
                valb -= 8;
            }
        }
    }

    return outIndex;
}

// -----------------------------
// Compute encoded length
// -----------------------------
int base64_enc_len(int inputLen) {
    return ((inputLen + 2) / 3) * 4 + 1; // +1 for null terminator
}

// -----------------------------
// Encode bytes → Base64
// -----------------------------
int base64_encode(char* output, const char* input, int inputLen) {
    int outIndex = 0;
    int i = 0;

    while (i < inputLen) {
        uint32_t octet_a = i < inputLen ? (unsigned char)input[i++] : 0;
        uint32_t octet_b = i < inputLen ? (unsigned char)input[i++] : 0;
        uint32_t octet_c = i < inputLen ? (unsigned char)input[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        output[outIndex++] = b64_table[(triple >> 18) & 0x3F];
        output[outIndex++] = b64_table[(triple >> 12) & 0x3F];
        output[outIndex++] = (i - 1 > inputLen) ? '=' : b64_table[(triple >> 6) & 0x3F];
        output[outIndex++] = (i > inputLen)     ? '=' : b64_table[(triple >> 0) & 0x3F];
    }

    output[outIndex] = '\0';
    return outIndex;
}

} // extern "C"
