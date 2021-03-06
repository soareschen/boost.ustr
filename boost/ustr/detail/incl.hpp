
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/config.hpp>
#include <boost/cstdint.hpp>

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus > 199711L
#define BOOST_USTR_CPP0X
#endif

#ifdef BOOST_NO_CHAR16_T
    typedef boost::uint16_t     utf16_codeunit_type;
#else
    typedef char16_t            utf16_codeunit_type;
#endif

#ifdef BOOST_NO_CHAR32_T
    typedef boost::uint32_t     codepoint_type;
#else
    typedef char32_t            codepoint_type;
#endif

#if (!defined(BOOST_NO_UNICODE_LITERALS) || defined(BOOST_USTR_CPP0X))
#   define USTR_CHAR_TYPE char
#   define USTR_RAW(str) u8##str
#else
#   ifdef BOOST_WINDOWS
#       define USTR_CHAR_TYPE wchar_t
#       define USTR_RAW(str) L##str
#   else
#       define USTR_CHAR_TYPE char
#       define USTR_RAW(str) str
#   endif
#endif

#define USTR_STRING_TYPE std::basic_string< USTR_CHAR_TYPE >
