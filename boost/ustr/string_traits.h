#pragma once

#include <memory>
#include <utility>
#include <algorithm>
#include "codeunit_traits.h"

namespace boost {
namespace ustr {

template <typename StringT>
class string_traits { };


template <
    typename CharT, 
    typename CharTraits, 
    typename Alloc >
class string_traits< std::basic_string<CharT, CharTraits, Alloc> >
{
  public:
    typedef string_traits< std::basic_string<
        CharT, CharTraits, Alloc> >                 this_traits;

    typedef std::basic_string<
        CharT, CharTraits, Alloc>                   string_type;
    typedef CharT                                   codeunit_type;

    static const size_t codeunit_size = sizeof(codeunit_type);

    static const string_type empty_string;

    typedef string_type*                            raw_strptr_type;
    typedef const string_type*                      const_raw_strptr_type;
    typedef std::shared_ptr<const string_type>      const_strptr_type;
    typedef std::unique_ptr<string_type>            mutable_strptr_type;

    typedef typename
        string_type::const_iterator                 codeunit_iterator_type;

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
            return str1 == str2;
        }

    };

    struct raw_strptr {
      public:
        static bool equals(raw_strptr_type str1, raw_strptr_type str2) {
            if(str1 && str2) {
                return *str1 == *str2;
            } else if(!str1 && str2) {
                return empty_string == *str2;
            } else if(str1 && !str2) {
                return *str1 == empty_string;
            } else {
                // both are empty strings
                return true;
            }
        }


        static bool equals(raw_strptr_type str1, const string_type& str2) {
            if(str1) {
                return *str1 == str2;
            } else {
                return empty_string == str2;
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

        static bool equals(const const_strptr_type& str1, 
                           const const_strptr_type& str2)
        {
            if(str1 && str2) {
                return *str1 == *str2;
            } else if(!str1 && str2) {
                return empty_string == *str2;
            } else if(str1 && !str2) {
                return *str1 == empty_string;
            } else {
                // both are empty strings
                return true;
            }
        }
    };

    struct mutable_strptr {
        static raw_strptr_type release(mutable_strptr_type& str) {
            return str.release();
        }

        static raw_strptr_type release(mutable_strptr_type&& str) {
            return str.release();
        }

        static raw_strptr_type get(mutable_strptr_type& str) {
            return str.get();
        }

        static void append(mutable_strptr_type& str, codeunit_type codeunit) {
            check_and_initialize(str);
            str->append(1, codeunit);
        } 

        static void check_and_initialize(mutable_strptr_type& str) {
            if(!str) {
                str.reset(new_string());
            }
        }
    };
};

template <typename CharT, typename CharTraits, typename Alloc>
const typename 
string_traits< std::basic_string<CharT, CharTraits, Alloc> >::string_type
string_traits< std::basic_string<CharT, CharTraits, Alloc> >::empty_string = "";


template <
    typename CharT, 
    typename Alloc >
class string_traits< std::vector<CharT, Alloc> >
{
  public:
    typedef std::vector<CharT, Alloc>               string_type;
    
    typedef CharT                                   codeunit_type;

    static const size_t codeunit_size = sizeof(codeunit_type);

    static const string_type empty_string;

    typedef string_type*                            raw_strptr_type;
    typedef const string_type*                      const_raw_strptr_type;
    typedef std::shared_ptr<const string_type>      const_strptr_type;
    typedef std::unique_ptr<string_type>            mutable_strptr_type;

    typedef typename
        string_type::const_iterator                 char_iterator_type;
    typedef codeunit_iterator<
        codeunit_type, char_iterator_type>          codeunit_iterator_type;

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
                return std::equal(str1.begin(),
                    str1.end(),
                    str2.begin()
                );
            }
            
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
                return codeunit_iterator_type(str->begin());
            } else {
                return codeunit_iterator_type(empty_string.begin());
            }
        }

        static codeunit_iterator_type 
        codeunit_end(const const_strptr_type& str) {
            if(str) {
                return codeunit_iterator_type(str->begin());
            } else {
                return codeunit_iterator_type(empty_string.begin());
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

        static raw_strptr_type release(mutable_strptr_type&& str) {
            return str.release();
        }

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

template < typename CharT, typename Alloc >
const typename string_traits< std::vector<CharT, Alloc> >::string_type
string_traits< std::vector<CharT, Alloc> >::empty_string = {};



} // namespace ustr
} // namespace boost
