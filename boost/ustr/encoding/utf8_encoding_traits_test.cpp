
#include <iterator>
#include <vector>
#include "gtest.h"
#include "utf8_encoding_traits.h"
#include "../string_traits.h"
#include "../test/fixture.h"

namespace boost {
namespace ustr {
namespace test {

using namespace boost::ustr;
using namespace boost::ustr::generic;
using std::string;

class u8_fixture {
  public:
    u8_fixture(const codepoint_type& decoded_, const string& encoded_) :
        decoded(decoded_), encoded(encoded_)
    { }

    const codepoint_type decoded;
    const string encoded;
};

class utf8_encoding_test : public ::testing::TestWithParam<u8_fixture> { };

TEST_P(utf8_encoding_test, single_codepoint) {
    u8_fixture param = GetParam();
  { // encoding
    codepoint_type decoded = param.decoded;
    string expected = param.encoded;

    string output;
    utf8::encode(decoded, std::back_inserter(output));
    EXPECT_EQ(output, expected);
  }

  { // decoding
    string encoded = param.encoded;
    codepoint_type expected = param.decoded;

    typename string::iterator begin = encoded.begin();
    typename string::iterator end = encoded.end();

    codepoint_type output = utf8::decode(begin, end);
    EXPECT_EQ(begin, end);
    EXPECT_EQ(output, expected);
  }
}

INSTANTIATE_TEST_CASE_P(ascii_chars, utf8_encoding_test,
    ::testing::Values(
        u8_fixture(0x01, "\x01"),
        u8_fixture('A', "A"),
        u8_fixture('Z', "Z"),
        u8_fixture('8', "8"),
        u8_fixture(0x7F, "\x7F")
    )
); 

INSTANTIATE_TEST_CASE_P(double_byte, utf8_encoding_test,
    ::testing::Values(
        u8_fixture(0x80, "\xC2\x80"),
        u8_fixture(0x248, "\xC9\x88"),
        u8_fixture(0x555, "\xD5\x95"),
        u8_fixture(0x7ff, "\xDF\xBF")
    )
); 

INSTANTIATE_TEST_CASE_P(triple_byte, utf8_encoding_test,
    ::testing::Values(
        u8_fixture(0x800, "\xE0\xA0\x80"),
        u8_fixture(0x1234, "\xE1\x88\xB4"),
        u8_fixture(0x5678, "\xE5\x99\xB8"),
        u8_fixture(0xFFFF, "\xEF\xBF\xBF")
    )
); 

INSTANTIATE_TEST_CASE_P(quad_byte, utf8_encoding_test,
    ::testing::Values(
        u8_fixture(0x10000, "\xF0\x90\x80\x80"),
        u8_fixture(0x24680, "\xF0\xA4\x9A\x80"),
        u8_fixture(0x99999, "\xF2\x99\xA6\x99"),
        u8_fixture(0x108088, "\xF4\x88\x82\x88"),
        u8_fixture(0x10FFFF, "\xF4\x8F\xBF\xBF")
    )
);




typedef std::string                                     StringT;
typedef string_traits<StringT>                          StringTraits;
typedef utf_encoding_traits< 
    8, StringTraits, replace_policy<'?'> >              EncodingTraits;
typedef typename
    StringTraits::mutable_strptr_type                   mutable_strptr_type;
typedef typename
    EncodingTraits::const_codepoint_iterator_type       const_codepoint_iterator;
typedef typename
    EncodingTraits::mutable_codepoint_iterator_type     mutable_codepoint_iterator;



class utf8_string_encoding_test : public ::testing::TestWithParam<u8_string_fixture> { };

TEST_P(utf8_string_encoding_test, multi_codepoint) {
    u8_string_fixture param = GetParam();

  { // encoding
    std::vector<codepoint_type> codepoints = param.decoded;

    StringT expected = param.encoded;

    mutable_strptr_type buffer(StringTraits::new_string());

    {
        mutable_codepoint_iterator buffer_ptr(&buffer);

        for(auto it = codepoints.begin(); it != codepoints.end(); ++it) {
            *buffer_ptr++ = *it;
        }
    }

    StringT output = *buffer;

    EXPECT_EQ(output, expected);
  }

  { // decoding
    std::vector<codepoint_type> expected = param.decoded;
    StringT encoded = param.encoded;
    typename std::vector<codepoint_type>::iterator expected_it = expected.begin();
    const_codepoint_iterator it(encoded.begin(), encoded.end());
    while(it != const_codepoint_iterator(encoded.end()))     
    {
        ASSERT_NE(expected_it, expected.end());
        EXPECT_EQ(*it, *expected_it);
        EXPECT_EQ(*it, *expected_it); // test repeated dereference
        ++it;
        ++expected_it;
    }

  }

}


INSTANTIATE_TEST_CASE_P(quad_byte, utf8_string_encoding_test, ::testing::ValuesIn(get_fixtures()));


TEST(utf8_validity_test, invalid_chars) {
    codepoint_type invalid = 0x110000;
    string output;
    EXPECT_THROW(utf8::encode<error_policy>(invalid, std::back_inserter(output)), encoding_error);
}



} // namespace test
} // namespace ustr
} // namespace boost
