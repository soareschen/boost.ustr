
#include <string>
#include <vector>
#include "unicode_string_adapter.h"
#include "string_traits.h"
#include "encoding/utf8_encoding_traits.h"
#include "test/fixture.h"
#include "gtest.h"

namespace boost {
namespace ustr {
namespace test {

using namespace boost::ustr;

typedef std::string                                     StringT;
typedef std::vector<char>                               StringT2;

typedef string_traits<StringT>                          StringTraits;
typedef utf_encoding_traits<
    1, StringTraits, replace_policy<'?'> >              EncodingTraits;

typedef unicode_string_adapter<
    StringT, StringTraits, EncodingTraits>              U8String;
typedef unicode_string_adapter_builder<
    StringT, StringTraits, EncodingTraits>              U8StringBuilder;

typedef unicode_string_adapter<
    StringT2, StringTraits, EncodingTraits>             U8String2;
typedef unicode_string_adapter_builder<
    StringT2, StringTraits, EncodingTraits>             U8StringBuilder2;


typedef typename 
    U8String::codepoint_iterator_type                   codepoint_iterator;

class utf8_string_adapter_test : public ::testing::TestWithParam<u8_string_fixture> {};

TEST_P(utf8_string_adapter_test, encoding) {

    u8_string_fixture fixture = GetParam();
    std::vector<codepoint_type> decoded = fixture.decoded;
    std::string encoded = fixture.encoded;

    U8String ustr(encoded.c_str());

    auto it = decoded.begin();
    codepoint_iterator u_it = ustr.begin();

    while(u_it != ustr.end()) {
        ASSERT_NE(it, decoded.end());
        EXPECT_EQ(*u_it, codepoint_type(*it));
        ++it;
        ++u_it;
    }

    U8StringBuilder mustr;

    std::copy(decoded.begin(), decoded.end(), mustr.begin());
    /* for(auto mit = decoded.begin(); mit != decoded.end(); ++mit) {
        mustr.append(*mit);
    } */

    U8String freezed = mustr.freeze();

    EXPECT_EQ(freezed, ustr);

}

TEST_P(utf8_string_adapter_test, conversion) {

    u8_string_fixture fixture = GetParam();
    std::vector<codepoint_type> decoded = fixture.decoded;
    std::string encoded = fixture.encoded;

    U8String    ustr1_1(encoded.c_str());
    U8String2   ustr2_1(encoded.c_str());

    U8String    ustr1_2(ustr2_1);
    U8String2   ustr2_2(ustr1_2);

    EXPECT_EQ(ustr1_1, ustr1_2);
    EXPECT_EQ(ustr2_1, ustr2_2);

    EXPECT_EQ(ustr1_1, ustr2_1);
    EXPECT_EQ(ustr2_1, ustr1_1);
    EXPECT_EQ(ustr1_1, ustr2_2);
    EXPECT_EQ(ustr2_1, ustr1_2);
}

INSTANTIATE_TEST_CASE_P(fixture_test, utf8_string_adapter_test, ::testing::ValuesIn(get_fixtures()));


} // namespace test
} // namespace ustr
} // namespace boost
