//
// MessagePack for C++ C++03/C++11 Adaptation
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V3_CPP_CONFIG_DECL_HPP
#define MSGPACK_V3_CPP_CONFIG_DECL_HPP

#include "msgpack/v2/cpp_config_decl.hpp"
#include "msgpack/cpp_version.hpp"
#if !defined(MSGPACK_NO_BOOST)
#include <boost/container/small_vector.hpp>
#else
#include <vector>
#endif

#if defined(MSGPACK_USE_CPP03)

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v3) {
/// @endcond

using v2::unique_ptr;

using v2::move;

using v2::enable_if;

using v2::integral_constant;

using v2::is_same;

using v2::underlying_type;

using v2::is_array;

using v2::remove_const;
using v2::remove_volatile;
using v2::remove_cv;

using v2::is_pointer;

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v3)
/// @endcond

}  // namespace msgpack


#else  // MSGPACK_USE_CPP03

namespace msgpack {
/// @cond
MSGPACK_API_VERSION_NAMESPACE(v3) {
/// @endcond

// unique_ptr
using v2::unique_ptr;
// using v2::make_unique; // since C++14
using v2::hash;

// utility
using v2::move;
using v2::swap;
using v2::enable_if;
using v2::is_same;
using v2::underlying_type;
using v2::is_array;

using v2::remove_const;
using v2::remove_volatile;
using v2::remove_cv;
using v2::is_pointer;

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v3)
/// @endcond
}  // namespace msgpack

#endif // MSGPACK_USE_CPP03

#if !defined(MSGPACK_NO_BOOST)
namespace msgpack { namespace lib {
template<class T>
using stack = boost::container::small_vector<T, 8>;
}}
#else
namespace msgpack { namespace lib {
template<class T>
using stack = std::vector<T>;
}}
#endif

#endif // MSGPACK_V3_CPP_CONFIG_DECL_HPP
