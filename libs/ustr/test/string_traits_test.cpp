
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <vector>
#include <boost/ustr/string_traits.hpp>
#include "gtest.h"

using boost::ustr::string_traits;

template <typename T>
class string_traits_test : public ::testing::Test { };

TYPED_TEST_CASE_P(string_traits_test);

TYPED_TEST_P(string_traits_test, raw_pointer) {
    typedef TypeParam                       StringT;
    typedef string_traits<StringT>          StringTraits;
    typedef typename
        StringTraits::string_type           string_type;
    typedef typename
        StringTraits::codeunit_type         codeunit_type;
    typedef typename
        StringTraits::raw_strptr_type       raw_strptr_type;
    typedef typename
        StringTraits::const_strptr_type     const_strptr_type;
    typedef typename
        StringTraits::mutable_strptr_type   mutable_strptr_type;

    codeunit_type example_[] = { 't', 'e', 's', 't' };
    string_type example(example_, example_ + sizeof(example_)/sizeof(codeunit_type));

    raw_strptr_type str1 = StringTraits::new_string(example);
    string_type str2 = *str1;
    EXPECT_TRUE(StringTraits::raw_strptr::equals(str1, example));
    EXPECT_TRUE(StringTraits::string::equals(str2, example));

    StringTraits::raw_strptr::delete_string(str1);
}

TYPED_TEST_P(string_traits_test, const_pointer) {
    typedef TypeParam                       StringT;
    typedef string_traits<StringT>          StringTraits;
    typedef typename
        StringTraits::string_type           string_type;
    typedef typename
        StringTraits::codeunit_type         codeunit_type;
    typedef typename
        StringTraits::raw_strptr_type       raw_strptr_type;
    typedef typename
        StringTraits::const_strptr_type     const_strptr_type;
    typedef typename
        StringTraits::mutable_strptr_type   mutable_strptr_type;

    codeunit_type str1_[] = { 't', 'e', 's', 't' };
    string_type str1(str1_, str1_ + sizeof(str1_)/sizeof(codeunit_type));

    const_strptr_type str2(StringTraits::new_string(str1));
    const_strptr_type str3(str2);

    EXPECT_EQ(StringTraits::const_strptr::get(str2),
              StringTraits::const_strptr::get(str3));

    EXPECT_TRUE(StringTraits::const_strptr::equals(str2, str3));

    const_strptr_type str4(StringTraits::new_string(str1));

    EXPECT_NE(StringTraits::const_strptr::get(str2),
              StringTraits::const_strptr::get(str4));

    EXPECT_TRUE(StringTraits::const_strptr::equals(str2, str4));
}

TYPED_TEST_P(string_traits_test, null_raw_strptr) {
    typedef TypeParam                       StringT;
    typedef string_traits<StringT>          StringTraits;
    typedef typename
        StringTraits::string_type           string_type;
    typedef typename
        StringTraits::codeunit_type         codeunit_type;
    typedef typename
        StringTraits::raw_strptr_type       raw_strptr_type;
    typedef typename
        StringTraits::const_strptr_type     const_strptr_type;
    typedef typename
        StringTraits::mutable_strptr_type   mutable_strptr_type;

    string_type str1; // empty string

    raw_strptr_type str2 = StringTraits::new_string(str1);
    raw_strptr_type str3 = NULL;
    raw_strptr_type str4 = NULL;

    EXPECT_TRUE(StringTraits::raw_strptr::equals(str2, str3));
    EXPECT_TRUE(StringTraits::raw_strptr::equals(str3, str2));
    EXPECT_TRUE(StringTraits::raw_strptr::equals(str3, str4));

    StringTraits::raw_strptr::delete_string(str2);
}

TYPED_TEST_P(string_traits_test, null_const_strptr) {
    typedef TypeParam                       StringT;
    typedef string_traits<StringT>          StringTraits;
    typedef typename
        StringTraits::string_type           string_type;
    typedef typename
        StringTraits::codeunit_type         codeunit_type;
    typedef typename
        StringTraits::raw_strptr_type       raw_strptr_type;
    typedef typename
        StringTraits::const_strptr_type     const_strptr_type;
    typedef typename
        StringTraits::mutable_strptr_type   mutable_strptr_type;

    string_type str1; // empty string

    const_strptr_type str2(StringTraits::new_string(str1));
    const_strptr_type str3((string_type*) NULL);
    const_strptr_type str4((string_type*) NULL);

    EXPECT_TRUE(StringTraits::const_strptr::equals(str2, str3));
    EXPECT_TRUE(StringTraits::const_strptr::equals(str3, str2));
    EXPECT_TRUE(StringTraits::const_strptr::equals(str3, str4));

}

TYPED_TEST_P(string_traits_test, mutable_strptr) {
    typedef TypeParam                       StringT;
    typedef string_traits<StringT>          StringTraits;
    typedef typename
        StringTraits::string_type           string_type;
    typedef typename
        StringTraits::codeunit_type         codeunit_type;
    typedef typename
        StringTraits::raw_strptr_type       raw_strptr_type;
    typedef typename
        StringTraits::const_strptr_type     const_strptr_type;
    typedef typename
        StringTraits::mutable_strptr_type   mutable_strptr_type;

    codeunit_type str1_[] = { 't', 'e', 's', 't' };
    string_type str1(str1_, str1_ + sizeof(str1_)/sizeof(codeunit_type));

    mutable_strptr_type str2;

    EXPECT_EQ(str2.get(), (raw_strptr_type) NULL);

    StringTraits::mutable_strptr::append(str2, codeunit_type('t'));
    StringTraits::mutable_strptr::append(str2, codeunit_type('e'));

    EXPECT_NE( StringTraits::mutable_strptr::get(str2), 
               (raw_strptr_type) NULL);
    
    codeunit_type expected_[] = { 't', 'e' };
    string_type expected(expected_, expected_ + sizeof(expected_)/sizeof(codeunit_type));

    EXPECT_TRUE(StringTraits::raw_strptr::equals(
                    StringTraits::mutable_strptr::get(str2), 
                    expected));

    StringTraits::mutable_strptr::append(str2, codeunit_type('s'));
    StringTraits::mutable_strptr::append(str2, codeunit_type('t'));

    raw_strptr_type str3 = StringTraits::mutable_strptr::release(str2);
    EXPECT_TRUE(StringTraits::raw_strptr::equals(str3, str1));
    EXPECT_EQ(str2.get(), (raw_strptr_type) NULL);

    StringTraits::raw_strptr::delete_string(str3);
}


REGISTER_TYPED_TEST_CASE_P(string_traits_test, 
        raw_pointer, const_pointer, null_raw_strptr, 
        null_const_strptr, mutable_strptr);

typedef ::testing::Types< 
        std::string, std::vector<char>, std::list<char>,
        std::basic_string<utf16_codeunit_type>, std::vector<utf16_codeunit_type>, std::list<utf16_codeunit_type>
    > string_traits_test_types;

INSTANTIATE_TYPED_TEST_CASE_P(stl_containers, string_traits_test, string_traits_test_types);

