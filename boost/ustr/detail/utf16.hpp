
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
namespace utf16 {

using namespace boost::ustr;


/*
 * 0000 0000 xxxx xxxx xxxx xxxx  ->  xxxx xxxx xxxx xxxx
 * 000u uuuu xxxx xxxx xxxx xxxx  ->  1101 10ww wwxx xxxx 1101 11xx xxxx xxxx
 * where wwww = uuuu - 1
 */

const utf16_codeunit_type HI_SURROGATE_PREFIX = 0xD800; // 1101 1000 0000 0000
const utf16_codeunit_type LO_SURROGATE_PREFIX = 0xDC00; // 1101 1100 0000 0000

const codepoint_type BIT_ONE_TO_TEN         = 0x3FF;    // 0000 0011 1111 1111
const codepoint_type BIT_ELEVEN_TO_FIFTEEN  = 0xFC00;   // 1111 1100 0000 0000

const codepoint_type UTF16_RESERVED_START   = 0xD800;
const codepoint_type UTF16_RESERVED_END     = 0xDFFF;

inline bool is_valid_codepoint(const codepoint_type& codepoint) {
    return codepoint <= 0x10FFFF &&
        !(UTF16_RESERVED_START <= codepoint && codepoint <= UTF16_RESERVED_END);
}

inline bool is_single_codeunit(const utf16_codeunit_type& codeunit) {
    return codeunit <= 0xD7FF || (0xE000 <= codeunit && codeunit <= 0xFFFF);
}

inline bool has_single_codeunit(const codepoint_type& codepoint) {
    return codepoint <= 0xD7FF || (0xE000 <= codepoint && codepoint <= 0xFFFF);
}

inline bool has_double_codeunit(const codepoint_type& codepoint) {
    return 0x10000 <= codepoint && codepoint <= 0x10FFFF;
}

inline utf16_codeunit_type get_high_surrogate(const codepoint_type& codepoint) {
    codepoint_type normalized = codepoint - 0x10000;
    return static_cast<utf16_codeunit_type>(((normalized >> 10) & BIT_ONE_TO_TEN) | HI_SURROGATE_PREFIX);
}

inline utf16_codeunit_type get_low_surrogate(const codepoint_type& codepoint) {
    return static_cast<utf16_codeunit_type>((codepoint & BIT_ONE_TO_TEN) | LO_SURROGATE_PREFIX);
}

inline bool is_high_surrogate(const utf16_codeunit_type& codeunit) {
    return (codeunit & BIT_ELEVEN_TO_FIFTEEN) == HI_SURROGATE_PREFIX;
}

inline bool is_low_surrogate(const utf16_codeunit_type& codeunit) {
    return (codeunit & BIT_ELEVEN_TO_FIFTEEN) == LO_SURROGATE_PREFIX;
}

inline codepoint_type decode_two_codeunits(
        const utf16_codeunit_type& hi, const utf16_codeunit_type& lo) 
{
    return  (((hi & BIT_ONE_TO_TEN) << 10) + 0x10000) |
            (lo & BIT_ONE_TO_TEN);
}

template <typename Policy = error_policy>
class utf16_encoder {
  public:
    template <typename OutputIterator>
    static inline void encode(const codepoint_type& codepoint, OutputIterator out) {
        if(has_single_codeunit(codepoint)) {
            *out++ = static_cast<const utf16_codeunit_type>(codepoint & 0xFFFF);
        } else if(has_double_codeunit(codepoint)) {
            utf16_codeunit_type hi = get_high_surrogate(codepoint);
            utf16_codeunit_type lo = get_low_surrogate(codepoint);

            *out++ = hi;
            *out++ = lo;
        } else {
            codepoint_type replacement_codepoint = Policy::replace_invalid_codepoint(codepoint);
            assert(replacement_codepoint != codepoint);
            encode(replacement_codepoint, out);
        }
    }

    template <typename CodeunitInputIterator>
    static inline codepoint_type decode(CodeunitInputIterator& begin, const CodeunitInputIterator& end) {
        utf16_codeunit_type hi = *begin++;
        if(is_high_surrogate(hi)) {
            if(begin == end) {
                return Policy::replace_invalid_codepoint();
            }

            utf16_codeunit_type lo = *begin++;
            if(!is_low_surrogate(lo)) {
                return Policy::replace_invalid_codepoint();
            } else {
                return check_and_return(decode_two_codeunits(hi, lo));
            }
        } else if(is_low_surrogate(hi)) {
            return Policy::replace_invalid_codepoint();
        } else {
            return static_cast<codepoint_type>(hi);
        }
    }

    template <typename CodeunitIterator>
    static inline codepoint_type decode_previous(const CodeunitIterator& begin, CodeunitIterator& end) {
        if(end == begin) {
            return Policy::replace_invalid_codepoint();
        }

        utf16_codeunit_type last = *--end;

        if(is_single_codeunit(last)) {
            return static_cast<codepoint_type>(last);
        } else if(is_low_surrogate(last)) {
            if(end == begin) {
                return Policy::replace_invalid_codepoint();
            }

            utf16_codeunit_type hi = *--end;
            return check_and_return(decode_two_codeunits(hi, last));
        } else {
            return Policy::replace_invalid_codepoint();
        }
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

} // namespace utf16
} // namespace encoding
} // namespace ustr
} // namesp
