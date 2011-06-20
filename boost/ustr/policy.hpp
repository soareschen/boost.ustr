
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/ustr/incl.hpp>

namespace boost {
namespace ustr {

template < codepoint_type replacement = '?' >
class replace_policy {
  public:
    codepoint_type replace_invalid_codepoint() {
        return replacement;
    }

    codepoint_type replace_invalid_codepoint(codepoint_type original) {
        return replacement;
    }
};

class encoding_error : public std::exception { };

class error_policy {
  public:
    codepoint_type replace_invalid_codepoint() {
        throw encoding_error();
    }

    codepoint_type replace_invalid_codepoint(codepoint_type original) {
        throw encoding_error();
    }
};

} // namespace ustr
} // namespace boost
