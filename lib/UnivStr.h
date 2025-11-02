#pragma once

#include <locale>
#include <string>
#include <codecvt>
#include <string_view>
#include <iostream>


class UnivStr : public std::u32string {
public:
    using std::u32string::u32string;
    // expose base assignment operators so assigning from std::u32string still works
    using std::u32string::operator=;
    // allow concatenation with +=
    using std::u32string::operator+=;
    
    // Constructor from std::u32string
    UnivStr(const std::u32string& str) : std::u32string(str) {}
    UnivStr& operator=(const std::string& utf8_str) {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        // assign into the base std::u32string to avoid calling this overload recursively
        std::u32string tmp = convert.from_bytes(utf8_str);
        static_cast<std::u32string&>(*this) = std::move(tmp);
        return *this;
    }
    // construct from a UTF-8 std::string
    UnivStr(const std::string& utf8_str) {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        std::u32string tmp = convert.from_bytes(utf8_str);
        static_cast<std::u32string&>(*this) = std::move(tmp);
    }
    static UnivStr from_utf8(const std::string& utf8_str) {
        // reuse the UTF-8 constructor above to perform conversion
        return UnivStr(utf8_str);
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

// Convert to UTF-8 for output
inline std::string to_utf8(const UnivStr& str) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    return convert.to_bytes(str);
}

// Stream operators for UTF-8 input/output
inline std::ostream& operator<<(std::ostream& os, const UnivStr& str) {
    return os << to_utf8(str);
}

inline std::istream& operator>>(std::istream& is, UnivStr& str) {
    std::string utf8_str;
    is >> utf8_str;  // Read UTF-8 string
    str = UnivStr(utf8_str);  // Convert to UTF-32 using existing constructor
    return is;
}

// getline for reading full lines of UTF-8 text
inline std::istream& getline(std::istream& is, UnivStr& str, char32_t delim = U'\n') {
    std::string utf8_str;
    // Read UTF-8 line and convert delimiter
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    char utf8_delim = convert.to_bytes(std::u32string(1, delim))[0];
    
    if (std::getline(is, utf8_str, utf8_delim)) {
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
