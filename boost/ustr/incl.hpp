
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

//#define BOOST_USTR_CPP0X

#ifndef BOOST_USTR_CPP0X
    #include <boost/cstdint.hpp>
    #include <boost/shared_ptr.hpp>
    #include <boost/scoped_ptr.hpp>

    typedef boost::uint16_t char16_t;
    typedef boost::uint32_t char32_t;
    
    namespace std {
        typedef basic_string<char16_t> u16string;
    }
#endif

typedef char32_t    codepoint_type;
