
#pragma once

#include "incl.h"

namespace boost {
namespace ustr {

template < codepoint_type replacement = 0xFFFD >
class replace_policy {
  public:
    static codepoint_type replace_invalid_codepoint() {
        return replacement;
    }

    static codepoint_type replace_invalid_codepoint(codepoint_type original) {
        return replacement;
    }
};

class encoding_error : public std::exception { };

class error_policy {
  public:
    static codepoint_type replace_invalid_codepoint() {
        throw encoding_error();
    }

    static codepoint_type replace_invalid_codepoint(codepoint_type original) {
        throw encoding_error();
    }
};

} // namespace ustr
} // namespace boost
