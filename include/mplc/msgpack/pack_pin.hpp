#pragma once
#include <mplc/msgpack/pack_opcua_variant.hpp>
#include "mplc/aggregation/pin.h"
template<>
struct msgpack::adaptor::convert<mplc::aggregation::Pin> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::aggregation::Pin& pin) const {
        if (o.type != msgpack::type::ARRAY) {
            throw msgpack::type_error();
        }
        const auto& array = o.via.array;
        OpcUa_VariantHlp variant;
        array.ptr[1].convert(variant);

        pin.time = array.ptr[0].via.i64;
        pin.value = variant;
        pin.quality = array.ptr[2].via.i64;

        return o;
    }
};
template<>
struct msgpack::adaptor::pack<mplc::aggregation::Pin> {
    template<typename Stream>
    packer<Stream>& operator()(packer<Stream>& o, const mplc::aggregation::Pin& pin) const {
        o.pack_array(3);
        o.pack(pin.time.dt());
        o.pack(pin.value);
        o.pack(pin.quality.sc());
        return o;
    }
};

template<>
struct msgpack::adaptor::convert<mplc::aggregation::Pin::ptr> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::aggregation::Pin::ptr& pin_ptr) const {
        if (pin_ptr == nullptr) {
            pin_ptr = mplc::aggregation::Pin::make();
        }
        if (o.type != msgpack::type::NIL) {
            o.convert(*pin_ptr);
        }
        return o;
    }
};

template<>
struct msgpack::adaptor::pack<mplc::aggregation::Pin::ptr> {
    template<typename Stream>
    packer<Stream>& operator()(packer<Stream>& o, const mplc::aggregation::Pin::ptr& pin_ptr) const {
        if (pin_ptr) {
            o.pack(*pin_ptr);
        } else {
            o.pack_nil();
        }
        return o;
    }
};
