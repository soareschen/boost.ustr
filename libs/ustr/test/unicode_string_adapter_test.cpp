
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define _SCL_SECURE_NO_WARNINGS // todo: should be defined on MSVC only

#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <boost/ustr/unicode_string_adapter.hpp>
#include <boost/ustr/detail/alt_string_traits.hpp>
#include <libs/ustr/test/fixture.hpp>
#include "gtest.h"

namespace boost {
namespace ustr {
namespace test {

using namespace boost::ustr;

template <typename T>
class string_adapter_single_test : public ::testing::Test {};

template <typename T>
class string_adapter_double_test : public ::testing::Test {};

TYPED_TEST_CASE_P(string_adapter_single_test);
TYPED_TEST_CASE_P(string_adapter_double_test);

TYPED_TEST_P(string_adapter_single_test, encoding) {

    typedef TypeParam                                   UString;
    typedef typename
        UString::mutable_adapter_type                   UStringBuilder;

    const size_t codeunit_size = UString::codeunit_size;

    typedef fixture_encoding<codeunit_size>             fixture_encoder;
    typedef typename
        fixture_encoder::encoded_type                   encoded_type;

    typedef std::vector<utf_string_fixture>             fixture_t;
    fixture_t fixtures = get_utf_fixtures();

    for(fixture_t::iterator fixture = fixtures.begin(); fixture != fixtures.end(); ++fixture) {
        utf_string_fixture param = *fixture;
        encoded_type encoded = fixture_encoder::get_encoded(param);

        UString ustr1 = UString::from_codeunits(encoded.begin(), encoded.end());
        EXPECT_TRUE(std::equal(param.decoded.begin(), param.decoded.end(), ustr1.begin()));

        UString ustr2 = UString::from_codepoints(param.decoded.begin(), param.decoded.end());
        EXPECT_TRUE(std::equal(param.decoded.begin(), param.decoded.end(), ustr2.begin()));

        EXPECT_TRUE(std::equal(ustr1.rbegin(), ustr1.rend(), ustr2.rbegin()));
        EXPECT_TRUE(std::equal(ustr1.rbegin(), ustr1.rend(), param.decoded.rbegin()));

        EXPECT_EQ(ustr2, ustr1);
    }
}

template <typename CodepointIterator>
class same_codepoint {
  public:
    same_codepoint(CodepointIterator it) : _it(it) { }

    void operator ()(const codepoint_type& codepoint) {
        EXPECT_EQ(codepoint, *_it++);
    }

  private:
    CodepointIterator _it;
};

TYPED_TEST_P(string_adapter_single_test, stl_algorithms) {
    typedef TypeParam                                   UString;
    typedef typename
        UString::mutable_adapter_type                   UStringBuilder;

    const size_t codeunit_size = UString::codeunit_size;

    typedef fixture_encoding<codeunit_size>             fixture_encoder;
    typedef typename
        fixture_encoder::encoded_type                   encoded_type;

    typedef std::vector<utf_string_fixture>             fixture_t;
    fixture_t fixtures = get_utf_fixtures();

    for(fixture_t::iterator fixture = fixtures.begin(); fixture != fixtures.end(); ++fixture) {
        utf_string_fixture param = *fixture;
        encoded_type encoded = fixture_encoder::get_encoded(param);

        UString ustr1 = UString::from_codepoints(param.decoded.begin(), param.decoded.end());

        /*
         * std::find test
         */
        codepoint_type target = *param.decoded.begin(); // select first codepoint as target

        EXPECT_EQ(std::find(ustr1.begin(), ustr1.end(), target), ustr1.begin());

        /*
         * std::for_each test
         */
        std::for_each(ustr1.begin(), ustr1.end(), 
                same_codepoint<typename UString::iterator>(ustr1.begin()));
    }
}


TYPED_TEST_P(string_adapter_double_test, conversion) {
    typedef typename
        TypeParam::UString1                            UString;
    typedef typename
        TypeParam::UString2                            UString2;
    typedef typename
        UString::mutable_adapter_type                  UStringBuilder;
    typedef typename
        UString2::mutable_adapter_type                 UStringBuilder2;

    typedef typename
        UString::string_type                            StringT;
    typedef typename
        UString2::string_type                           StringT2;

    typedef fixture_encoding<UString::codeunit_size>    fixture_encoder;
    typedef typename
        fixture_encoder::encoded_type                   encoded_type;

    typedef fixture_encoding<UString2::codeunit_size>   fixture_encoder2;
    typedef typename
        fixture_encoder2::encoded_type                  encoded_type2;

    typedef std::vector<utf_string_fixture> fixture_t;
    fixture_t fixtures = get_utf_fixtures();

    for(fixture_t::iterator fixture = fixtures.begin(); fixture != fixtures.end(); ++fixture) {
        utf_string_fixture param = *fixture;
        encoded_type encoded = fixture_encoder::get_encoded(param);
        encoded_type2 encoded2 = fixture_encoder2::get_encoded(param);

        UString    ustr1_1 = UString::from_codeunits(encoded.begin(), encoded.end());
        UString2   ustr2_1 = UString2::from_codeunits(encoded2.begin(), encoded2.end());

        typename UString2::iterator uit = ustr2_1.begin();
        for(utf_string_fixture::decoded_t::const_iterator it = param.decoded.begin(); it != param.decoded.end(); ++it) {
            EXPECT_EQ(*uit, *it);
            ++uit;
        }

        EXPECT_TRUE(std::equal(param.decoded.begin(), param.decoded.end(), ustr1_1.begin()));
        EXPECT_TRUE(std::equal(param.decoded.begin(), param.decoded.end(), ustr2_1.begin()));

        UString    ustr1_2(ustr2_1);
        UString2   ustr2_2(ustr1_1);

        EXPECT_EQ(ustr1_1, ustr1_2);
        EXPECT_EQ(ustr2_1, ustr2_2);

        EXPECT_EQ(ustr1_1, ustr2_1);
        EXPECT_EQ(ustr2_1, ustr1_1);
        EXPECT_EQ(ustr1_1, ustr2_2);
        EXPECT_EQ(ustr2_1, ustr1_2);
    }
}


TYPED_TEST_P(string_adapter_double_test, concatenation) {
    typedef typename
        TypeParam::UString1                             UString1;
    typedef typename
        TypeParam::UString2                             UString2;
    typedef typename
        UString1::mutable_adapter_type                  UStringBuilder1;
    typedef typename
        UString2::mutable_adapter_type                  UStringBuilder2;

    typedef std::vector<utf_string_fixture> fixture_t;
    fixture_t fixtures = get_utf_fixtures();

    for(fixture_t::iterator fixture = fixtures.begin(); fixture != fixtures.end(); ++fixture) {
        utf_string_fixture param = *fixture;
        utf_string_fixture::decoded_t::const_iterator codepoint_it = param.decoded.begin();

        UStringBuilder1 mustr1;
        UStringBuilder2 mustr2;

        // append the first 2 code points to mustr1
        for(int i=0; i<2; ++i) {
            mustr1.append(*codepoint_it);
            ++codepoint_it;
        }

        // append the rest of code points to mustr2
        while(codepoint_it != param.decoded.end()) {
            mustr2.append(*codepoint_it);
            ++codepoint_it;
        }

        UString1 ustr1 = mustr1.freeze();
        UString2 ustr2 = mustr2.freeze();

        // Now append the two strings together
        UString1 ustr3 = ustr1 + ustr2;

        EXPECT_TRUE(std::equal(ustr3.begin(), ustr3.end(), param.decoded.begin()));
    }
}

class ustr_test_type_param1 {
  public:
    typedef unicode_string_adapter< std::string >           UString1;
    typedef unicode_string_adapter< std::vector<char> >     UString2;
};

class ustr_test_type_param2 {
  public:
    typedef unicode_string_adapter< std::string >           UString1;
    typedef unicode_string_adapter< 
        std::vector<utf16_codeunit_type> >                  UString2;
};

class ustr_test_type_param3 {
  public:
    typedef unicode_string_adapter< 
        std::basic_string<utf16_codeunit_type> >            UString1;
    typedef unicode_string_adapter< std::vector<char> >     UString2;
};

class ustr_test_type_param4 {
  public:
    typedef unicode_string_adapter< std::list<char> >       UString1;
    typedef unicode_string_adapter< 
        std::list<utf16_codeunit_type> >                    UString2;
};

REGISTER_TYPED_TEST_CASE_P(string_adapter_single_test, encoding, stl_algorithms);
REGISTER_TYPED_TEST_CASE_P(string_adapter_double_test, conversion, concatenation);

typedef ::testing::Types<
        unicode_string_adapter< std::string >,
        unicode_string_adapter< std::basic_string<utf16_codeunit_type> >,
        unicode_string_adapter< std::vector<char> >,
        unicode_string_adapter< std::vector<utf16_codeunit_type> >,
        unicode_string_adapter< std::list<char> >,
        unicode_string_adapter< std::list<utf16_codeunit_type> >,
        unicode_string_adapter< std::string, alt_string_traits<std::string> >
    > single_test_type_params;

INSTANTIATE_TYPED_TEST_CASE_P(basic, string_adapter_single_test, single_test_type_params);

typedef ::testing::Types<
        ustr_test_type_param1,
        ustr_test_type_param2,
        ustr_test_type_param3,
        ustr_test_type_param4
    > double_test_type_params;

INSTANTIATE_TYPED_TEST_CASE_P(basic, string_adapter_double_test, double_test_type_params);

TEST(string_adapter_validation_test, codepoint_replacement) {
    std::string *raw_string = new std::string("\x80\x80X");

    unicode_string_adapter< std::string > malformed_string = 
        unicode_string_adapter<std::string>::from_ptr(raw_string);

    typename unicode_string_adapter< std::string >::iterator
    mit = malformed_string.begin();

    EXPECT_EQ(*mit++, (codepoint_type)0xFFFD);
    EXPECT_EQ(*mit++, (codepoint_type)0xFFFD);
    EXPECT_EQ(*mit++, (codepoint_type)'X');

    std::string sanitized_string = *malformed_string;
    typename std::string::iterator sit = sanitized_string.begin();

    EXPECT_EQ(*sit++, '\xEF');
    EXPECT_EQ(*sit++, '\xBF');
    EXPECT_EQ(*sit++, '\xBD');

    EXPECT_EQ(*sit++, '\xEF');
    EXPECT_EQ(*sit++, '\xBF');
    EXPECT_EQ(*sit++, '\xBD');

    EXPECT_EQ(*sit++, 'X');
}



} // namespace test
} // namespace ustr
} // namespace boost
