#pragma once

//#include <msgpack/object.hpp>
#include <mplc/msgpack_ext.hpp>
#include <boost/type_traits/is_same.hpp>
//#include <boost/type_traits/is_integral.hpp>
//#include <boost/type_traits/is_floating_point.hpp>

namespace mplc { namespace meta {
    template<class T, class = void>
    struct Serializer {
        static void call(const T& val, msgpack::object::with_zone& dst) {
            dst << val;
        }
    };
    template<>
    struct Serializer<const char*> {
        static void call(const char* val, msgpack::object::with_zone& dst) {
            dst.type = msgpack::type::STR;
            dst.via.str.ptr = val;
            dst.via.str.size = static_cast<uint32_t>(strlen(val));
        }
    };
    /*template<class T>
    struct Serializer<T, boost::enable_if_t<boost::is_integral<T>::value || boost::is_floating_point<T>::value>> {
        static void call(T val, msgpack::object::with_zone& dst) {
            dst << val;
        }
    };*/
    template<class T, class = void>
    struct Deserializer {
        static void call(T& dst, const msgpack::object& src) {
            static_assert(!boost::is_same<char*, T>::value, "Unavailable deserialize into <char*>");
            static_assert(!boost::is_same<unsigned char*, T>::value, "Unavailable deserialize into <unsigned char*>");
            src.convert(dst);
        }
    };

}}  // namespace mplc::meta
