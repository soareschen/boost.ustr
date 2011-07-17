
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <assert.h>
#include <boost/ustr/detail/incl.hpp>
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
    return static_cast<unsigned char>(CONTINUATION_BYTE_PREFIX | (codepoint & BIT_ONE_TO_SIX));
}

inline unsigned char build_second_last_byte(const codepoint_type& codepoint) {
    return static_cast<unsigned char>(CONTINUATION_BYTE_PREFIX | ((codepoint >> 6) & BIT_ONE_TO_SIX));
}

inline unsigned char build_third_last_byte(const codepoint_type& codepoint) {
    return static_cast<unsigned char>(CONTINUATION_BYTE_PREFIX | ((codepoint >> 12) & BIT_ONE_TO_SIX));
}

inline codepoint_type decode_two_bytes(
        const unsigned char& first_byte, 
        const unsigned char& second_byte) 
{
    return (
        ((first_byte & BIT_ONE_TO_FIVE) << 6) | 
        (second_byte & BIT_ONE_TO_SIX)
    );
}

inline codepoint_type decode_three_bytes(
        const unsigned char& first_byte,
        const unsigned char& second_byte,
        const unsigned char& third_byte)
{
    return (
        ((first_byte & BIT_ONE_TO_FOUR) << 12) | 
        ((second_byte & BIT_ONE_TO_SIX) << 6) |
        (third_byte & BIT_ONE_TO_SIX)
    );
}

inline codepoint_type decode_four_bytes(
        const unsigned char& first_byte,
        const unsigned char& second_byte,
        const unsigned char& third_byte,
        const unsigned char& fourth_byte)
{
    return (
        ((first_byte & BIT_ONE_TO_THREE) << 18) |
        ((second_byte & BIT_ONE_TO_SIX) << 12) |
        ((third_byte & BIT_ONE_TO_SIX) << 6) |
        (fourth_byte & BIT_ONE_TO_SIX)
    );
}

template <typename Policy = error_policy>
class utf8_encoder {
  public:
    template <typename OutputIterator>
    static inline void encode(const codepoint_type& codepoint, OutputIterator out) {
        if(has_single_codeunit(codepoint)) {
            *out++ = static_cast<char>(codepoint);
        } else if(has_double_codeunit(codepoint)) {
            *out++ = static_cast<const char>(DOUBLE_BYTE_PREFIX | ((codepoint >> 6) & BIT_ONE_TO_SIX));
            *out++ = build_last_byte(codepoint);
        } else if(has_triple_codeunit(codepoint)) {
            *out++ = static_cast<const char>(TRIPLE_BYTE_PREFIX | ((codepoint >> 12) & BIT_ONE_TO_SIX));
            *out++ = build_second_last_byte(codepoint);
            *out++ = build_last_byte(codepoint);
        } else if(has_quad_codeunit(codepoint)) {
            *out++ = static_cast<const char>(QUAD_BYTE_PREFIX | ((codepoint >> 18) & BIT_ONE_TO_THREE));
            *out++ = build_third_last_byte(codepoint);
            *out++ = build_second_last_byte(codepoint);
            *out++ = build_last_byte(codepoint);
        } else {
            codepoint_type replacement_codepoint = Policy::replace_invalid_codepoint(codepoint);
            assert(replacement_codepoint != codepoint);
            encode(replacement_codepoint, out);
        }
    }

    template <typename CodeunitInputIterator>
    static inline codepoint_type decode(CodeunitInputIterator& begin, const CodeunitInputIterator& end) {
        unsigned char first_byte = *begin++;
        
        if(!is_valid_first_byte(first_byte)) {
            return Policy::replace_invalid_codepoint();
        }

        if(is_single_codeunit(first_byte)) {
            return static_cast<codepoint_type>(first_byte);
        } 

        // 2nd byte
        if(begin == end) {
            return Policy::replace_invalid_codepoint();
        }

        unsigned char second_byte = *begin++;
        if(!is_continuation_byte(second_byte)) {
            return Policy::replace_invalid_codepoint();
        }

        if(is_double_codeunit(first_byte)) {
            return check_and_return(decode_two_bytes(first_byte, second_byte));
        }

        // 3rd byte
        if(begin == end) {
            return Policy::replace_invalid_codepoint();
        }

        unsigned char third_byte = *begin++;

        if(!is_continuation_byte(third_byte)) {
            return Policy::replace_invalid_codepoint();
        }
            
        if(is_triple_codeunit(first_byte)) {
            return check_and_return(decode_three_bytes(first_byte, 
                        second_byte, third_byte));
        }

        // 4th byte
        if(begin == end) {
            return Policy::replace_invalid_codepoint();
        }

        unsigned char fourth_byte = *begin++;

        if(!is_continuation_byte(fourth_byte)) {
            return Policy::replace_invalid_codepoint();
        }

        if(is_quad_codeunit(first_byte)) {
            return check_and_return(decode_four_bytes(first_byte, 
                        second_byte, third_byte, fourth_byte));
        }

        // should not reach here
        return Policy::replace_invalid_codepoint();
    }

    /*
     * decode_previous decodes the code point previous to the second argument.
     * The second argument end is the first code unit of the current code point,
     * NOT the last code unit of the code point that we currently decoding.
     *
     * decode_previous will make sure the end iterator will never decremented 
     * beyond the first argument begin.
     *
     * decode_previous will alter the second argument end to the first code unit
     * of the currently processed code point, NOT the last code unit of the
     * further previous code point. With this the altered iterator can work properly
     * when passed to decode().
     */
    template <typename CodeunitIterator>
    static inline codepoint_type decode_previous(const CodeunitIterator& begin, CodeunitIterator& end) {

        // last
        if(end == begin) {
            return Policy::replace_invalid_codepoint();
        }

        unsigned char last_byte = *--end;

        if(!is_continuation_byte(last_byte)) {
            if(is_single_codeunit(last_byte)) {
                return static_cast<codepoint_type>(last_byte);
            } else {
                return Policy::replace_invalid_codepoint();
            }
        }

        // second last
        if(end == begin) {
            return Policy::replace_invalid_codepoint();
        }

        unsigned char second_last_byte = *--end;

        if(!is_continuation_byte(second_last_byte)) {
            if(is_double_codeunit(second_last_byte)) {
                return check_and_return(decode_two_bytes(second_last_byte, last_byte));
            } else {
                return Policy::replace_invalid_codepoint();
            }
        }

        // third last
        if(end == begin) {
            return Policy::replace_invalid_codepoint();
        }

        unsigned char third_last_byte = *--end;

        if(!is_continuation_byte(third_last_byte)) {
            if(is_triple_codeunit(third_last_byte)) {
                return check_and_return(decode_three_bytes(third_last_byte, 
                        second_last_byte, last_byte));
            } else {
                return Policy::replace_invalid_codepoint();
            }
        }

        // fourth last
        if(end == begin) {
            return Policy::replace_invalid_codepoint();
        }

        unsigned char fourth_last_byte = *--end;

        if(!is_continuation_byte(fourth_last_byte)) {
            if(is_quad_codeunit(fourth_last_byte)) {
                return check_and_return(decode_four_bytes(
                    fourth_last_byte,
                    third_last_byte,
                    second_last_byte,
                    last_byte
                ));
            } else {
                return Policy::replace_invalid_codepoint();
            }
        }

        return Policy::replace_invalid_codepoint();
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

} // namespace utf8
} // namespace encoding
} // namespace ustr
} // namespace boost
