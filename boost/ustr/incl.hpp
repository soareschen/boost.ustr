
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
