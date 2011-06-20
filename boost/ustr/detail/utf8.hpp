
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <assert.h>
#include <boost/ustr/incl.hpp>
#include <boost/ustr/policy.hpp>

namespace boost {
namespace ustr {
namespace encoding {
namespace utf8 {

using namespace boost::ustr;

static const unsigned char DOUBLE_BYTE_PREFIX =         0xC0u;      // 11000000
static const unsigned char TRIPLE_BYTE_PREFIX =         0xE0u;      // 11100000
static const unsigned char QUAD_BYTE_PREFIX =           0xF0u;      // 11110000
static const unsigned char CONTINUATION_BYTE_PREFIX =   0X80u;      // 10000000

static const unsigned char BIT_ONE_TO_SIX =             0x3Fu;      // 00111111
static const unsigned char BIT_ONE_TO_FIVE =            0x1Fu;      // 00011111
static const unsigned char BIT_ONE_TO_FOUR =            0x0Fu;      // 00001111
static const unsigned char BIT_ONE_TO_THREE =           0x07u;      // 00000111


inline bool is_valid_codepoint(const codepoint_type& codepoint) {
    return codepoint <= 0x10FFFFu;
}


inline bool has_single_codeunit(const codepoint_type& codepoint) {
    return codepoint < 0x80u;
}

inline bool has_double_codeunit(const codepoint_type& codepoint) {
    return 0x79u < codepoint && codepoint < 0x800u;
}

inline bool has_triple_codeunit(const codepoint_type& codepoint) {
    return 0x7FFu < codepoint && codepoint < 0x10000u;
}

inline bool has_quad_codeunit(const codepoint_type& codepoint) {
    return 0xFFFFu < codepoint && codepoint < 0x110000u;
}

inline bool is_continuation_byte(const unsigned char& codeunit) {
    return (codeunit & 0xC0u) == CONTINUATION_BYTE_PREFIX;
}

inline bool is_valid_first_byte(const unsigned char& codeunit) {
    return  !is_continuation_byte(codeunit) && 
            codeunit != 0xC0 &&
            codeunit != 0xC1 &&
            codeunit < 0xF5;
}

inline bool is_single_codeunit(const unsigned char& codeunit) {
    return (codeunit & CONTINUATION_BYTE_PREFIX) == 0;
}

inline bool is_double_codeunit(const unsigned char& codeunit) {
    return (codeunit & 0xE0u) == DOUBLE_BYTE_PREFIX;
}

inline bool is_triple_codeunit(const unsigned char& codeunit) {
    return (codeunit & 0xF0u) == TRIPLE_BYTE_PREFIX;
}

inline bool is_quad_codeunit(const unsigned char& codeunit) {
    return (codeunit & 0xF8u) == QUAD_BYTE_PREFIX;
}

inline unsigned char build_last_byte(const codepoint_type& codepoint) {
    return CONTINUATION_BYTE_PREFIX | (codepoint & BIT_ONE_TO_SIX);
}

inline unsigned char build_second_last_byte(const codepoint_type& codepoint) {
    return CONTINUATION_BYTE_PREFIX | ((codepoint >> 6) & BIT_ONE_TO_SIX);
}

inline unsigned char build_third_last_byte(const codepoint_type& codepoint) {
    return CONTINUATION_BYTE_PREFIX | ((codepoint >> 12) & BIT_ONE_TO_SIX);
}

template <typename Policy>
inline codepoint_type check_and_return(const codepoint_type& codepoint, Policy policy = Policy()) {
    if(is_valid_codepoint(codepoint)) {
        return codepoint;
    } else {
        return policy.replace_invalid_codepoint(codepoint);
    }
}

} // namespace utf8

using namespace boost::ustr::encoding::utf8;

template <typename Policy = error_policy>
class utf8_encoder {
  public:
    template <typename OutputIterator>
    static inline void encode(const codepoint_type& codepoint, OutputIterator out, Policy policy = Policy()) {
        if(has_single_codeunit(codepoint)) {
            *out++ = codepoint;
        } else if(has_double_codeunit(codepoint)) {
            *out++ = DOUBLE_BYTE_PREFIX | ((codepoint >> 6) & BIT_ONE_TO_SIX);
            *out++ = build_last_byte(codepoint);
        } else if(has_triple_codeunit(codepoint)) {
            *out++ = TRIPLE_BYTE_PREFIX | ((codepoint >> 12) & BIT_ONE_TO_SIX);
            *out++ = build_second_last_byte(codepoint);
            *out++ = build_last_byte(codepoint);
        } else if(has_quad_codeunit(codepoint)) {
            *out++ = QUAD_BYTE_PREFIX | ((codepoint >> 18) & BIT_ONE_TO_THREE);
            *out++ = build_third_last_byte(codepoint);
            *out++ = build_second_last_byte(codepoint);
            *out++ = build_last_byte(codepoint);
        } else {
            codepoint_type replacement_codepoint = policy.replace_invalid_codepoint(codepoint);
            assert(replacement_codepoint != codepoint);
            encode(replacement_codepoint, out);
        }
    }

    template <typename CodeunitInputIterator>
    static inline codepoint_type decode(CodeunitInputIterator& begin, const CodeunitInputIterator& end, Policy policy = Policy()) {
        unsigned char first_byte = *begin++;
        
        if(!is_valid_first_byte(first_byte)) {
            return policy.replace_invalid_codepoint();
        }

        if(is_single_codeunit(first_byte)) {
            return first_byte;
        } 

        // 2nd byte
        if(begin == end) {
            return policy.replace_invalid_codepoint();
        }

        unsigned char second_byte = *begin++;
        if(!is_continuation_byte(second_byte)) {
            return policy.replace_invalid_codepoint();
        }

        if(is_double_codeunit(first_byte)) {
            return check_and_return<Policy>(
                ((first_byte & BIT_ONE_TO_FIVE) << 6) | 
                (second_byte & BIT_ONE_TO_SIX)
            );
        }

        // 3rd byte
        if(begin == end) {
            return policy.replace_invalid_codepoint();
        }

        unsigned char third_byte = *begin++;

        if(!is_continuation_byte(third_byte)) {
            return policy.replace_invalid_codepoint();
        }
            
        if(is_triple_codeunit(first_byte)) {
            return check_and_return<Policy>(
                ((first_byte & BIT_ONE_TO_FOUR) << 12) | 
                ((second_byte & BIT_ONE_TO_SIX) << 6) |
                (third_byte & BIT_ONE_TO_SIX)
            );
        }

        // 4th byte
        if(begin == end) {
            return policy.replace_invalid_codepoint();
        }

        unsigned char fourth_byte = *begin++;

        if(!is_continuation_byte(fourth_byte)) {
            return policy.replace_invalid_codepoint();
        }

        if(is_quad_codeunit(first_byte)) {
            return check_and_return<Policy>(
                ((first_byte & BIT_ONE_TO_THREE) << 18) |
                ((second_byte & BIT_ONE_TO_SIX) << 12) |
                ((third_byte & BIT_ONE_TO_SIX) << 6) |
                (fourth_byte & BIT_ONE_TO_SIX)
            );
        }

        // should not reach here
        return policy.replace_invalid_codepoint();
    }

};

} // namespace encoding
} // namespace ustr
} // namespace boost
