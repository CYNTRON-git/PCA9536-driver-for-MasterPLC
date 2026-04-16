#pragma once
#include "pack_base.hpp"

// --- mplc::lib::unordered_map
template<class K, class V>
struct msgpack::adaptor::convert<mplc::lib::unordered_map<K, V>> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::lib::unordered_map<K, V>& v) const {
        if (o.type != msgpack::type::MAP)
            throw msgpack::type_error();
        for (const msgpack::object_kv* it = msgpack::begin(o.via.map); it != msgpack::end(o.via.map); ++it) {
            v[it->key.as<K>()] = it->val.as<V>();
        }
        return o;
    }
};

template<class K, class V>
struct msgpack::adaptor::pack<mplc::lib::unordered_map<K, V>> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, const mplc::lib::unordered_map<K, V>& v) const {
        o.pack_map(v.size());
        for (auto it = v.begin(); it != v.end(); ++it) {
            o.pack(it->first);
            o.pack(it->second);
        }
        return o;
    }
};

template<class K, class V>
struct msgpack::adaptor::object_with_zone<mplc::lib::unordered_map<K, V>> {
    void operator()(msgpack::object::with_zone& o, const mplc::lib::unordered_map<K, V>& v) const {
        o.type = type::MAP;
        o.via.map.size = v.size();
        o.via.map.ptr = static_cast<msgpack::object_kv*>(
            o.zone.allocate_align(sizeof(msgpack::object_kv) * o.via.map.size,
                                  MSGPACK_ZONE_ALIGNOF(msgpack::object_kv)));
        size_t pos = 0;
        for (auto it = v.begin(); it != v.end(); ++it) {
            o.via.map.ptr[pos].key = msgpack::object(it->first, o.zone);
            o.via.map.ptr[pos].val = msgpack::object(it->second, o.zone);
        }
    }
};
