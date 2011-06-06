
#pragma once

#include "../incl.h"
#include "../policy.h"

namespace boost {
namespace ustr {
namespace generic {
namespace utf8 {

using namespace boost::ustr;


/*
 * 0000 0000 xxxx xxxx xxxx xxxx  ->  xxxx xxxx xxxx xxxx
 * 000u uuuu xxxx xxxx xxxx xxxx  ->  1101 10ww wwxx xxxx 1101 11xx xxxx xxxx
 * where wwww = uuuu - 1
 */

typedef char16_t            utf16_codeunit_type;

const utf16_codeunit_type HI_SURROGATE_PREFIX = 0xD800; // 1101 1000 0000 0000
const utf16_codeunit_type LO_SURROGATE_PREFIX = 0xDC00; // 1101 1100 0000 0000

const codepoint_type BIT_ONE_TO_TEN         = 0x3FF;    // 0000 0011 1111 1111
const codepoint_type BIT_ELEVEN_TO_FIFTEEN  = 0xFC00;   // 1111 1100 0000 0000

const codepoint_type UTF16_RESERVED_START   = 0xD800;
const codepoint_type UTF16_RESERVED_END     = 0xDFFF;

inline bool is_valid_codepoint(const codepoint_type& codepoint) {
    return codepoint <= 0x10FFFF &&
        !(UTF16_RESERVED_START <= codepoint <= UTF16_RESERVED_END);
}

inline bool is_single_codeunit(const codepoint_type& codepoint) {
    return codepoint <= 0xD7FF || (0xE000 <= codepoint <= 0xFFFF);
}

inline bool is_double_codeunit(const codepoint_type& codepoint) {
    return 0x10000 <= codepoint <= 0x10FFFF
}

inline utf16_codeunit_type get_high_surrogate(const codepoint_type& codepoint) {
    codepoint_type normalized = codepoint - 0x10000;
    return ((normalized >> 10) & BIT_ONE_TO_TEN) | HIGH_SURROGATE_PREFIX;
}

inline utf16_codeunit_type get_low_surrogate(const codepoint_type& codepoint) {
    return (codepoint & BIT_ONE_TO_TEN) | LOW_SURROGATE_PREFIX;
}

inline bool is_high_surrogate(const utf16_codeunit_type& codeunit) {
    return (codeunit & BIT_ELEVEN_TO_FIFTEEN) == HIGH_SURROGATE_PREFIX;
}

inline bool is_low_surrogate(const utf16_codeunit_type& codeunit) {
    return (codeunit & BIT_ELEVEN_TO_FIFTEEN) == LOW_SURROGATE_PREFIX;
}

template <typename Policy = error_policy, typename OutputIterator>
inline void encode(const codepoint_type& codepoint, OutputIterator out) {
    if(is_single_codeunit(codepoint)) {
        *out++ = codepoint & 0xFFFF;
    } else if(is_double_codeunit(codepoint)) {
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

template <typename Policy = error_policy, typename CodeunitInputIterator>
inline codepoint_type decode(CodeunitInputIterator& begin, const CodeunitInputIterator& end) {
    utf16_codeunit_type hi = *begin++;
    if(is_high_surrogate(hi)) {
        utf16_codeunit_type lo = *begin++;
        if(!is_low_surrogate(lo)) {
            return Policy::replace_invalid_codepoint();
        } else {
            return  (((hi & BIT_ONE_TO_TEN) << 10) + 0x10000) |
                    (lo & BIT_ONE_TO_TEN);
        }
    } else if(is_low_surrogate(hi)) {
        return Policy::replace_invalid_codepoint();
    } else {
        return codepoint_type(hi);
    }
}

} // namespace utf8
} // namespace generic
} // namespace ustr
} // namesp
