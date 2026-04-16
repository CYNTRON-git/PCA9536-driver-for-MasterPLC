#pragma once
#include "pack_base.hpp"
#include <mplc/api/ref_to.h>
#include "mplc/vm/object_id.h"

template<>
struct msgpack::adaptor::convert<mplc::api::RefTo> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::api::RefTo& v) const {
        switch (o.type) {
        case msgpack::type::EXT:
        case msgpack::type::NEGATIVE_INTEGER:
        case msgpack::type::POSITIVE_INTEGER:
            v.id = o.via.i64;
            break;
        case msgpack::type::STR: {
            if (o.via.str.size == 0) {
                break;
            }

            mplc::vm::ItemID tmp(mplc::lib::string_view(o.via.str.ptr, o.via.str.size));
            v.id = tmp.id;
            v.path.swap(tmp.path);
            if (!v.path.empty()) {
                v.raw.assign(o.via.str.ptr, o.via.str.size);
            }
            break;
        }
        default:;
        }
        return o;
    }
};

template<>
struct msgpack::adaptor::pack<mplc::api::RefTo> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, const mplc::api::RefTo& v) const {
        if (v.raw.empty()) {
            o.pack(v.id);
        } else {
            o.pack(v.raw);
        }
        return o;
    }
};

template<>
struct msgpack::adaptor::object_with_zone<mplc::api::RefTo> {
    void operator()(msgpack::object::with_zone& o, const mplc::api::RefTo& v) const {
        if (v.raw.empty()) {
            o.type = type::POSITIVE_INTEGER;
            o.via.i64 = v.id;
        } else {
            o.type = type::STR;
            o.via.str.size = v.raw.size();
            void* ptr;
            o.via.str.ptr = static_cast<const char*>(ptr = o.zone.allocate_no_align(o.via.str.size));
            std::memcpy(ptr, v.raw.c_str(), o.via.str.size);
        }
    }
};

template<>
struct msgpack::adaptor::convert<mplc::vm::ObjectID> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::vm::ObjectID& v) const {
        switch (o.type) {
        case msgpack::type::EXT:
        case msgpack::type::NEGATIVE_INTEGER:
        case msgpack::type::POSITIVE_INTEGER:
            v.id = o.via.i64;
            break;
        case msgpack::type::STR: {
            if (o.via.str.size == 0) {
                break;
            }
            v = mplc::vm::ObjectID(mplc::lib::string_view(o.via.str.ptr, o.via.str.size));
            break;
        }
        default:;
        }
        return o;
    }
};

template<>
struct msgpack::adaptor::pack<mplc::vm::ObjectID> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, const mplc::vm::ObjectID& v) const {
        if (v.path.empty()) {
            o.pack(v.id);
        } else {
            o.pack(v.to_string());
        }
        return o;
    }
};

template<>
struct msgpack::adaptor::object_with_zone<mplc::vm::ObjectID> {
    void operator()(msgpack::object::with_zone& o, const mplc::vm::ObjectID& v) const {
        if (v.path.empty()) {
            o.type = type::POSITIVE_INTEGER;
            o.via.i64 = v.id;
        } else {
            object_with_zone<std::string>{}(o, v.to_string());
        }
    }
};
