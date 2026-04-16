#pragma once
#include <share/config.h>
#include <msgpack/object.hpp>
#include <mplc/libs/containers.hpp>
#include <mplc/libs/string_view.hpp>
#include <boost/type_traits/enable_if.hpp>

namespace mplc { namespace meta { namespace lib {
    using mplc::lib::string_view;
    template<class T>
    using hash = boost::hash<T>;
    template<class K, class V, class H = hash<K>>
    using unordered_map = mplc::lib::unordered_map<K, V, H>;
    using boost::enable_if_t;
}}}  // namespace mplc::meta::lib
