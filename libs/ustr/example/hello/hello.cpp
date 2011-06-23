
#include <string>
#include <vector>
#include <iostream>
#include <boost/ustr/unicode_string_adapter.hpp>

using namespace boost::ustr;

template <typename StringT, typename StringTraits, typename EncodingTraits>
unicode_string_adapter<StringT, StringTraits, EncodingTraits>
add_suffix(const unicode_string_adapter<StringT, StringTraits, EncodingTraits>& str);

int main() {
    // "世界" encoded in UTF-8 (means world)
    std::string my_string = "\xE4\xB8\x96\xE7\x95\x8C"; 

    // Unicode-aware string in UTF-8 encoding
    unicode_string_adapter<std::string> my_unicode_string(my_string);

    // Pass my_unicode_string to a very complex Unicode string
    // processing function (in this case add_suffix for illustration)
    // and get back a new Unicode string.
    unicode_string_adapter<std::string> 
    my_new_unicode_string = add_suffix(my_unicode_string);

    // The raw std::string can be obtained easily through operator *().
    std::string my_new_raw_string = *my_new_unicode_string;

    // If the terminal encoding is configured correctly, this should
    // display the chinese characters "世界你好".
    std::cout << my_new_raw_string << std::endl;
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
add_suffix(unicode_string_adapter<StringT, StringTraits, EncodingTraits> str) {
    // "你好" encoded in UTF-16 (means hello)
    static char16_t suffix[] = { 0x4F60, 0x597D };

    // static unicode strings used within a library still has
    // to be held in certain encoding and container.
    static unicode_string_adapter< std::vector<char16_t> > 
    unicode_suffix( std::vector<char16_t>(suffix) );

    // Now library developers can choose their own favorite encoding
    // and container internally (in this case UTF-16 in std::vector), 
    // but library users can pass to the API different encoding and 
    // different container (in this case UTF-8 in std::string)
    // and it can still work flawlessly.
    unicode_string_adapter<StringT, StringTraits, EncodingTraits>
    new_str = str + unicode_suffix;
    
    return new_str;
}
