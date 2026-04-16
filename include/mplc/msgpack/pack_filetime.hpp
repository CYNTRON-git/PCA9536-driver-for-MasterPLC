#pragma once
#include <msgpack/pack.hpp>
#include <share/filetime.h>
#include "pack_base.hpp"

template<>
struct msgpack::adaptor::convert<FileTime> {
    msgpack::object const& operator()(msgpack::object const& o, FileTime& v) const {
        DateTime ft;
        o.convert(ft);
        v = ft.ticks();
        return o;
    }
};

template<>
struct msgpack::adaptor::pack<FileTime> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, FileTime v) const {
        o.pack(DateTime(v.dt(), DateTime::tick));
        return o;
    }
};

template<>
struct msgpack::adaptor::object_with_zone<FileTime> {
    void operator()(msgpack::object::with_zone& o, FileTime v) const {
        o << DateTime(v.dt(), DateTime::tick);
    }
};
