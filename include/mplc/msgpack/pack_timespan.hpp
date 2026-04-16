#pragma once
#include "pack_base.hpp"
#include <mplc/time_span.h>

template<>
struct msgpack::adaptor::convert<mplc::TimeSpan> {
    msgpack::object const& operator()(msgpack::object const& o, TimeSpan& v) const {
        switch (o.type) {
        case type::EXT:
            switch ((ExtType)o.via.ext.type()) {
            case ExtType::TimeSpan:
                v = *reinterpret_cast<const int64_t*>(o.via.ext.data());
                break;
            /*case ExtType::OpcUa_Variant: {
                OpcUa_VariantHlp tmp;
                o.convert(tmp);
                int64_t tiks;
                tmp.GetInt64(tiks);
                v = tiks;
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
struct msgpack::adaptor::pack<mplc::TimeSpan> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, TimeSpan v) const {
        const auto ft = v.ticks();
        const size_t size = sizeof(ft);
        o.pack_ext(size, msgpack::type_id_v<TimeSpan>());
        o.pack_ext_body(reinterpret_cast<const char*>(&ft), size);
        return o;
    }
};

template<>
struct msgpack::adaptor::object_with_zone<mplc::TimeSpan> {
    void operator()(msgpack::object::with_zone& o, TimeSpan v) const {
        o.type = type::EXT;
        const auto ft = v.ticks();
        const size_t size = sizeof(ft);
        char* data = static_cast<char*>(o.zone.allocate_no_align(1 + size));
        data[0] = msgpack::type_id_v<TimeSpan>();
        memcpy(data + 1, &ft, size);
        o.via.ext.ptr = data;
        o.via.ext.size = size;
    }
};
