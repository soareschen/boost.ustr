
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
#include <boost/ustr/codeunit_traits.hpp>

namespace boost {
namespace ustr {

template <typename StringT>
class char_type { };

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
    typedef boost::shared_ptr<const string_type>    const_strptr_type;
    typedef boost::shared_ptr<string_type>          mutable_strptr_type;
    // We've got to use shared_ptr when unique_ptr isn't available, because
    // a boost::scoped_ptr can't be returned under C++03 (since returning it
    // requires copying it, and it's non-copyable).

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
            raw_strptr_type r = str.get();
            str.reset();
            return r;
        }

        #ifdef BOOST_USTR_CPP0X
        static raw_strptr_type release(mutable_strptr_type&& str) {
            raw_strptr_type r = str.get();
            str.reset();
            return r;
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
            if(!str) {
                str.reset(new_string());
            }
        }
    };
};

template <typename StringT, typename CharT>
const StringT string_traits< StringT, CharT >::empty_string;

} // namespace ustr
} // namespace boost
