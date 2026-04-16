#pragma once
#include <boost/preprocessor/cat.hpp>

#if defined(__COUNTER__) && (__COUNTER__ + 1 == __COUNTER__ + 0)
#    define MPLC_PRIVATE_UNIQUE_ID __COUNTER__
#else
#    define MPLC_PRIVATE_UNIQUE_ID __LINE__
#endif

#define MPLC_PRIVATE_NAME(n) BOOST_PP_CAT(n, BOOST_PP_CAT(_mplc_uniq_, MPLC_PRIVATE_UNIQUE_ID))
