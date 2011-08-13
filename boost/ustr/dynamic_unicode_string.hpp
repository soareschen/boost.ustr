
#include <boost/ustr/unicode_string_adapter.hpp>

namespace boost {
namespace ustr {

template <typename ConstStringAdapter>
class dynamic_unicode_string_adapter_type { };


class unicode_string_type {
  public:
    unicode_string_type() { }

    bool operator ==(const unicode_string_type& other) const {
        return this == &other;
    }

  private:
    unicode_string_type(const unicode_string_type&);
    unicode_string_type& operator =(const unicode_string_type&);
};

template <
    typename StringT, typename StringTraits,
    typename EncoderTraits, typename Policy
>
class unicode_string_type_identifier {
  public:
    static const unicode_string_type      type;
};

template <
    typename StringT, typename StringTraits,
    typename EncoderTraits, typename Policy
>
const unicode_string_type unicode_string_type_identifier<
    StringT, StringTraits,
    EncoderTraits, Policy>::type;


class dynamic_codepoint_iterator_object 
    : public std::iterator<std::bidirectional_iterator_tag, codepoint_type>
{
  public:
    virtual const codepoint_type dereference() const = 0;
    virtual void increment() const = 0;
    virtual void decrement() const = 0;
    virtual bool equals(const dynamic_codepoint_iterator_object* other) const = 0;
    virtual const unicode_string_type& get_type() const = 0;
    virtual void* get_raw_iterator() const = 0;

    virtual ~dynamic_codepoint_iterator_object() { }
};

class dynamic_unicode_string_object {
  public:
    virtual dynamic_codepoint_iterator_object* begin() const = 0;
    virtual dynamic_codepoint_iterator_object* end() const = 0;
    virtual const unicode_string_type& get_type() const = 0;

    virtual ~dynamic_unicode_string_object() { }
};

class dynamic_codepoint_iterator : 
    public std::iterator<std::bidirectional_iterator_tag, codepoint_type>
{
  public:
    typedef size_t                                                  size_type;
    typedef ptrdiff_t                                               difference_type;
    typedef codepoint_type                                          value_type;
    typedef codepoint_type&                                         reference;
    typedef const codepoint_type&                                   const_reference;
    typedef codepoint_type*                                         pointer;
    typedef const codepoint_type*                                   const_pointer;

    dynamic_codepoint_iterator(dynamic_codepoint_iterator_object* it)
        : _it(it) { }

    dynamic_codepoint_iterator(const dynamic_codepoint_iterator& other)
        : _it(other._it) { }

    const codepoint_type operator *() const {
        return get()->dereference();
    }

    codepoint_type operator *() {
        return get()->dereference();
    }

    const dynamic_codepoint_iterator& operator ++() const {
        get()->increment();
        return *this;
    }

    dynamic_codepoint_iterator& operator ++() {
        get()->increment();
        return *this;
    }

    bool operator ==(const dynamic_codepoint_iterator& other) const {
        return get()->equals(other.get());
    }

    bool operator !=(const dynamic_codepoint_iterator& other) const {
        return !get()->equals(other.get());
    }

    dynamic_codepoint_iterator& operator =(const dynamic_codepoint_iterator& other) {
        _it = other._it;
        return *this;
    }

    dynamic_codepoint_iterator_object* get() const {
        return _it.get();
    }
  private:
    boost::shared_ptr<dynamic_codepoint_iterator_object> _it;
};

class dynamic_unicode_string {
  public:
    // STL Container Boilerplate typedefs
    typedef std::allocator<codepoint_type>                          allocator_type;
    typedef dynamic_codepoint_iterator                              iterator;
    typedef const iterator                                          const_iterator;
    typedef std::reverse_iterator<iterator>                         reverse_iterator;
    typedef const reverse_iterator                                  const_reverse_iterator;

    template <typename UnicodeStringAdapter>
    static dynamic_unicode_string create_dynamic_string(const UnicodeStringAdapter& str) {
        typedef typename dynamic_unicode_string_adapter_type<
            UnicodeStringAdapter>::type                     impl_type;

        return dynamic_unicode_string(new impl_type(str));
    }

    dynamic_unicode_string(dynamic_unicode_string_object* str) 
        : _str(str) 
    { }

    dynamic_unicode_string(const dynamic_unicode_string& other)
        : _str(other._str)
    { }

    dynamic_codepoint_iterator begin() const {
        return dynamic_codepoint_iterator(get()->begin());
    }

    dynamic_codepoint_iterator end() const {
        return dynamic_codepoint_iterator(get()->end());
    }

    size_t length() const {
        size_t length = 0;
        for(dynamic_codepoint_iterator it = begin(); it != end(); ++it) {
            ++length;
        }
        return length;
    }

    bool operator ==(const dynamic_unicode_string& other) const {
        if(length() != other.length()) {
            return false;
        } else {
            return std::equal(begin(), end(), other.begin());
        }
    }

    dynamic_unicode_string& operator =(const dynamic_unicode_string& other) {
        _str = other._str;
        return *this;
    }

    dynamic_unicode_string_object* get() const {
        return _str.get();
    }

  private:
    boost::shared_ptr<dynamic_unicode_string_object> _str;
};

template <
    typename StringT, 
    typename StringTraits, 
    typename EncoderTraits, 
    typename Policy>
class dynamic_unicode_string_impl : public dynamic_unicode_string_object {
  public:
    typedef unicode_string_adapter<
        StringT, StringTraits, EncoderTraits, Policy>   const_adapter_type;
    typedef typename
        const_adapter_type::codepoint_iterator_type     codepoint_iterator_type;

    dynamic_unicode_string_impl(const const_adapter_type& str) 
        : _str(str) 
    { }

    class dynamic_codepoint_iterator_impl 
        : public dynamic_codepoint_iterator_object 
    {
      public:
        dynamic_codepoint_iterator_impl(codepoint_iterator_type it)
            : _it(it)
        { }

        virtual const codepoint_type dereference() const {
            return *_it;
        }

        virtual void increment() const {
            ++_it;
        }

        virtual void decrement() const {
            --_it;
        }

        virtual bool equals(const dynamic_codepoint_iterator_object* other) const {
            if(get_type() == other->get_type()) {
                return _it == *(reinterpret_cast<codepoint_iterator_type*>(other->get_raw_iterator()));
            } else {
                return false;
            }
        }

        virtual void* get_raw_iterator() const {
            return const_cast<codepoint_iterator_type*>(&_it);
        }

        virtual const unicode_string_type& get_type() const {
            return unicode_string_type_identifier<
                StringT, StringTraits, EncoderTraits, Policy>::type;
        }

        virtual ~dynamic_codepoint_iterator_impl() { }
      private:
        codepoint_iterator_type _it;
    };

    virtual dynamic_codepoint_iterator_object* begin() const {
        return new dynamic_codepoint_iterator_impl(_str.begin());
    }

    virtual dynamic_codepoint_iterator_object* end() const {
        return new dynamic_codepoint_iterator_impl(_str.end());
    }

    virtual const unicode_string_type& get_type() const {
        return unicode_string_type_identifier<
            StringT, StringTraits, EncoderTraits, Policy>::type;
    }

    virtual ~dynamic_unicode_string_impl() { }

  private:
    const_adapter_type  _str;
};

template <
    typename StringT, typename StringTraits,
    typename EncoderTraits, typename Policy>
class dynamic_unicode_string_adapter_type<
    unicode_string_adapter<
        StringT, StringTraits,
        EncoderTraits, Policy
    > > 
{
  public:
    typedef dynamic_unicode_string_impl<
        StringT, StringTraits, EncoderTraits, Policy>       type;
};

} // namespace ustr
} // namespace boost
