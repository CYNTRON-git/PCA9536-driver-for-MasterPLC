#pragma once
#include <mplc/libs/containers.hpp>
#include "pack_base.hpp"
// --- mplc::lib::string_view

template<>
struct msgpack::adaptor::convert<mplc::lib::string_view> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::lib::string_view& v) const {
        switch (o.type) {
        case type::NIL:
            v.clear();
            break;
        case type::STR:
            v = mplc::lib::string_view(o.via.str.ptr, o.via.str.size);
            break;
        case type::BIN:
            v = mplc::lib::string_view(o.via.bin.ptr, o.via.bin.size);
            break;
        case type::BOOLEAN:
        case type::POSITIVE_INTEGER:
        case type::NEGATIVE_INTEGER:
        case type::FLOAT32:
        case type::FLOAT64:
        case type::ARRAY:
        case type::MAP:
        case type::EXT:
        default:
            throw msgpack::type_error();
        }
        return o;
    }
};

template<>
struct msgpack::adaptor::pack<mplc::lib::string_view> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, const mplc::lib::string_view& v) const {
        o.pack_str(v.size());
        o.pack_str_body(v.data(), v.size());
        return o;
    }
};

template<>
struct msgpack::adaptor::object_with_zone<mplc::lib::string_view> {
    void operator()(msgpack::object::with_zone& o, const mplc::lib::string_view& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.type = msgpack::type::STR;
        char* ptr = static_cast<char*>(o.zone.allocate_align(size, MSGPACK_ZONE_ALIGNOF(char)));
        o.via.str.ptr = ptr;
        o.via.str.size = size;
        std::memcpy(ptr, v.data(), v.size());
    }
};
