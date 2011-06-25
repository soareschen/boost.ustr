
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define _SCL_SECURE_NO_WARNINGS // todo: Should be for MSVC only

#include <iterator>
#include <vector>
#include <algorithm>
#include "gtest.h"
#include <boost/ustr/encoding_traits.hpp>
#include <boost/ustr/string_traits.hpp>
#include <boost/ustr/policy.hpp>
#include <libs/ustr/test/fixture.hpp>

namespace boost {
namespace ustr {
namespace test {

using namespace boost::ustr;
using namespace boost::ustr::encoding;
using std::string;

class u8_fixture {
  public:
    u8_fixture(const codepoint_type& decoded_, const string& encoded_) :
        decoded(decoded_), encoded(encoded_)
    { }
    
    u8_fixture operator =(const u8_fixture& other) {
        return *this;
    }
    
    typedef codepoint_type decoded_t;
    typedef string encoded_t;

    const decoded_t decoded;
    const encoded_t encoded;
};

class utf8_encoding_test : public ::testing::TestWithParam<u8_fixture> { };

TEST_P(utf8_encoding_test, single_codepoint) {
    u8_fixture param = GetParam();

    string encoded;
    utf8_encoder<error_policy>::encode(param.decoded, std::back_inserter(encoded));
    EXPECT_EQ(encoded, param.encoded);

    u8_fixture::encoded_t::const_iterator begin = param.encoded.begin();

    codepoint_type decoded = utf8_encoder<error_policy>::decode(begin, param.encoded.end());
    EXPECT_EQ(begin, param.encoded.end());
    EXPECT_EQ(decoded, param.decoded);
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

template <typename T>
class encoding_traits_test : public ::testing::TestWithParam<utf_string_fixture> {

};

TYPED_TEST_CASE_P(encoding_traits_test);

TYPED_TEST_P(encoding_traits_test, fixture_test) {
    typedef TypeParam                                       StringT;
    typedef string_traits<StringT>                          StringTraits;
    typedef utf_encoding_traits< 
        StringTraits, replace_policy<'?'> >                 EncodingTraits;
    typedef typename
        StringTraits::raw_strptr_type                       raw_strptr_type;
    typedef typename
        StringTraits::mutable_strptr_type                   mutable_strptr_type;
    typedef typename
        EncodingTraits::codepoint_iterator_type             codepoint_iterator;

    const size_t codeunit_size = StringTraits::codeunit_size;

    typedef fixture_encoding<codeunit_size>                 fixture_encoder;
    typedef typename
        fixture_encoder::encoded_type                       encoded_type;

    typedef std::vector<utf_string_fixture> fixture_t;
    fixture_t fixtures = get_utf_fixtures();

    for(fixture_t::iterator fixture = fixtures.begin(); fixture != fixtures.end(); ++fixture) {
        utf_string_fixture param = *fixture;

        encoded_type encoded = fixture_encoder::get_encoded(param);
        mutable_strptr_type buffer(StringTraits::new_string());

        for(utf_string_fixture::decoded_t::const_iterator it = param.decoded.begin(); it != param.decoded.end(); ++it) {
            EncodingTraits::append_codepoint(buffer, *it);
        }

        EXPECT_TRUE(std::equal(buffer->begin(), buffer->end(), encoded.begin()));

        StringT encoded_string = StringTraits::string::from_iter(encoded.begin(), encoded.end());
        typename std::vector<codepoint_type>::const_iterator expected_it = param.decoded.begin();
        codepoint_iterator it2(encoded_string.begin(), encoded_string.begin(), encoded_string.end());
        // Was 'it' instead of 'it2', but MSVC8 gave warning C4288.

        codepoint_iterator end = codepoint_iterator(
                encoded_string.end(), encoded_string.begin(), encoded_string.end());
        while(it2 != end)
        {
            ASSERT_NE(expected_it, param.decoded.end());
            EXPECT_EQ(*it2, *expected_it);
            EXPECT_EQ(*it2, *expected_it); // test repeated dereference
            ++it2;
            ++expected_it;
        }

        EXPECT_TRUE(std::equal(param.decoded.begin(), param.decoded.end(), 
                    codepoint_iterator(encoded_string.begin(), encoded_string.begin(), encoded_string.end())));
    }
}

TYPED_TEST_P(encoding_traits_test, reverse_decode) {
    typedef TypeParam                                       StringT;
    typedef string_traits<StringT>                          StringTraits;
    typedef utf_encoding_traits< 
        StringTraits, error_policy >                        EncodingTraits;
    typedef typename
        StringTraits::raw_strptr_type                       raw_strptr_type;
    typedef typename
        StringTraits::mutable_strptr_type                   mutable_strptr_type;
    typedef typename
        EncodingTraits::codepoint_iterator_type             codepoint_iterator;

    const size_t codeunit_size = StringTraits::codeunit_size;

    typedef fixture_encoding<codeunit_size>                 fixture_encoder;
    typedef typename
        fixture_encoder::encoded_type                       encoded_type;

    typedef typename StringT::iterator                      codeunit_iterator;
    typedef typename 
        std::vector<codepoint_type>::const_reverse_iterator reverse_codepoint_iterator;


    typedef std::vector<utf_string_fixture> fixture_t;
    fixture_t fixtures = get_utf_fixtures();

    for(fixture_t::iterator fixture = fixtures.begin(); fixture != fixtures.end(); ++fixture) {
        utf_string_fixture param = *fixture;

        encoded_type encoded = fixture_encoder::get_encoded(param);
        StringT encoded_string = StringTraits::string::from_iter(encoded.begin(), encoded.end());

        codeunit_iterator begin = encoded_string.begin();
        codeunit_iterator end = encoded_string.end();
        
        reverse_codepoint_iterator rbegin = param.decoded.rbegin();
        reverse_codepoint_iterator rend = param.decoded.rend();
        
        while(end != begin) {
            codepoint_type decoded = EncodingTraits::encoder::decode_previous(begin, end);
            EXPECT_EQ(decoded, *rbegin++);
        }
    }
}

REGISTER_TYPED_TEST_CASE_P(encoding_traits_test, fixture_test, reverse_decode);

typedef ::testing::Types< std::string, std::vector<char>, 
        std::vector<utf16_codeunit_type>, std::basic_string<utf16_codeunit_type>
    > test_types;
INSTANTIATE_TYPED_TEST_CASE_P(basic, encoding_traits_test, test_types);


TEST(utf8_validity_test, invalid_chars) {
    codepoint_type invalid = 0x110000;
    string output;
    EXPECT_THROW(utf8_encoder<error_policy>::encode(invalid, std::back_inserter(output)), encoding_error);
}



} // namespace test
} // namespace ustr
} // namespace boost
