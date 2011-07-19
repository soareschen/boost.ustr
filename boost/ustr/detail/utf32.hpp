
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/ustr/detail/incl.hpp>
#include <boost/ustr/policy.hpp>

namespace boost {
namespace ustr {
namespace encoding {
namespace utf32 {

using namespace boost::ustr;

inline bool is_valid_codepoint(const codepoint_type& codepoint) {
    return codepoint <= 0x10FFFF;
}

template <typename Policy = error_policy>
class utf32_encoder {
  public:
    template <typename OutputIterator>
    static inline void encode(const codepoint_type& codepoint, OutputIterator out) {
        *out++ = check_and_return(codepoint);
    }

    template <typename CodeunitInputIterator>
    static inline codepoint_type decode(CodeunitInputIterator& begin, const CodeunitInputIterator& end) {
        return check_and_return(*begin++);
    }

    template <typename CodeunitIterator>
    static inline codepoint_type decode_previous(const CodeunitIterator& begin, CodeunitIterator& end) {
        if(end == begin) {
            return Policy::replace_invalid_codepoint();
        }

        return check_and_return(*--end);
    }

  private:
    static inline codepoint_type check_and_return(const codepoint_type& codepoint) {
        if(is_valid_codepoint(codepoint)) {
            return codepoint;
        } else {
            return Policy::replace_invalid_codepoint(codepoint);
        }
    }
};

} // namespace utf32
} // namespace encoding
} // namespace ustr
} // namespace boost


