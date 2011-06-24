
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <utility>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <boost/ustr/incl.hpp>
#include <boost/ustr/policy.hpp>
#include <boost/ustr/string_traits.hpp>
#include <boost/ustr/encoding_traits.hpp>

namespace boost {
namespace ustr {

template <
    typename StringT,
    typename StringTraits,
    typename EncodingTraits
>
class unicode_string_adapter;

template <
    typename StringT,
    typename StringTraits,
    typename EncodingTraits
>
class unicode_string_adapter_builder;


template <
    typename StringT,
    typename StringTraits = string_traits<StringT>,
    typename EncodingTraits = utf_encoding_traits<
        StringTraits, replace_policy<'?'> 
    >
>
class unicode_string_adapter
{
  public:
    typedef StringTraits                                            string_traits;
    typedef EncodingTraits                                          encoding_traits;

    typedef unicode_string_adapter<
        StringT, StringTraits, EncodingTraits>                      this_type;

    typedef unicode_string_adapter<
        StringT, StringTraits, EncodingTraits>                      const_adapter_type;
    typedef unicode_string_adapter_builder<
        StringT, StringTraits, EncodingTraits>                      mutable_adapter_type;

    typedef typename string_traits::string_type                     string_type;
    typedef typename string_traits::raw_strptr_type                 raw_strptr_type;
    typedef typename string_traits::const_strptr_type               const_strptr_type;
    typedef typename string_traits::mutable_strptr_type             mutable_strptr_type;

    typedef typename string_traits::codeunit_type                   codeunit_type;

    typedef typename 
        string_traits::codeunit_iterator_type                       codeunit_iterator_type;

    typedef typename 
        encoding_traits::codepoint_iterator_type                    codepoint_iterator_type;
    typedef typename 
        std::back_insert_iterator<mutable_adapter_type>             codepoint_output_iterator_type;
        
    typedef codepoint_iterator_type                                 iterator;
    typedef codepoint_type                                          value_type;
    typedef this_type&                                              reference;

    typedef typename
        string_traits::raw_char_type                                raw_char_type;

    typedef std::allocator<codepoint_type>                          Allocator;
    
    static const size_t codeunit_size = string_traits::codeunit_size;
    
    template <typename CodeunitIterator>
    static this_type from_codeunits(CodeunitIterator begin, CodeunitIterator end) {
        mutable_strptr_type buffer;
        while(begin != end) {
            string_traits::mutable_strptr::append(buffer, *begin++);
        }

        return this_type(string_traits::mutable_strptr::release(buffer));
    }

    template <typename CodepointIterator>
    static this_type from_codepoints(CodepointIterator begin, CodepointIterator end) {
        mutable_adapter_type buffer;
        std::copy(begin, end, buffer.begin());

        return buffer.freeze();
    }

    /*
     * Implicit lightweight copy construction from other const adapter.
     * The two const adapter will share the same underlying buffer since 
     * they are immutable.
     */
    unicode_string_adapter(const this_type& other) :
        _buffer(other.get_buffer())
    {
        validate();
    }

    /*
     * Implicit lightweight move construction from other const adapter.
     * The move construction skips the atomic reference count increment
     * of the smart pointer to the underlying shared buffer.
     */
    #ifdef BOOST_USTR_CPP0X
    unicode_string_adapter(this_type&& other) :
        _buffer(std::move(other._buffer))
    {
        validate();
    }
    #endif

    /*
     * Implicit conversion from any const adapter of different encodings.
     */
    template <typename StringT_, typename StringTraits_, typename EncodingTraits_>
    unicode_string_adapter(const unicode_string_adapter<
                StringT_, StringTraits_, EncodingTraits_>& other)
    {
        mutable_adapter_type buffer;
        buffer.append(other);

        _buffer.reset(buffer.release());
    }

    /*
     * Explicit construction from a raw string reference.
     * A new copy of string content is allocated.
     */
    explicit unicode_string_adapter(const string_type& other) :
        _buffer(string_traits::new_string(other))
    {
        validate();
    }

    /*
     * Explicit lightweight copy construction from a const pointer to the string.
     */
    explicit unicode_string_adapter(const const_strptr_type& other) :
        _buffer(other)
    {
        validate();
    }

    /*
     * Explicit lightweight move construction from a const pointer to the string.
     */
    #ifdef BOOST_USTR_CPP0X
    explicit unicode_string_adapter(const_strptr_type&& other) :
        _buffer(std::forward<const_strptr_type>(other))
    {
        validate();
    }
    #endif

    /*
     * Explicit lightweight construction from a raw pointer to the string.
     * The raw pointer is adopted and now owned by this class.
     */
    explicit unicode_string_adapter(raw_strptr_type other) :
        _buffer(other)
    {
        validate();
    }


    /*
     * Explicit copy construction from existing mutable adapter.
     */
    explicit unicode_string_adapter(const mutable_adapter_type& other) :
        _buffer(other.clone_buffer())
    {
        validate();
    }


    /*
     * Implicit move construction from existing mutable adapter.
     */
    #ifdef BOOST_USTR_CPP0X
    unicode_string_adapter(mutable_adapter_type&& other) :
        _buffer(other.release())
    {
        validate();
    }
    #endif

    mutable_adapter_type edit() const {
        const raw_strptr_type original_buffer = string_traits::const_strptr::get(_buffer);
        raw_strptr_type new_buffer = string_traits::clone_string(original_buffer);
        return mutable_adapter_type(new_buffer);
    }

    codepoint_iterator_type begin() const {
        return codepoint_begin();
    }

    codepoint_iterator_type end() const {
        return codepoint_end();
    }

    codepoint_iterator_type codepoint_begin() const {
        return codepoint_iterator_type(
                string_traits::const_strptr::codeunit_begin(_buffer), 
                string_traits::const_strptr::codeunit_end(_buffer));
    }

    codepoint_iterator_type codepoint_end() const {
        return codepoint_iterator_type(
                string_traits::const_strptr::codeunit_end(_buffer));
    }

    const_strptr_type get_buffer() const {
        return _buffer;
    }

    /*
     * Create a string object on stack from the underlying string object on heap.
     * It is not possible to return a reference as the new string object is created
     * on the fly.
     */
    string_type to_string() const {
        return *string_traits::const_strptr::get(_buffer);
    }

    /*
     * Compare any two const string adapter with different encodings to determine
     * whether they contains the same code point. The comparison is done at code point
     * level so it may not be entirely accurate, but it is more efficient. A full blown
     * normalized comparison function is available at the supplement Unicode utility library.
     */
    template <typename StringT_, typename StringTraits_, typename EncodingTraits_>
    bool operator ==(const unicode_string_adapter<
            StringT_, StringTraits_, EncodingTraits_>& other) const 
    {
        typedef unicode_string_adapter<
            StringT_, StringTraits_, EncodingTraits_>       other_type;
        typedef typename
            other_type::codepoint_iterator_type             other_codepoint_iterator_type;

        if(other.codepoint_length() != codepoint_length()) {
            return false;
        }

        codepoint_iterator_type it = codepoint_begin();
        other_codepoint_iterator_type other_it = other.codepoint_begin();

        while(it != codepoint_end()) {
            if(*it != *other_it) {
                return false;
            }
            ++it;
            ++other_it;
        }

        return true;
    }

    template <typename StringT_, typename StringTraits_, typename EncodingTraits_>
    this_type operator +(const unicode_string_adapter<
            StringT_, StringTraits_, EncodingTraits_>& other) const
    {
        return add(other);
    }

    template <typename StringT_, typename StringTraits_, typename EncodingTraits_>
    this_type add(const unicode_string_adapter<
            StringT_, StringTraits_, EncodingTraits_>& other) const
    {
        mutable_adapter_type buffer;
        buffer.append(*this);
        buffer.append(other);
        return buffer.freeze();
    }
    
    string_type operator *() {
        return to_string();
    }

    size_t length() const {
        return codepoint_length();
    }

    size_t codepoint_length() const {
        size_t length = 0;
        for(codepoint_iterator_type it = codepoint_begin(); it != codepoint_end(); ++it) {
            ++length;
        }
        return length;
    }

    void validate() {

    }

  private:
    const_strptr_type _buffer;
};

template <
    typename StringT,
    typename StringTraits = string_traits<StringT>,
    typename EncodingTraits = utf_encoding_traits< 
        StringTraits, replace_policy<'?'> 
    >
>
class unicode_string_adapter_builder
{
  public:
    typedef StringTraits                                            string_traits;
    typedef EncodingTraits                                          encoding_traits;

    typedef unicode_string_adapter_builder<
        StringT, StringTraits, EncodingTraits>                      this_type;
    typedef this_type&                                     reference;

    typedef unicode_string_adapter<
        StringT, StringTraits, EncodingTraits>                      const_adapter_type;
    typedef unicode_string_adapter_builder<
        StringT, StringTraits, EncodingTraits>                      mutable_adapter_type;

    typedef typename string_traits::string_type                     string_type;
    typedef typename string_traits::raw_strptr_type                 raw_strptr_type;
    typedef typename string_traits::const_strptr_type               const_strptr_type;
    typedef typename string_traits::mutable_strptr_type             mutable_strptr_type;

    typedef typename string_traits::codeunit_type                   codeunit_type;

    typedef typename 
        string_traits::codeunit_iterator_type                       codeunit_iterator_type;

    typedef typename 
        encoding_traits::codepoint_iterator_type                    codepoint_iterator_type;

    typedef typename 
        std::back_insert_iterator<this_type>                        codepoint_output_iterator_type;

    typedef codepoint_output_iterator_type                          iterator;
    typedef codepoint_type                                          value_type;

    typedef std::allocator<codepoint_type>                          Allocator;
    typedef const codepoint_type&                     const_reference;

    
    unicode_string_adapter_builder() :
        _buffer()
    { }

    raw_strptr_type clone_buffer() {
        raw_strptr_type original_buffer = string_traits::mutable_strptr::get(_buffer);
        raw_strptr_type new_buffer = string_traits::clone_string(original_buffer);
        return new_buffer;
    }

    raw_strptr_type release() {
        return string_traits::mutable_strptr::release(_buffer);
    }

    /*
     * Lightweight conversion from mutable adapter to const adapter by
     * passing the underlying buffer to the new const adapter object.
     *
     * The difference of this with the explicit copy construction:
     *
     *      unicode_string_adapter(const unicode_string_adapter_builder&)
     *
     * is that the above is a copy construction where a new copy of string is created.
     */
    const_adapter_type freeze() {
        return const_adapter_type(string_traits::mutable_strptr::release(_buffer));
    }

    codepoint_output_iterator_type begin() {
        return codepoint_begin();
    }

    codepoint_output_iterator_type codepoint_begin() {
        return codepoint_output_iterator_type(*this);
    }

    /*
     * The append operations are not thread safe
     */
    void append(const codepoint_type& codepoint) {
        encoding_traits::append_codepoint(_buffer, codepoint);
    }

    void push_back(const codepoint_type& codepoint) {
        append(codepoint);
    }

    template <
        typename StringT_, typename StringTraits_, typename EncodingTraits_>
    void append(const unicode_string_adapter<
        StringT_, StringTraits_, EncodingTraits_>& str) {

        // size_t codeunit_length = encoding_traits::estimate_codeunit_length(str.codepoint_length());
        // string_traits::mutable_strptr::reserve_space(_buffer, codeunit_length);

        std::copy(str.begin(), str.end(), begin());
    }

  private:
    unicode_string_adapter_builder(this_type&);
    bool operator ==(const this_type&) const;
    this_type& operator =(const this_type&);

    mutable_strptr_type _buffer;
};



} // namespace ustr
} // namespace boost
