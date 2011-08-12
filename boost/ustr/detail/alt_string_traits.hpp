
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
#include <iterator>
#include <boost/ustr/detail/util.hpp>
#include <boost/ustr/detail/incl.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/type_traits.hpp>

namespace boost {
namespace ustr {

template <
    typename StringT,
    typename CharT = typename char_type<StringT>::type
>
class alt_string_traits { 
  public:
    typedef string_traits<StringT>                  this_traits;
    typedef StringT                                 string_type;

    typedef CharT                                   codeunit_type;

    static const size_t codeunit_size = sizeof(codeunit_type);
    static const string_type empty_string;

    typedef string_type                             raw_strptr_type;
    typedef const string_type                       const_raw_strptr_type;

    typedef const string_type                       const_strptr_type;
    typedef string_type                             mutable_strptr_type;

    typedef typename
        string_type::const_iterator                 codeunit_iterator_type;
    typedef
        std::back_insert_iterator<string_type>      codeunit_output_iterator_type;

    typedef typename util::get_raw_char_type<
        codeunit_type>::type                        raw_char_type;
    typedef std::basic_string<raw_char_type>        raw_string_type;

    static raw_strptr_type new_string(const string_type& str) {
        return string_type(str);
    }

    static raw_strptr_type new_string() {
        return string_type();
    }

    static raw_strptr_type clone_string(raw_strptr_type str) {
        return string_type(str);
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
            return string_type(str);
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
            return string::equals(str1, str2);
        }

        static bool equals(raw_strptr_type str1, const string_type& str2) {
            return string::equals(str1, str2);
        }

        static void delete_string(raw_strptr_type str) {
            
        }
    };

    struct const_strptr {
        static const_raw_strptr_type get(const const_strptr_type& str) {
            return str;
        }

        static codeunit_iterator_type
        codeunit_begin(const const_strptr_type& str) {
            return str.begin();
        }

        static codeunit_iterator_type 
        codeunit_end(const const_strptr_type& str) {
            return str.end();
        }

        static bool equals(const_strptr_type str1, const_strptr_type str2) {
            return string::equals(str1, str2);
        }

        static void reset(const_strptr_type& str, raw_strptr_type new_str) {
            const_cast<string_type&>(str).assign(new_str);
        }
    };

    struct mutable_strptr {
        static raw_strptr_type release(mutable_strptr_type& str) {
            return str;
        }

        static raw_strptr_type get(mutable_strptr_type& str) {
            return str;
        }

        static void append(mutable_strptr_type& str, codeunit_type codeunit) {
            str.push_back(codeunit);
        }

        static void check_and_initialize(mutable_strptr_type& str) {

        }

        static codeunit_output_iterator_type output_iterator(mutable_strptr_type& str) {
            return std::back_inserter(str);
        } 
    };
};

template <typename StringT, typename CharT>
const StringT alt_string_traits< StringT, CharT >::empty_string;

} // namespace ustr
} // namespace boost
