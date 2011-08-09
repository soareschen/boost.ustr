
#include <string>
#include <vector>
#include <iostream>
#include <boost/ustr/unicode_string_adapter.hpp>

using namespace boost::ustr;

template <typename StringT, typename StringTraits, typename EncodingTraits>
unicode_string_adapter<StringT, StringTraits, EncodingTraits>
add_suffix(const unicode_string_adapter<StringT, StringTraits, EncodingTraits>& str);

int main() {
    unicode_string_adapter<std::string> my_string = USTR("世界你好");

    std::cout << my_string << std::endl;
}
