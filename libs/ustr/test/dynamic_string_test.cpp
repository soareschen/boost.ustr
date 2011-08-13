

#include "gtest.h"
#include <boost/ustr/dynamic_unicode_string.hpp>

using namespace boost::ustr;

TEST(dynamic_unicode_string_adapter, basic_test) {
    unicode_string_adapter<std::string> test_string = USTR("世界你好");
    dynamic_unicode_string str = dynamic_unicode_string::create_dynamic_string(test_string);


    EXPECT_EQ(str.begin(), str.begin());
    EXPECT_EQ(str.end(), str.end());

    EXPECT_EQ(*str.begin(), *test_string.begin());
    EXPECT_EQ(*++str.begin(), *++test_string.begin());
    EXPECT_EQ(*str.end(), *test_string.end());

    EXPECT_TRUE(std::equal(test_string.begin(), test_string.end(), str.begin()));
    EXPECT_TRUE(std::equal(str.begin(), str.end(), test_string.begin()));

    EXPECT_EQ(str.length(), test_string.length());

    unicode_string_adapter< std::basic_string<utf16_codeunit_type> > test_string2(test_string);
    dynamic_unicode_string str2 = dynamic_unicode_string::create_dynamic_string(test_string2);

    EXPECT_TRUE(std::equal(str2.begin(), str2.end(), str.begin()));
    EXPECT_TRUE(std::equal(str2.begin(), str2.end(), test_string.begin()));
    EXPECT_TRUE(std::equal(str2.begin(), str2.end(), test_string2.begin()));
    EXPECT_EQ(str2, str);
}
