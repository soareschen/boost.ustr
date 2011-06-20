
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <boost/ustr/unicode_string_adapter.hpp>
#include <boost/ustr/string_traits.hpp>
#include <boost/ustr/encoding_traits.hpp>
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

    std::vector<utf_string_fixture> fixtures = get_utf_fixtures();

    for(auto fixture = fixtures.begin(); fixture != fixtures.end(); ++fixture) {
        utf_string_fixture param = *fixture;
        encoded_type encoded = fixture_encoder::get_encoded(param);

        UString ustr1 = UString::from_codeunits(encoded.begin(), encoded.end());
        EXPECT_TRUE(std::equal(param.decoded.begin(), param.decoded.end(), ustr1.begin()));

        UString ustr2 = UString::from_codepoints(param.decoded.begin(), param.decoded.end());
        EXPECT_TRUE(std::equal(param.decoded.begin(), param.decoded.end(), ustr2.begin()));

        EXPECT_EQ(ustr2, ustr1);
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

    std::vector<utf_string_fixture> fixtures = get_utf_fixtures();

    for(auto fixture = fixtures.begin(); fixture != fixtures.end(); ++fixture) {
        utf_string_fixture param = *fixture;
        encoded_type encoded = fixture_encoder::get_encoded(param);
        encoded_type2 encoded2 = fixture_encoder2::get_encoded(param);

        UString    ustr1_1 = UString::from_codeunits(encoded.begin(), encoded.end());
        UString2   ustr2_1 = UString2::from_codeunits(encoded2.begin(), encoded2.end());

        auto uit = ustr2_1.begin();
        for(auto it = param.decoded.begin(); it != param.decoded.end(); ++it) {
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

class ustr_test_type_param1 {
  public:
    typedef unicode_string_adapter< std::string >           UString1;
    typedef unicode_string_adapter< std::vector<char> >     UString2;
};

class ustr_test_type_param2 {
  public:
    typedef unicode_string_adapter< std::string >           UString1;
    typedef unicode_string_adapter< std::vector<char16_t> > UString2;
};

class ustr_test_type_param3 {
  public:
    typedef unicode_string_adapter< std::u16string >        UString1;
    typedef unicode_string_adapter< std::vector<char> >     UString2;
};

class ustr_test_type_param4 {
  public:
    typedef unicode_string_adapter< std::list<char> >        UString1;
    typedef unicode_string_adapter< std::list<char16_t> >    UString2;
};

REGISTER_TYPED_TEST_CASE_P(string_adapter_single_test, encoding);
REGISTER_TYPED_TEST_CASE_P(string_adapter_double_test, conversion);

typedef ::testing::Types<
        unicode_string_adapter< std::string >,
        unicode_string_adapter< std::u16string >,
        unicode_string_adapter< std::vector<char> >,
        unicode_string_adapter< std::vector<char16_t> >,
        unicode_string_adapter< std::list<char> >,
        unicode_string_adapter< std::list<char16_t> >
    > single_test_type_params;

INSTANTIATE_TYPED_TEST_CASE_P(basic, string_adapter_single_test, single_test_type_params);

typedef ::testing::Types<
        ustr_test_type_param1,
        ustr_test_type_param2,
        ustr_test_type_param3,
        ustr_test_type_param4
    > double_test_type_params;

INSTANTIATE_TYPED_TEST_CASE_P(basic, string_adapter_double_test, double_test_type_params);




} // namespace test
} // namespace ustr
} // namespace boost
