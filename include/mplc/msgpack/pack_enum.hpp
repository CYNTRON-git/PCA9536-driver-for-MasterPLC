#pragma once
#include "pack_base.hpp"

template<class Enum>
struct msgpack::adaptor::convert<Enum, boost::enable_if_t<boost::is_enum<Enum>::value>> {
    msgpack::object const& operator()(msgpack::object const& o, Enum& v) const {
        if (o.type != msgpack::type::POSITIVE_INTEGER && o.type != msgpack::type::NEGATIVE_INTEGER)
            throw msgpack::type_error();
        v = (Enum)o.via.i64;
        return o;
    }
};

template<class Enum>
struct msgpack::adaptor::pack<Enum, boost::enable_if_t<boost::is_enum<Enum>::value>> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, Enum v) const {
        o.pack((int)v);
        return o;
    }
};

template<class Enum>
struct msgpack::adaptor::object_with_zone<Enum, boost::enable_if_t<boost::is_enum<Enum>::value>> {
    void operator()(msgpack::object::with_zone& o, Enum v) const {
        o.type = type::POSITIVE_INTEGER;
        o.via.i64 = v;
    }
};
