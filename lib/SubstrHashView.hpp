#include <ranges>
#include <string>
#include <cctype>
#include <iostream>
#include <string_view>
#include <concepts>
#include "../TFdef.h"

// An iterator that generates rolling hash values for substrings   
class SubstringHashIterator {
private:
	std::string_view str_;
	size_t current_pos_;
	unsigned int round;
	static constexpr ui64 M1 = 1000000007;
	static constexpr ui64 M2 = 1000000009;
	static constexpr ui64 B1 = 131;
	static constexpr ui64 B2 = 1331;
	static constexpr uInt Shift = uInt(1) << 64;
	ui64 H1, H2;

public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = size_t;
	using difference_type = std::ptrdiff_t;
	using pointer = void;
	using reference = size_t;
	
	SubstringHashIterator(std::string_view str, size_t pos = 0, unsigned _round = 0)
	: str_(str), current_pos_(pos), round(_round), H1(str_[0]), H2(str_[0]) {}
	
	Int operator*() const {
		return H1 * Shift + H2;
	}
	
	SubstringHashIterator& operator++() {
		++current_pos_;
		if(current_pos_ == str_.length())
			current_pos_ = 0, ++round;
		H1 = (H1 * B1 + str_[current_pos_]) % M1;
		H2 = (H2 * B2 + str_[current_pos_]) % M2;
		return *this;
	}
	
	SubstringHashIterator operator++(int) {
		SubstringHashIterator tmp = *this;
		++current_pos_;
		if(current_pos_ == str_.length())
			current_pos_ = 0, ++round;
		H1 = (H1 * B1 + str_[current_pos_]) % M1;
		H2 = (H2 * B2 + str_[current_pos_]) % M2;
		return tmp;
	}
	
	bool operator==(const SubstringHashIterator& other) const {
		return current_pos_ == other.current_pos_ && round == other.round;
	}
};

// A view that generates rolling hash values for all substrings of a given string
class SubstringHashView {
private:
    std::string_view str_;
    
public:
    // C++23: Using auto parameters for better generic code
    SubstringHashView() = default;
    explicit SubstringHashView(std::string_view str) noexcept
        : str_(str) {}
    
    // Range interface
    [[nodiscard]] auto begin() const noexcept {
        return SubstringHashIterator(str_, 0, 0);
    }
    
    [[nodiscard]] auto end() const noexcept {
        return SubstringHashIterator(str_, 0, str_.length());
    }
    
    // C++23: Additional range operations
    [[nodiscard]] bool empty() const noexcept { 
        return str_.empty(); 
    }
    
    [[nodiscard]] auto size() const noexcept { 
        return str_.length(); 
    }
};