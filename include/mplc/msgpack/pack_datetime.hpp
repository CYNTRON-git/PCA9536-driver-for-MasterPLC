#pragma once
#include "pack_base.hpp"
#include <msgpack/pack.hpp>
#include <mplc/date_time.h>
template<>
struct msgpack::adaptor::convert<mplc::DateTime> {
    msgpack::object const& operator()(msgpack::object const& o, DateTime& v) const {
        switch (o.type) {
        case type::EXT:
            switch ((ExtType)o.via.ext.type()) {
            case ExtType::DateTime:
                v = *reinterpret_cast<const int64_t*>(o.via.ext.data());
                break;
            /*case ExtType::OpcUa_Variant: {
                OpcUa_VariantHlp tmp;
                o.convert(tmp);
                OpcUa_DateTime dt;
                tmp.GetTime(dt);
                v = dt;
                break;
            }*/
            case ExtType::None:
            default:;
            }
            break;
        case type::NEGATIVE_INTEGER:
        case type::POSITIVE_INTEGER:
            v = o.via.i64;
            break;
        case type::FLOAT32:
        case type::FLOAT64:
            v = o.via.f64;
            break;
        default:;
        }
        return o;
    }
};

template<>
struct msgpack::adaptor::pack<mplc::DateTime> {
    template<typename Stream>
    packer<Stream>& operator()(packer<Stream>& o, DateTime v) const {
        const auto ft = v.ticks();
        const size_t size = sizeof(ft);
        o.pack_ext(size, msgpack::type_id_v<DateTime>());
        o.pack_ext_body(reinterpret_cast<const char*>(&ft), size);
        return o;
    }
};

template<>
struct msgpack::adaptor::object_with_zone<mplc::DateTime> {
    void operator()(msgpack::object::with_zone& o, DateTime v) const {
        o.type = type::EXT;
        const auto ft = v.ticks();
        const size_t size = sizeof(ft);
        char* data = static_cast<char*>(o.zone.allocate_no_align(1 + size));
        data[0] = msgpack::type_id_v<DateTime>();
        memcpy(data + 1, &ft, size);
        o.via.ext.ptr = data;
        o.via.ext.size = size;
    }
};
