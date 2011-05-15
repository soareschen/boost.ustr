
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
typedef string_traits<std::string>                      StringTraits;
typedef utf_encoding_traits<
    8, StringTraits, replace_policy<'?'> >              EncodingTraits;

typedef unicode_string_adapter<
    StringT, StringTraits, EncodingTraits>              U8String;
typedef unicode_string_adapter_builder<
    StringT, StringTraits, EncodingTraits>              U8StringBuilder;

typedef typename 
    U8String::const_codepoint_iterator_type             const_codepoint_iterator;

class utf8_string_adapter_test : public ::testing::TestWithParam<u8_string_fixture> {};

TEST_P(utf8_string_adapter_test, encoding) {

    u8_string_fixture fixture = GetParam();

    std::vector<codepoint_type> decoded = fixture.decoded;

    /* std::vector<codepoint_type> decoded = {
        0x0048, 0x0065, 0x006C, 0x006C, 0x006F,
        0x0020, 0x0057, 0x006F, 0x0072, 0x006C,
        0x0064, 0x0021
    };*/

    std::string encoded = fixture.encoded;

    U8String ustr(encoded);

    auto it = decoded.begin();
    const_codepoint_iterator u_it = ustr.begin();

    while(u_it != ustr.end()) {
        ASSERT_NE(it, decoded.end());
        EXPECT_EQ(*u_it, codepoint_type(*it));
        ++it;
        ++u_it;
    }

    U8StringBuilder mustr;
    for(auto mit = decoded.begin(); mit != decoded.end(); ++mit) {
        mustr.append(*mit);
    } 

    U8String freezed = mustr.freeze();

    EXPECT_EQ(freezed, ustr);

}

INSTANTIATE_TEST_CASE_P(fixture_test, utf8_string_adapter_test, ::testing::ValuesIn(get_fixtures()));


} // namespace test
} // namespace ustr
} // namespace boost
