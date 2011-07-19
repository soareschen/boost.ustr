
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <iterator>
#include <iostream>
#include <boost/ustr/detail/incl.hpp>
#include <boost/ustr/detail/utf8.hpp>
#include <boost/ustr/detail/utf16.hpp>
#include <boost/ustr/detail/utf32.hpp>

namespace boost {
namespace ustr {
namespace util {

using boost::ustr::encoding::utf8::utf8_encoder;
using boost::ustr::encoding::utf16::utf16_encoder;
using boost::ustr::encoding::utf32::utf32_encoder;

template <size_t codeunit_size, typename Policy>
class encoding_engine { 

};

template <typename Policy>
class encoding_engine<1, Policy> {
  public:
    typedef utf8_encoder<Policy>    type;
};

template <typename Policy>
class encoding_engine<2, Policy> {
  public:
    typedef utf16_encoder<Policy>   type;
};

template <typename Policy>
class encoding_engine<4, Policy> {
  public:
    typedef utf32_encoder<Policy>   type;
};


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

} // namspace util
} // namespace ustr 
} // namespace boost
