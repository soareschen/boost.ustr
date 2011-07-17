
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

/*
 * add_suffix() is an illustrative example of how unicode_string_adapter
 * can be used in library APIs to provide a generic way to process
 * Unicode code points stored in arbitrary string types.
 *
 * In this example add_suffix() will add the chinese characters "你好" to
 * the end of the input string.
 */
template <typename StringT, typename StringTraits, typename EncodingTraits>
unicode_string_adapter<StringT, StringTraits, EncodingTraits>
add_suffix(const unicode_string_adapter<StringT, StringTraits, EncodingTraits>& str) {
    typedef unicode_string_adapter<
        StringT, StringTraits, EncodingTraits>              other_type;
    typedef unicode_string_adapter<
        std::vector< utf16_codeunit_type > >                this_type;

    // "你好" encoded in UTF-16 (means hello)
    utf16_codeunit_type suffix[] = { 0x4F60, 0x597D };

    // static unicode strings used within a library still has
    // to be held in certain encoding and container.
    this_type unicode_suffix = this_type::from_codeunits(suffix, suffix + sizeof(suffix)/sizeof(utf16_codeunit_type));

    // Now library developers can choose their own favorite encoding
    // and container internally (in this case UTF-16 in std::vector), 
    // but library users can pass to the API different encoding and 
    // different container (in this case UTF-8 in std::string)
    // and it can still work flawlessly.
    unicode_string_adapter<StringT, StringTraits, EncodingTraits>
    new_str = str + unicode_suffix;
    
    return new_str;
}
