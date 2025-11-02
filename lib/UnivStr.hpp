#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <bit>
#include <array>

namespace utf {
    // UTF-8 to UTF-32 conversion
    inline std::u32string to_utf32(std::string_view utf8) {
        std::u32string result;
        result.reserve(utf8.size()); // at least this many code points

        for (size_t i = 0; i < utf8.size();) {
            char32_t codepoint = 0;
            unsigned char first = static_cast<unsigned char>(utf8[i]);
            
            if (first < 0x80) {
                // ASCII
                codepoint = first;
                i += 1;
            } else if ((first & 0xE0) == 0xC0 && i + 1 < utf8.size()) {
                // 2-byte sequence
                codepoint = ((first & 0x1F) << 6) |
                           (static_cast<unsigned char>(utf8[i + 1]) & 0x3F);
                i += 2;
            } else if ((first & 0xF0) == 0xE0 && i + 2 < utf8.size()) {
                // 3-byte sequence
                codepoint = ((first & 0x0F) << 12) |
                           ((static_cast<unsigned char>(utf8[i + 1]) & 0x3F) << 6) |
                           (static_cast<unsigned char>(utf8[i + 2]) & 0x3F);
                i += 3;
            } else if ((first & 0xF8) == 0xF0 && i + 3 < utf8.size()) {
                // 4-byte sequence
                codepoint = ((first & 0x07) << 18) |
                           ((static_cast<unsigned char>(utf8[i + 1]) & 0x3F) << 12) |
                           ((static_cast<unsigned char>(utf8[i + 2]) & 0x3F) << 6) |
                           (static_cast<unsigned char>(utf8[i + 3]) & 0x3F);
                i += 4;
            } else {
                // Invalid sequence, skip byte
                i += 1;
                continue;
            }
            
            result.push_back(codepoint);
        }
        
        return result;
    }

    // UTF-32 to UTF-8 conversion
    inline std::string to_utf8(std::u32string_view utf32) {
        std::string result;
        result.reserve(utf32.size() * 4); // maximum possible size

        for (char32_t codepoint : utf32) {
            if (codepoint < 0x80) {
                // ASCII
                result.push_back(static_cast<char>(codepoint));
            } else if (codepoint < 0x800) {
                // 2-byte sequence
                result.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
                result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
            } else if (codepoint < 0x10000) {
                // 3-byte sequence
                result.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
                result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
            } else if (codepoint < 0x110000) {
                // 4-byte sequence
                result.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
                result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
            }
        }
        
        return result;
    }
}

class UnivStr : public std::u32string {
public:
    using std::u32string::u32string;
    // expose base assignment operators so assigning from std::u32string still works
    using std::u32string::operator=;
    // allow concatenation with +=
    using std::u32string::operator+=;
    
    // Constructor from std::u32string
    UnivStr(const std::u32string& str) : std::u32string(str) {}
    
    // Assign from UTF-8 string
    UnivStr& operator=(const std::string& utf8_str) {
        static_cast<std::u32string&>(*this) = utf::to_utf32(utf8_str);
        return *this;
    }
    
    // Construct from UTF-8 string
    explicit UnivStr(const std::string& utf8_str) 
        : std::u32string(utf::to_utf32(utf8_str)) {}
    
    // Convert to UTF-8
    [[nodiscard]] std::string to_utf8() const {
        return utf::to_utf8(*this);
    }
    
    // Factory method for UTF-8 conversion
    [[nodiscard]] static UnivStr from_utf8(std::string_view utf8_str) {
        return UnivStr(std::string(utf8_str));
    }
};

// Free function operators for string concatenation
inline UnivStr operator+(const UnivStr& lhs, const UnivStr& rhs) {
    return UnivStr(static_cast<const std::u32string&>(lhs) + static_cast<const std::u32string&>(rhs));
}

inline UnivStr operator+(const UnivStr& lhs, const char32_t* rhs) {
    return UnivStr(static_cast<const std::u32string&>(lhs) + rhs);
}

inline UnivStr operator+(const char32_t* lhs, const UnivStr& rhs) {
    return UnivStr(lhs + static_cast<const std::u32string&>(rhs));
}

inline UnivStr operator+(const UnivStr& lhs, char32_t rhs) {
    return UnivStr(static_cast<const std::u32string&>(lhs) + rhs);
}

inline UnivStr operator+(char32_t lhs, const UnivStr& rhs) {
    return UnivStr(std::u32string(1, lhs) + static_cast<const std::u32string&>(rhs));
}

// Stream operators for UTF-8 input/output
inline std::ostream& operator<<(std::ostream& os, const UnivStr& str) {
    return os << str.to_utf8();
}

inline std::istream& operator>>(std::istream& is, UnivStr& str) {
    std::string utf8_str;
    is >> utf8_str;  // Read UTF-8 string
    str = UnivStr(utf8_str);  // Convert to UTF-32 using constructor
    return is;
}

// getline for reading full lines of UTF-8 text
inline std::istream& getline(std::istream& is, UnivStr& str) {
    std::string utf8_str;
    if (std::getline(is, utf8_str)) {
        str = UnivStr(utf8_str);
    } else {
        is.setstate(std::ios::failbit);
    }
    return is;
}

// Overload for char delimiters (common case for ASCII delimiters)
inline std::istream& getline(std::istream& is, UnivStr& str, char delim) {
    std::string utf8_str;
    if (std::getline(is, utf8_str, delim)) {
        str = UnivStr(utf8_str);
    } else {
        is.setstate(std::ios::failbit);
    }
    return is;
}
