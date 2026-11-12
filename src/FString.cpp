#include "FString.h"
#include <cstdlib>
#include <cstring>

namespace FString {
    char* ToCStr(const FString* str) {
        if (!str || !str->data || str->length <= 0)
            return nullptr;

        int maxBytes = (str->length * 3) + 1;
        char* out = (char*)malloc(maxBytes);
        if (!out) return nullptr;

        int o = 0;
        for (int i = 0; i < str->length - 1; i++) {
            char16_t c = str->data[i];

            // idk im js as confused as you
            if (c <= 0x7F) {
                out[o++] = (char)c;
            } else if (c <= 0x7FF) {
                out[o++] = (char)(0xC0 | (c >> 6));
                out[o++] = (char)(0x80 | (c & 0x3F));
            } else {
                out[o++] = (char)(0xE0 | (c >> 12));
                out[o++] = (char)(0x80 | ((c >> 6) & 0x3F));
                out[o++] = (char)(0x80 | (c & 0x3F));
            }
        }

        out[o] = 0;
        return out;
    }

    FString FromCStr(const char* str) {
        FString fs{};
        if (!str) return fs;

        // count UTF-16 shit
        int utf16len = 0;
        const unsigned char* s = (const unsigned char*)str;
        while (*s) {
            if ((*s & 0x80) == 0) {
                s += 1;
                utf16len += 1;
            } else if ((*s & 0xE0) == 0xC0) {
                s += 2;
                utf16len += 1;
            } else if ((*s & 0xF0) == 0xE0) {
                s += 3;
                utf16len += 1;
            } else {
                s += 4;
                utf16len += 1;
            }
        }

        fs.length = fs.capacity = utf16len + 1;
        fs.data = (TCHAR*)malloc(fs.capacity * sizeof(TCHAR));
        if (!fs.data) {
            fs.length = fs.capacity = 0;
            return fs;
        }

        // decode that shit proper trust
        int i = 0;
        s = (const unsigned char*)str;
        while (*s) {
            if ((*s & 0x80) == 0) {
                fs.data[i++] = *s++;
            } else if ((*s & 0xE0) == 0xC0) {
                fs.data[i++] = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
                s += 2;
            } else if ((*s & 0xF0) == 0xE0) {
                fs.data[i++] = ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
                s += 3;
            } else {
                // Skip 4-byte UTF-8 (replace them)
                fs.data[i++] = '?';
                s += 4;
            }
        }

        fs.data[i] = 0;
        return fs;
    }

    // FREEEEE FREEEEEEEEEE
    void Free(FString* str) {
        if (!str)
            return;
        
        if (str->data)
            free(str->data);

        str->data = nullptr;
        str->length = 0;
        str->capacity = 0;
    }
}
