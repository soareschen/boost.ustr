
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <string>
#include <iterator>
#include <boost/ustr/incl.hpp>
#include <boost/ustr/policy.hpp>
#include <boost/ustr/detail/utf8.hpp>
#include <boost/ustr/detail/utf16.hpp>

namespace boost { 
namespace ustr {

using boost::ustr::encoding::utf8::utf8_encoder;
using boost::ustr::encoding::utf16::utf16_encoder;

template <size_t codeunit_size, typename Policy>
class encoding_engine { 

};

template <typename Policy>
class encoding_engine<1, Policy> {
  public:
    typedef utf8_encoder<Policy>    type;
};

template <typename Policy>
class encoding_engine<2, Policy> {
  public:
    typedef utf16_encoder<Policy>   type;
};


/*
 * UTF-8 Encoding Traits
 */
template < typename StringTraits, typename Policy = replace_policy<'?'> >
class utf_encoding_traits {
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

    typedef typename encoding_engine<
        string_traits::codeunit_size, Policy >::type    encoder;

    class codepoint_iterator;

    typedef codepoint_iterator                          codepoint_iterator_type;
    
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
        return codepoint_length;
    }

    static void append_codepoint(mutable_strptr_type& str, const codepoint_type& codepoint) {
        string_traits::mutable_strptr::check_and_initialize(str);
        encoder::encode(codepoint, std::back_inserter(*str));
    }
    
    class codepoint_iterator :
        public std::iterator<std::bidirectional_iterator_tag, codepoint_type>
    {
      public:
        typedef codepoint_type                      reference;
        typedef const codepoint_type                const_reference;

        codepoint_iterator(
                codeunit_iterator_type codeunit_it,
                codeunit_iterator_type begin,
                codeunit_iterator_type end) :
            _current(codeunit_it), _next(codeunit_it), 
            _begin(begin), _end(end)
        { }

        codepoint_iterator(const codepoint_iterator& other) :
            _current(other._current), _next(other._next), 
            _begin(other._begin), _end(other._end)
        { }
        
        codepoint_iterator& operator =(const codepoint_iterator& other) {
            _current = other._current;
            _next = other._next;
            _begin = other._begin;
            _end = other._end;
            
            return *this;
        }

        /*
         * It is not possible to return a reference here, because the 
         * code point is generated on the fly by extracting code units
         * in the UTF-8 string. Please voice out if this violates the
         * rule of C++ iterators, and suggest alternative return types.`
         */
        const codepoint_type operator *() const {
            if(_current == _next) {
                // the iterator has just been incremented
                return encoder::decode(_next, _end);
            } else {
                // the iterator has been dereferenced before
                // since the last increment. This shouldn't
                // happen often as most code only dereference the
                // iterator once for each increment.
                codeunit_iterator_type clone(_current);
                return encoder::decode(clone, _end);
            }
        }

        const codepoint_iterator& operator ++() const {
            increment();
            return *this;
        }

        codepoint_iterator& operator ++() {
            increment();
            return *this;
        }

        codepoint_iterator operator ++(int) const {
            codepoint_iterator it(*this);
            increment();
            return it;
        }

        codepoint_iterator& operator --() {
            decrement();
            return *this;
        }

        const codepoint_iterator& operator --() const {
            decrement();
            return *this;
        }

        codepoint_iterator operator --(int) const {
            codepoint_iterator it(*this);
            decrement();
            return it;
        }

        void increment() const {
            if(_current != _end) {
                if(_current == _next) {
                    // the iterator has *not* been dereferenced before
                    // since the last increment. This shouldn't
                    // happen often as most code will dereference
                    // at least once for each increment.
                    // Even without dereference, the decoding still
                    // needs to be performed as we can't otherwise
                    // know which position is the next codepoint.
                    encoder::decode(_next, _end);
                }
                
                _current = _next;
            }
        }

        void decrement() const {
            if(_current != _begin) {
                if(_current == _next) {
                    // Since _next == _current, after decrementing _next 
                    // will point right after _current so _next still
                    // remains valid.
                    encoder::decode_previous(_begin, _current);
                } else {
                    // If _next is already pointing after _current then 
                    // after decrement _next would become invalid. 
                    // Hence we'll reset _next to be the same as _current.
                    encoder::decode_previous(_begin, _current);
                    _next = _current;
                }
            }
        }

        bool operator ==(const codepoint_iterator& other) const {
            return _current == other._current;
        }

        bool operator !=(const codepoint_iterator& other) const {
            return !(_current == other._current);
        }

      private:
        mutable codeunit_iterator_type          _current;
        mutable codeunit_iterator_type          _next;
        codeunit_iterator_type            _begin;
        codeunit_iterator_type            _end;
    };


};




} // namespace ustr
} // namespace boost
