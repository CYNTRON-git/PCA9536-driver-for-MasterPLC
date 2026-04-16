#pragma once
#include <boost/preprocessor/variadic/size.hpp>
#include <boost/preprocessor/cat.hpp>
#define MPLC_PP_EMPTY_(...)
#define MPLC_PP_EXPAND_(x) x
#define MPLC_PP_CAT_N_(Prefix, ...) BOOST_PP_CAT(Prefix, BOOST_PP_VARIADIC_SIZE(__VA_ARGS__))
#define MPLC_PP_EXPAND_AS(Prefix, ...) MPLC_PP_EXPAND_(MPLC_PP_CAT_N_(Prefix, __VA_ARGS__)(__VA_ARGS__))
