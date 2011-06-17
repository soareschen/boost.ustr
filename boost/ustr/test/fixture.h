
#pragma once

#include <string>
#include <vector>
#include "gtest.h"

namespace boost {
namespace ustr {
namespace test {

class utf_string_fixture {
  public:
    utf_string_fixture(const std::vector<codepoint_type>& decoded_, 
                      const std::vector<char>& u8_encoded_,
                      const std::vector<char16_t>& u16_encoded_) :
        decoded(decoded_), u8_encoded(u8_encoded_), u16_encoded(u16_encoded_)
    { }

    const std::vector<codepoint_type> decoded;
    const std::vector<char> u8_encoded;
    const std::vector<char16_t> u16_encoded;
};

inline std::vector<utf_string_fixture>
get_utf_fixtures() {
    static const std::vector<codepoint_type> decoded1 = { 0x01, 'A', 'Z', '8', 0x7F };
    static const std::vector<codepoint_type> decoded2 = { 0x80, 0x248, 0x555, 0x7FF };
    static const std::vector<codepoint_type> decoded3 = { 0x800, 0x1234, 0x5678, 0xFFFF };
    static const std::vector<codepoint_type> decoded4 = { 0x10000, 0x24680, 0x99999, 0x108088, 0x10FFFF };
    static const std::vector<codepoint_type> decoded5 = { 0x0048, 0x0065, 0x006C, 0x006C, 0x006F,
                                                          0x0020, 0x0057, 0x006F, 0x0072, 0x006C,
                                                          0x0064, 0x0021 };   // Hello World!
    static const std::vector<codepoint_type> decoded6 = { 0x4E16, 0x754C, 0x4F60, 0x597D }; // 世界你好


    static const std::vector<char> u8_encoded1 = { '\x01', '\x41', '\x5A', '\x38', '\x7F' };
    static const std::vector<char> u8_encoded2 = { '\xC2', '\x80', '\xC9', '\x88', '\xD5', '\x95', '\xDF', '\xBF' };
    static const std::vector<char> u8_encoded3 = { '\xE0', '\xA0', '\x80', '\xE1', '\x88', '\xB4', '\xE5', '\x99', 
                                                         '\xB8', '\xEF', '\xBF', '\xBF' };
    static const std::vector<char> u8_encoded4 = { '\xF0', '\x90', '\x80', '\x80', '\xF0', '\xA4', '\x9A', '\x80', '\xF2',
                                                         '\x99', '\xA6', '\x99', '\xF4', '\x88', '\x82', '\x88', '\xF4', '\x8F', 
                                                         '\xBF', '\xBF' };
    static const std::vector<char> u8_encoded5 = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
    static const std::vector<char> u8_encoded6 = { '\xE4', '\xB8', '\x96', '\xE7', '\x95', '\x8C', '\xE4', '\xBD', '\xA0',
                                                         '\xE5', '\xA5', '\xBD' };


    static const std::vector<char16_t> u16_encoded1 = { 0x0001, 0x0041, 0x005A, 0x0038, 0x007F };
    static const std::vector<char16_t> u16_encoded2 = { 0x0080, 0x0248, 0x0555, 0x07FF };
    static const std::vector<char16_t> u16_encoded3 = { 0x0800, 0x1234, 0x5678, 0xFFFF };
    static const std::vector<char16_t> u16_encoded4 = { 0xD800, 0xDC00, 0xD851, 0xDE80, 0xDA26, 0xDD99, 
                                                        0xDBE0, 0xDC88, 0xDBFF, 0xDFFF };
    static const std::vector<char16_t> u16_encoded5 = { 0x0048, 0x0065, 0x006C, 0x006C, 0x006F,
                                                        0x0020, 0x0057, 0x006F, 0x0072, 0x006C,
                                                        0x0064, 0x0021 };   // Hello World!
    static const std::vector<char16_t> u16_encoded6 = { 0x4E16, 0x754C, 0x4F60, 0x597D };


    std::vector<utf_string_fixture> fixtures = {
        utf_string_fixture(decoded1, u8_encoded1, u16_encoded1),
        utf_string_fixture(decoded2, u8_encoded2, u16_encoded2),
        utf_string_fixture(decoded3, u8_encoded3, u16_encoded3),
        utf_string_fixture(decoded4, u8_encoded4, u16_encoded4),
        utf_string_fixture(decoded5, u8_encoded5, u16_encoded5),
        utf_string_fixture(decoded6, u8_encoded6, u16_encoded6),
    };

    return fixtures;
}

template <size_t codeunit_size>
class fixture_encoding;

template <>
class fixture_encoding<1> {
  public:
    typedef std::vector<char> encoded_type;
    
    static inline encoded_type get_encoded(const utf_string_fixture& fixture) {
        return fixture.u8_encoded;
    }
};

template <>
class fixture_encoding<2> {
  public:
    typedef std::vector<char16_t> encoded_type;
    
    static inline encoded_type get_encoded(const utf_string_fixture& fixture) {
        return fixture.u16_encoded;
    }
};


} // namespace test
} // namespace ustr
} // namespace boost
