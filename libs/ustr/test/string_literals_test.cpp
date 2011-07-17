

#include <algorithm>
#include <gtest.h>
#include <boost/ustr/unicode_string_adapter.hpp>

namespace boost {
namespace ustr {
namespace test {

using namespace boost::ustr;

TEST(string_literals_test, source_encoding_correctness) {
    const codepoint_type decoded_[] = { 
        0x4E16, 0x754C, 0x4F60, 0x597D
        //'h', 'e', 'l', 'l', 'o', ' ',
        //'w', 'o', 'r', 'l', 'd', '!' 
    };

    std::vector<codepoint_type> decoded(decoded_, 
            decoded_ + sizeof(decoded_) / sizeof(codepoint_type));

    //unicode_string_adapter<std::string> str = USTR("hello world!");
    unicode_string_adapter< std::vector<char> > str = USTR("世界你好");

    EXPECT_TRUE(std::equal(str.begin(), str.end(), decoded.begin()));
}

} // namespace test
} // namespace ustr
} // namespace boost
