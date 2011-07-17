
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <iostream>
#include <boost/ustr/detail/incl.hpp>

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
    typedef char                                    type;
};

template <>
class char_type_by_size<2> {
  public:
    typedef utf16_codeunit_type                     type;
};

template <>
class char_type_by_size<4> {
  public:
    typedef codepoint_type                          type;
};

template <typename CharT>
class get_raw_char_type {
  public:
    typedef typename
        char_type_by_size< sizeof(CharT) >::type    type;
};

} // namespace ustr 
} // namespace boost
