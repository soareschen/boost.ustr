
#pragma once

#include <iterator>
#include "../incl.h"
#include "utf8.h"
#include "../policy.h"

namespace boost { 
namespace ustr {


using namespace boost::ustr::generic;

template <size_t codeunit_size, typename StringTraits, typename Policy >
class utf_encoding_traits;


/*
 * UTF-8 Encoding Traits
 */
template < 
    typename StringTraits, 
    typename Policy
>
class utf_encoding_traits<8, StringTraits, Policy> {
  public:
    typedef StringTraits                                string_traits;
    typedef typename 
        string_traits::codeunit_type                    codeunit_type;
    typedef typename 
        string_traits::codeunit_iterator_type           codeunit_iterator_type;

    typedef typename
        string_traits::raw_strptr_type                  raw_strptr_type;
    typedef typename
        string_traits::const_strptr_type                const_strptr_type;
    typedef typename
        string_traits::mutable_strptr_type              mutable_strptr_type;

    class const_codepoint_iterator;
    class mutable_codepoint_iterator;

    typedef const_codepoint_iterator                    const_codepoint_iterator_type;
    typedef mutable_codepoint_iterator                  mutable_codepoint_iterator_type;

    
    /*
     * Estimate the number of codeunits required to represent a given length of
     * codepoint. Since encodings such as UTF-8 has variable code point length,
     * it is not possible to get the precise code unit length without looking
     * through every single code point of the target string.
     *
     * Still, this is a good way to estimate the memory needed for allocation and
     * can potentially save some memory reallocation effort.
     */
    static size_t estimate_codeunit_length(size_t codepoint_length) {
        return codepoint_length * 4;
    }

    static void append_codepoint(mutable_strptr_type& str, const codepoint_type& codepoint) {
        string_traits::mutable_strptr::check_and_initialize(str);
        generic::utf8::encode<Policy>(codepoint, std::back_inserter(*str));
    }
    
    class const_codepoint_iterator {
      public:
        const_codepoint_iterator(
                codeunit_iterator_type codeunit_it,
                codeunit_iterator_type end) :
            _current(codeunit_it), _next(codeunit_it), _end(end)
        { }

        const_codepoint_iterator(codeunit_iterator_type end) :
            _current(end), _next(end), _end(end)
        { }

        /*
         * It is not possible to return a reference here, because the 
         * code point is generated on the fly by extracting code units
         * in the UTF-8 string. Please voice out if this violates the
         * rule of C++ iterators, and suggest alternative return types.`
         */
        codepoint_type operator *() {
            if(_current == _next) {
                // the iterator has just been incremented
                return utf8::decode<Policy>(_next, _end);
            } else {
                // the iterator has been dereferenced before
                // since the last increment. This shouldn't
                // happen often as most code only dereference the
                // iterator once for each increment.
                codeunit_iterator_type clone(_current);
                return utf8::decode<Policy>(clone, _end);
            }
        }

        const_codepoint_iterator& operator ++() {
            if(_current != _end) {
                if(_current == _next) {
                    // the iterator has *not* been dereferenced before
                    // since the last increment. This shouldn't
                    // happen often as most code will dereference
                    // at least once for each increment.
                    // Even without dereference, the decoding still
                    // needs to be performed as we can't otherwise
                    // know which position is the next codepoint.
                    utf8::decode<Policy>(_next, _end);
                }
                
                _current = _next;
            }
            return *this;
        }

        bool operator ==(const const_codepoint_iterator& other) {
            return _current == other._current;
        }

        bool operator !=(const const_codepoint_iterator& other) {
            return _current != other._current;
        }
      private:
        codeunit_iterator_type          _current;
        codeunit_iterator_type          _next;
        const codeunit_iterator_type    _end;
    };

    class mutable_codepoint_iterator {
      public:
        mutable_codepoint_iterator(mutable_strptr_type* str) :
            _str(str)
        { }

        mutable_codepoint_iterator& operator =(codepoint_type codepoint) {
            append_codepoint(*_str, codepoint);
            return *this;
        }

        mutable_codepoint_iterator& operator *() {
            return *this;
        }

        mutable_codepoint_iterator& operator ++() {
            return *this;
        }

        mutable_codepoint_iterator& operator ++(int) {
            return *this;
        }

      private:
        bool operator ==(const mutable_codepoint_iterator&) = delete;
        mutable_codepoint_iterator(const mutable_codepoint_iterator&) = delete;
        mutable_codepoint_iterator& operator =(const mutable_codepoint_iterator&) = delete;

        /*
         * The underlying type for mutable codepoint iterator is
         * a raw pointer to the mutable smart pointer to the string object,
         * which means it is a double pointer type that is roughly equivalend 
         * to string_type**.
         *
         * This is because the currently pointed string object can be frozen
         * and passed to the const adapter. When that happen, this mutable
         * iterator is not suppose to modify the original string object or else
         * the constness of the frozen string object will be violated.
         *
         * Supposingly the mutable iterator should be invalidated when the
         * mutable adapter is frozen, but the behavior of invalidation is not
         * nice, so the better way is to hold the pointer to the smart pointer,
         * so that when the smart pointer become null, a new string is created
         * and the content is appended to it.
         */
        mutable_strptr_type *_str;
    };

};




} // namespace ustr
} // namespace boost
