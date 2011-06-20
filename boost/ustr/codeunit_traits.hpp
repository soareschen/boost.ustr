
#include <iterator>
#include <iostream>
#include "incl.h"

namespace boost {
namespace ustr {


template <size_t codeunit_size, typename CharT>
class char_holder;  

template <typename CharT, typename CharIterator>
class codeunit_iterator;

template <size_t CharSize>
class char_type_by_size {};

template <>
class char_type_by_size<1> {
  public:
    typedef char                                type;
};

template <>
class char_type_by_size<2> {
  public:
    typedef char16_t                            type;
};

template <>
class char_type_by_size<4> {
  public:
    typedef char32_t                            type;
};

template <typename CharT>
class get_raw_char_type {
  public:
    typedef typename
        char_type_by_size< sizeof(CharT) >::type       type;
};

/* 
template <typename CharT, typename CharIterator>
class codeunit_iterator : 
    public std::iterator<std::forward_iterator_tag, 
        char_holder<sizeof(CharT), CharT> > 
{
  public:
    typedef codeunit_iterator<
        CharT, CharIterator>            this_type;

    typedef CharT                       char_type;
    typedef CharIterator                char_iterator_type;

    static const size_t codeunit_size = sizeof(char_type);

    typedef char_holder<
        codeunit_size, char_type>       char_holder_type;

    codeunit_iterator(char_iterator_type iter) : _iter(iter) { 
        std::cout << "codeunit iterator construction" << std::endl;
    }

    char_holder_type operator *() {
        return char_holder_type(*_iter);
    }

    this_type operator ++() {
        ++_iter;
        return *this;
    }

    bool operator ==(const this_type& it) {
        return _iter == it._iter;
    }

  private:
    char_iterator_type          _iter;
};

template <size_t codeunit_size, typename CharT>
class char_holder { };  

// 8-bit character holder
template <typename CharT>
class char_holder<1, CharT> {
  public:
    typedef char char_type;

    char_holder(CharT character) : _char(character) { }

    operator char () {
        return static_cast<char>(_char);
    }

    operator unsigned char () {
        return static_cast<unsigned char>(_char);
    }

    bool operator ==(const unsigned char& c) {
        return static_cast<unsigned char>(_char) == c;
    }

    bool operator ==(const char_holder<8, CharT>& c) {
        return _char == c._char;
    }

    // explicitly disallows any conversion to other size chars,
    // because otherwise it would introduce hard to detect bugs.
    operator wchar_t () = delete;
    operator char16_t () = delete;
    operator char32_t () = delete;

    // Need to delete operator int() because otherwise the 
    // conversion would still be ambiguous, and conversion
    // to int shouldn't be allowed any way.
    //
    // This causes any comparison to the char_holder must be
    // made with explicit cast to char.
    operator int () = delete;

    bool operator ==(wchar_t) = delete;
    bool operator ==(char16_t) = delete;
    bool operator ==(char32_t) = delete;
    bool operator ==(int) = delete;
  private:
    CharT _char;
};

// 16-bit char_holder
template <typename CharT>
class char_holder<16, CharT> {
  public:
    typedef char16_t char_type;

    char_holder(CharT c) : _char(c) { }

    operator char16_t () {
        return static_cast<char16_t>(_char);
    }

    // IF IN WINDOWS
    operator wchar_t () {
        return static_cast<wchar_t>(_char);
    }


    bool operator ==(const char16_t& c) {
        return static_cast<char16_t>(_char) == c;
    }

    bool operator ==(const wchar_t& c) {
        return static_cast<wchar_t>(_char) == c;
    }

    bool operator ==(const char_holder<16, CharT>& c) {
        return _char == c._char;
    }

    // IF IN UNIX
    // operator wchar_t () = delete;
    operator char () = delete;
    operator unsigned char () = delete;
    operator char32_t () = delete;
    operator int () = delete;

    // bool operator ==(wchar_t) = delete;
    bool operator ==(char) = delete;
    bool operator ==(unsigned char) = delete;
    bool operator ==(char32_t) = delete;
    bool operator ==(int) = delete;
  private:
    CharT       _char;
};

// 32-bit char_holder
template <typename CharT>
class char_holder<32, CharT> {
  public:
    typedef char32_t char_type;

    char_holder(CharT c) : _char(c) { }

    operator char32_t () {
        return static_cast<char32_t>(_char);
    }

    bool operator ==(const char32_t& c) {
        return static_cast<char32_t>(_char) == c;
    }

    bool operator ==(const char_holder<32, CharT>& c) {
        return _char == c._char;
    }

    operator char () = delete;
    operator unsigned char() = delete;
    operator wchar_t () = delete;
    operator char16_t () = delete;
    operator int () = delete;

    bool operator ==(char) = delete;
    bool operator ==(unsigned char) = delete;
    bool operator ==(wchar_t) = delete;
    bool operator ==(char16_t) = delete;
    bool operator ==(int) = delete;

  private:
    CharT _char;
};
*/


} // namespace ustr 
} // namespace boost
