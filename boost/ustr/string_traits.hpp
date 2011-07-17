
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <utility>
#include <algorithm>
#include <boost/ustr/detail/codeunit_traits.hpp>
#include <boost/ustr/detail/incl.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/type_traits.hpp>

namespace boost {
namespace ustr {

template <typename StringT>
class char_type { 
  public:
    typedef typename
        StringT::value_type     type;
};

template < typename CharT, typename CharTraits, typename Alloc >
class char_type< std::basic_string<CharT, CharTraits, Alloc> > {
  public:
    typedef CharT   type;
};

template < typename CharT, typename Alloc >
class char_type< std::vector<CharT, Alloc> > {
  public:
    typedef CharT   type;
};

template < typename CharT, typename Alloc >
class char_type< std::list<CharT, Alloc> > {
  public:
    typedef CharT   type;
};

template <
    typename StringT,
    typename CharT = typename char_type<StringT>::type
>
class string_traits { 
  public:
    typedef string_traits<StringT>                  this_traits;
    typedef StringT                                 string_type;

    typedef CharT                                   codeunit_type;

    static const size_t codeunit_size = sizeof(codeunit_type);
    static const string_type empty_string;

    typedef string_type*                            raw_strptr_type;
    typedef const string_type*                      const_raw_strptr_type;


#ifdef BOOST_USTR_CPP0x
    typedef std::shared_ptr<const string_type>      const_strptr_type;
    typedef std::unique_ptr<string_type>            mutable_strptr_type;
#elif BOOST_HAS_TR1_SHARED_PTR
    typedef std::tr1::shared_ptr<const string_type> const_strptr_type;
    typedef std::auto_ptr<string_type>              mutable_strptr_type;
#else
    typedef boost::shared_ptr<const string_type>    const_strptr_type;
    typedef std::auto_ptr<string_type>              mutable_strptr_type;
#endif

    typedef typename
        string_type::const_iterator                 codeunit_iterator_type;

    typedef typename
        get_raw_char_type<codeunit_type>::type      raw_char_type;
    typedef std::basic_string<raw_char_type>        raw_string_type;

    static raw_strptr_type new_string(const string_type& str) {
        return new string_type(str);
    }

    static raw_strptr_type new_string() {
        return new string_type();
    }

    static raw_strptr_type clone_string(raw_strptr_type str) {
        return new string_type(*str);
    }

    struct string {
      public:
        static bool equals(const string_type& str1, const string_type& str2) {
            if(str1.size() != str2.size()) {
                return false;
            } else if(str1.empty() && str2.empty()) {
                return true;
            } else {
                return std::equal(
                    str1.begin(),
                    str1.end(),
                    str2.begin()
                );
            }
            
        }

        static string_type from_raw_string(const raw_string_type& str) {
            string_type new_str;
            std::copy(str.begin(), str.end(), std::back_inserter(new_str));
            return new_str;
        }

        template <typename Iterator>
        static string_type from_iter(Iterator begin, Iterator end) {
            string_type str;
            std::copy(begin, end, std::back_inserter(str));
            return str;
        }
    };

    struct raw_strptr {
      public:
        static bool equals(raw_strptr_type str1, raw_strptr_type str2) {
            if(str1 && str2) {
                return string::equals(*str1, *str2);
            } else if(!str1 && str2) {
                return string::equals(empty_string, *str2);
            } else if(str1 && !str2) {
                return string::equals(*str1, empty_string);
            } else {
                return true;
            }
        }

        static bool equals(raw_strptr_type str1, const string_type& str2) {
            if(str1) {
                return string::equals(*str1, str2);
            } else {
                return string::equals(empty_string, str2);
            }
        }

        static void delete_string(raw_strptr_type str) {
            delete str;
        }
    };

    struct const_strptr {
        static const_raw_strptr_type get(const const_strptr_type& str) {
            return str.get();
        }

        static codeunit_iterator_type
        codeunit_begin(const const_strptr_type& str) {
            if(str) {
                return str->begin();
            } else {
                return empty_string.begin();
            }
        }

        static codeunit_iterator_type 
        codeunit_end(const const_strptr_type& str) {
            if(str) {
                return str->end();
            } else {
                return empty_string.end();
            }

        }

        static bool equals(const_strptr_type str1, const_strptr_type str2) {
            if(str1 && str2) {
                return string::equals(*str1, *str2);
            } else if(!str1 && str2) {
                return string::equals(empty_string, *str2);
            } else if(str1 && !str2) {
                return string::equals(*str1, empty_string);
            } else {
                return true;
            }
        }
    };

    struct mutable_strptr {
        static raw_strptr_type release(mutable_strptr_type& str) {
            return str.release();
        }

#ifndef BOOST_NO_RVALUE_REFERENCES
        static raw_strptr_type release(mutable_strptr_type&& str) {
            return str.release();
        }
#endif

        static raw_strptr_type get(mutable_strptr_type& str) {
            return str.get();
        }

        static void append(mutable_strptr_type& str, codeunit_type codeunit) {
            check_and_initialize(str);
            str->push_back(codeunit);
        }

        static void check_and_initialize(mutable_strptr_type& str) {
            if(!str.get()) {
                str.reset(new_string());
            }
        }
    };
};

template <typename StringT, typename CharT>
const StringT string_traits< StringT, CharT >::empty_string;

} // namespace ustr
} // namespace boost
