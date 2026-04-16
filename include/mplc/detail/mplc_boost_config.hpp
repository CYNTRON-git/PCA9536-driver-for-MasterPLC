#pragma once

//  boost/config/user.hpp  ---------------------------------------------------//

#ifndef BOOST_ALL_NO_LIB
#    define BOOST_ALL_NO_LIB
#endif

#define BOOST_WHATEVER_NO_LIB
#undef BOOST_PP_VARIADICS
#define BOOST_PP_VARIADICS 1

/**
 * \brief Disables Boost.Asio's deprecated interfaces and functionality.
 * See Networking TS Compatibility (https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/net_ts.html)
 * for a list of older interfaces that have been deprecated, and their replacements.
 */
#define BOOST_ASIO_NO_DEPRECATED

/**
 * \brief By default, Boost.Asio is a header-only library. However, some developers may prefer to build Boost.Asio using
 * separately compiled source code. To do this, add #include <boost/asio/impl/src.hpp> to one (and only one) source file
 * in a program, then build the program with BOOST_ASIO_SEPARATE_COMPILATION defined in the project/compiler settings.
 * Alternatively, BOOST_ASIO_DYN_LINK may be defined to build a separately-compiled Boost.Asio as part of a shared
 * library.
 */
#define BOOST_ASIO_SEPARATE_COMPILATION
#define BOOST_ASIO_DYN_LINK 1
#define BOOST_CHARCONV_DYN_LINK 1
#define BOOST_PROGRAM_OPTIONS_DYN_LINK 1
#define BOOST_DATE_TIME_NO_LIB
#define BOOST_REGEX_NO_LIB
//#define BOOST_THREAD_PROVIDES_FUTURE
#ifdef _WIN32_WCE
#    define BOOST_NO_STD_LOCALE
#    define BOOST_LEXICAL_CAST_ASSUME_C_LOCALE
#endif
#ifdef PLC_WINPAC_9000
#    pragma warning(disable : 4390)
#    define _InterlockedCompareExchange InterlockedCompareExchange
#    define _InterlockedExchangeAdd InterlockedExchangeAdd
#endif
