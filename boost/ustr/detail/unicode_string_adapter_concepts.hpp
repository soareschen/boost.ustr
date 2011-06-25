
//          Copyright Soares Chen Ruo Fei 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/type_traits.hpp>
#include <boost/concept_check.hpp>

template <
    typename StringT, 
    typename StringTraits, 
    typename EncodingTraits >
class unicode_string_adapter_concepts {
  private:
    BOOST_STATIC_ASSERT((::boost::is_same<StringT, typename StringTraits::string_type>::value));
};
