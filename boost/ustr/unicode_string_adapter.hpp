
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <algorithm>
#include <iterator>
#include <boost/static_assert.hpp>
#include <boost/concept_check.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/ustr/policy.hpp>
#include <boost/ustr/string_traits.hpp>
#include <boost/ustr/encoding_traits.hpp>
#include <boost/ustr/detail/incl.hpp>
#include <boost/ustr/detail/unicode_string_adapter_concepts.hpp>

#define USTR(str) \
    ::boost::ustr::unicode_string_adapter< USTR_STRING_TYPE >(USTR_RAW(str))

namespace boost {
namespace ustr {

template <
    typename StringT,
    typename StringTraits = string_traits<StringT>,
    typename EncodingTraits = utf_encoding_traits<StringTraits>
>
class unicode_string_adapter;

template <
    typename StringT,
    typename StringTraits = string_traits<StringT>,
    typename EncodingTraits = utf_encoding_traits<StringTraits>
>
class unicode_string_adapter_builder;

typedef unicode_string_adapter< std::string >   u8_string;

typedef unicode_string_adapter< 
    std::basic_string<utf16_codeunit_type> >    u16_string;

typedef unicode_string_adapter<
    std::basic_string<codepoint_type> >         u32_string;

template <
    typename StringT,
    typename StringTraits,
    typename EncodingTraits
>
std::ostream&
operator <<(std::ostream& out, 
    const unicode_string_adapter<StringT, StringTraits, EncodingTraits>& str)
{
    return out << *str;
}

/*!
 * Unicode String Adapter class
 *
 * An adapter class for Unicode strings. 
 */
template <
    typename StringT,
    typename StringTraits,
    typename EncodingTraits
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

    typedef std::back_insert_iterator<mutable_adapter_type>         codepoint_output_iterator_type;
    //typedef custom_insert_iterator<mutable_adapter_type, 
    //        codeunit_append_inserter_traits<mutable_adapter_type> > codeunit_output_iterator_type;
        
    typedef typename
        string_traits::raw_char_type                                raw_char_type;

    // STL Container Boilerplate typedefs
    typedef std::allocator<codepoint_type>                          allocator_type;
    typedef codepoint_iterator_type                                 iterator;
    typedef const codepoint_iterator_type                           const_iterator;
    typedef std::reverse_iterator<iterator>                         reverse_iterator;
    typedef const reverse_iterator                                  const_reverse_iterator;

    typedef size_t                                                  size_type;
    typedef ptrdiff_t                                               difference_type;
    typedef codepoint_type                                          value_type;
    typedef codepoint_type&                                         reference;
    typedef const codepoint_type&                                   const_reference;
    typedef codepoint_type*                                         pointer;
    typedef const codepoint_type*                                   const_pointer;

    static const size_t codeunit_size = string_traits::codeunit_size;

    BOOST_CONCEPT_ASSERT((unicode_string_adapter_concepts<StringT, StringTraits, EncodingTraits>));

    template <typename CodeunitIterator>
    static this_type from_codeunits(CodeunitIterator begin, CodeunitIterator end) {
        BOOST_STATIC_ASSERT((
                 sizeof(typename std::iterator_traits<CodeunitIterator>::value_type) 
                 == codeunit_size));

        mutable_adapter_type buffer;
        std::copy(begin, end, buffer.codeunit_begin());

        return buffer.freeze();
    }

    template <typename CodepointIterator>
    static this_type from_codepoints(CodepointIterator begin, CodepointIterator end) {
        BOOST_STATIC_ASSERT((
                 sizeof(typename std::iterator_traits<CodepointIterator>::value_type) 
                 == sizeof(codepoint_type)));

        mutable_adapter_type buffer;
        std::copy(begin, end, buffer.begin());

        return buffer.freeze();
    }

    template <typename CodeunitIterator>
    static codepoint_iterator<CodeunitIterator, typename encoding_traits::encoder, typename encoding_traits::policy>
    make_codepoint_iterator(CodeunitIterator current, CodeunitIterator begin, CodeunitIterator end) {
        return codepoint_iterator<CodeunitIterator, 
               typename encoding_traits::encoder, typename encoding_traits::policy>(current, begin, end);
    }

    static this_type from_const_strptr(const const_strptr_type& other) {
        this_type str;
        str._buffer = other;
        str.validate();

        return str;
    }

    static this_type from_ptr(raw_strptr_type other) {
        this_type str;
        string_traits::const_strptr::reset(str._buffer, other);
        str.validate();

        return str;
    }

    unicode_string_adapter() : _buffer() { }

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
#ifndef BOOST_NO_RVALUE_REFERENCES
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

        string_traits::const_strptr::reset(_buffer, buffer.release());
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
     * Explicit lightweight move construction from a const pointer to the string.
     */
#ifndef BOOST_NO_RVALUE_REFERENCES
    explicit unicode_string_adapter(const_strptr_type&& other) :
        _buffer(std::forward<const_strptr_type>(other))
    {
        validate();
    }
#endif


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
#ifndef BOOST_NO_RVALUE_REFERENCES
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

    codepoint_iterator_type begin() {
        return codepoint_iterator_type(
                string_traits::const_strptr::codeunit_begin(_buffer), 
                string_traits::const_strptr::codeunit_begin(_buffer), 
                string_traits::const_strptr::codeunit_end(_buffer));
    }

    const codepoint_iterator_type begin() const {
        return codepoint_iterator_type(
                string_traits::const_strptr::codeunit_begin(_buffer), 
                string_traits::const_strptr::codeunit_begin(_buffer), 
                string_traits::const_strptr::codeunit_end(_buffer));
    }

    codepoint_iterator_type end() {
        return codepoint_iterator_type(
                string_traits::const_strptr::codeunit_end(_buffer),
                string_traits::const_strptr::codeunit_begin(_buffer), 
                string_traits::const_strptr::codeunit_end(_buffer));
    }

    const codepoint_iterator_type end() const {
        return codepoint_iterator_type(
                string_traits::const_strptr::codeunit_end(_buffer),
                string_traits::const_strptr::codeunit_begin(_buffer), 
                string_traits::const_strptr::codeunit_end(_buffer));
    }

    const_strptr_type get_buffer() const {
        return _buffer;
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    /*
     * Create a string object on stack from the underlying string object on heap.
     * It is not possible to return a reference as the new string object is created
     * on the fly.
     */
    const string_type& to_string() const {
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

        codepoint_iterator_type it = begin();
        other_codepoint_iterator_type other_it = other.begin();

        while(it != end()) {
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
        return concat(other);
    }

    template <typename StringT_, typename StringTraits_, typename EncodingTraits_>
    this_type concat(const unicode_string_adapter<
            StringT_, StringTraits_, EncodingTraits_>& other) const
    {
        mutable_adapter_type buffer;
        buffer.append(*this);
        buffer.append(other);
        return buffer.freeze();
    }
    
    const string_type& operator *() const {
        return to_string();
    }

    size_t length() const {
        return codepoint_length();
    }

    size_t codepoint_length() const {
        size_t length = 0;
        for(codepoint_iterator_type it = begin(); it != end(); ++it) {
            ++length;
        }
        return length;
    }

    void validate() {
        bool valid = encoding_traits::validate(
                string_traits::const_strptr::codeunit_begin(_buffer), 
                string_traits::const_strptr::codeunit_end(_buffer));

        if(!valid && encoding_traits::replace_malformed) {
            mutable_adapter_type sanitized;
            sanitized.append(*this);
            string_traits::const_strptr::reset(_buffer, sanitized.release());
        }
    }

  private:
    const_strptr_type _buffer;
};

template <
    typename StringT,
    typename StringTraits,
    typename EncodingTraits
>
class unicode_string_adapter_builder
{
  public:
    typedef StringTraits                                            string_traits;
    typedef EncodingTraits                                          encoding_traits;

    typedef unicode_string_adapter_builder<
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

    class codeunit_adapter_builder_proxy;
    typedef std::back_insert_iterator<mutable_adapter_type>         codepoint_output_iterator_type;
    typedef std::back_insert_iterator<
        codeunit_adapter_builder_proxy>                             codeunit_output_iterator_type;

    // STL Container Boilerplate typedefs
    typedef std::allocator<codepoint_type>                          allocator_type;
    typedef codepoint_output_iterator_type                          iterator;
    typedef const codepoint_output_iterator_type                    const_iterator;
    typedef size_t                                                  size_type;
    typedef ptrdiff_t                                               difference_type;
    typedef codepoint_type                                          value_type;
    typedef codepoint_type&                                         reference;
    typedef const codepoint_type&                                   const_reference;
    typedef codepoint_type*                                         pointer;
    typedef const codepoint_type*                                   const_pointer;
    
    static const size_t codeunit_size = string_traits::codeunit_size;

    BOOST_CONCEPT_ASSERT((unicode_string_adapter_concepts<StringT, StringTraits, EncodingTraits>));

    unicode_string_adapter_builder() :
        _buffer()
    { }

#ifndef BOOST_NO_RVALUE_REFERENCES
    unicode_string_adapter_builder(this_type&& other) :
        _buffer(string_traits::mutable_strptr::release(other._buffer))
    { }
#endif

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
        return const_adapter_type::from_ptr(string_traits::mutable_strptr::release(_buffer));
    }

    const_adapter_type freeze_copy() {
        return const_adapter_type::from_ptr(clone_buffer());
    }

    codepoint_output_iterator_type begin() {
        return codepoint_output_iterator_type(*this);
    }

    codeunit_output_iterator_type codeunit_begin() {
        /*
         * We have to hack this so that the compiler thinks that the object *this is from 
         * the builder proxy class so that it can call proxy methods and typedefs from that
         * class instead.
         */
        return codeunit_output_iterator_type(
                *reinterpret_cast<codeunit_adapter_builder_proxy*>(this));
    }

    void append(const codepoint_type& codepoint) {
        append_codepoint(codepoint);
    }

    /*
     * The append operation is not thread safe
     */
    void append_codepoint(const codepoint_type& codepoint) {
        encoding_traits::append_codepoint(_buffer, codepoint);
    }

    void push_back(const codepoint_type& codepoint) {
        append_codepoint(codepoint);
    }

    template <typename CodeUnit>
    void append_codeunit(const CodeUnit& codeunit) {
        // make sure that the code unit is that same size as the string
        // adapter's code unit size. We can't simply make the parameter
        // type as codeunit_type because the compiler would automatically
        // make coercion towards arguments of different size.
        BOOST_STATIC_ASSERT(sizeof(CodeUnit) == codeunit_size);
        
        // For simplicity at this moment, we'll leave the encoding validation 
        // during freeze().
        string_traits::mutable_strptr::append(_buffer, 
                static_cast<codeunit_type>(codeunit));
    }

    template <
        typename StringT_, typename StringTraits_, typename EncodingTraits_>
    void append(const unicode_string_adapter<
        StringT_, StringTraits_, EncodingTraits_>& str) {

        // size_t codeunit_length = encoding_traits::estimate_codeunit_length(str.codepoint_length());
        // string_traits::mutable_strptr::reserve_space(_buffer, codeunit_length);

        std::copy(str.begin(), str.end(), begin());
    }

    /*
     * This is a C++ hack to make the compiler "thinks" that unicode_string_adapter_builder's objects 
     * are objects from another class, the codeunit_adapter_builder_proxy. This is so that the compiler 
     * takes the typedefs of this builder proxy instead of the original builder class, because we want 
     * to make the builder work with code units instead of code points.
     *
     * Because STL algorithms take the typedefs seriously and we can't typedef both code units and code 
     * points as value_type, the only way to get around this restriction is to make this builder proxy 
     * class and reinterpret_cast the *this pointer of the builder objects into builder proxy objects.
     */
    class codeunit_adapter_builder_proxy {
      public:
        // STL Container Boilerplate typedefs
        typedef codeunit_type                               value_type;
        typedef std::back_insert_iterator<
            codeunit_adapter_builder_proxy>                 iterator;
        typedef const iterator                              const_iterator;
        typedef std::allocator<value_type>                  allocator_type;
        typedef size_t                                      size_type;
        typedef ptrdiff_t                                   difference_type;
        typedef value_type&                                 reference;
        typedef const value_type&                           const_reference;
        typedef value_type*                                 pointer;
        typedef const value_type*                           const_pointer;

        /*
         * std::back_insert_iterator will call the push_back() method of the container,
         * but we want it to call append_codeunit(), so the hack here is to cast *this 
         * back to the builder class and call the right method.
         */
        void push_back(const_reference codeunit) {
            as_original()->append_codeunit(codeunit);
        }

        this_type* as_original() {
            return reinterpret_cast<this_type*>(this);
        }

      private:
        /*
         * The default constructor is private and undefined so it is impossible to create
         * actual objects of this class.
         */
        codeunit_adapter_builder_proxy();
        codeunit_adapter_builder_proxy& operator =(const codeunit_adapter_builder_proxy&);
    };

  private:
    unicode_string_adapter_builder(const this_type&);
    bool operator ==(const this_type&) const;
    this_type& operator =(const this_type&);

    mutable_strptr_type _buffer;
};


} // namespace ustr
} // namespace boost
