#pragma once

#include <locale>
#include <string>
#include <codecvt>
#include <string_view>


class UnivStr : public std::u32string {
public:
    using std::u32string::u32string;
    // expose base assignment operators so assigning from std::u32string still works
    using std::u32string::operator=;
    // assign from a UTF-8 std::string
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
