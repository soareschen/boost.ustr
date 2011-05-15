
#pragma once

#include <string>
#include <vector>
#include "gtest.h"

namespace boost {
namespace ustr {
namespace test {

class u8_string_fixture {
  public:
    u8_string_fixture(std::vector<codepoint_type> decoded_, std::string encoded_) :
        decoded(decoded_), encoded(encoded_)
    { }

    const std::vector<codepoint_type> decoded;
    const std::string encoded;
};

inline std::vector<u8_string_fixture>
get_fixtures() {
    std::vector<codepoint_type> string1 = { 0x01, 'A', 'Z', '8', 0x7F };
    std::vector<codepoint_type> string2 = { 0x80, 0x248, 0x555, 0x7FF };
    std::vector<codepoint_type> string3 = { 0x800, 0x1234, 0x5678, 0xFFFF };
    std::vector<codepoint_type> string4 = { 0x10000, 0x24680, 0x99999, 0x108088, 0x10FFFF };
    std::vector<codepoint_type> string5 = { 0x0048, 0x0065, 0x006C, 0x006C, 0x006F,
                                            0x0020, 0x0057, 0x006F, 0x0072, 0x006C,
                                            0x0064, 0x0021 };   // Hello World!
    std::vector<codepoint_type> string6 = { 0x04E16, 0x754C, 0x4F60, 0x597D }; // 世界你好

    std::vector<u8_string_fixture> fixtures = {
        u8_string_fixture(string1,
            "\x01\x41\x5A\x38\x7F"
        ),
        u8_string_fixture(string2,
            "\xC2\x80\xC9\x88\xD5\x95\xDF\xBF"
        ),
        u8_string_fixture(string3,
            "\xE0\xA0\x80\xE1\x88\xB4\xE5\x99\xB8\xEF\xBF\xBF"
        ),
        u8_string_fixture(string4,
            "\xF0\x90\x80\x80\xF0\xA4\x9A\x80\xF2\x99\xA6\x99\xF4\x88\x82\x88\xF4\x8F\xBF\xBF"
        ),
        u8_string_fixture(string5, "Hello World!"),
        u8_string_fixture(string6, 
            "\xE4\xB8\x96\xE7\x95\x8C\xE4\xBD\xA0\xE5\xA5\xBD"
        )
    };

    return fixtures;

}

} // namespace test
} // namespace ustr
} // namespace boost
