#pragma once
#include <boost/config.hpp>

#ifndef BOOST_NO_CXX17_STRUCTURED_BINDINGS
#    define MPLC_UNUSED [[maybe_unused]]
#elif defined(__GNUC__) || defined(__clang__)
#    define MPLC_UNUSED __attribute__((unused))
#else
#    define MPLC_UNUSED BOOST_ATTRIBUTE_UNUSED
#endif
