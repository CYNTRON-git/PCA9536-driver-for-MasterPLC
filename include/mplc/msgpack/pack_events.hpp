#pragma once

#include "pack_base.hpp"

#include "mplc/msgpack/pack_opcua_variant.hpp"
#include "mplc/msgpack/pack_filetime.hpp"
#include "mplc/msgpack/pack_datetime.hpp"

#include "events/events_archive_rec.h"
#include "events/event_instance_def.h"

template<>
struct msgpack::adaptor::convert<mplc::events::EventsArchiveRec> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::events::EventsArchiveRec& event) {
        if (o.type != msgpack::type::ARRAY) {
            throw msgpack::type_error();
        }
        const auto& array = o.via.array;
        {
            FileTime ft;
            array.ptr[0].convert(ft);
            event.Time = ft;
            array.ptr[1].convert(ft);
            event.ActiveTime = ft;
            array.ptr[2].convert(ft);
            event.InActiveTime = ft;
            array.ptr[3].convert(ft);
            event.AckedTime = ft;
        }
        event.RecId = array.ptr[4].via.i64;
        event.Severity = array.ptr[5].via.i64;
        event.UpdateType = (mplc::events::EventsArchiveRec::update_type)array.ptr[6].via.i64;
        event.Active = array.ptr[7].via.boolean;
        event.Acked = array.ptr[8].via.boolean;
        std::string message;
        array.ptr[9].convert(message);
        event.Message = message;

        std::string comment;
        array.ptr[10].convert(comment);
        event.Comment = comment;

        std::string client_addr;
        array.ptr[11].convert(client_addr);
        event.ClientAddress = client_addr;

        std::string client_usr_id;
        array.ptr[12].convert(client_usr_id);
        event.ClientUserId = client_usr_id;

        event.CopyArchiveFields = array.ptr[13].via.boolean;

        auto& fields = event.OtherFields;
        std::string name;
        OpcUa_VariantHlp value;

        for (auto i = 0; i < array.ptr[14].via.array.size; i++) {
            array.ptr[14].via.array.ptr[i].convert(name);
            array.ptr[15].via.array.ptr[i].convert(value);
            fields[name] = value;
        }
        return o;
    }
};

template<>
struct msgpack::adaptor::pack<mplc::events::EventsArchiveRec> {
    template<typename Stream>
    packer<Stream>& operator()(packer<Stream>& o, const mplc::events::EventsArchiveRec& event) const {
        std::vector<std::string> names;
        std::vector<OpcUa_VariantHlp> values;

        for (auto [name, value]: event.OtherFields) {
            names.emplace_back(name);
            values.emplace_back(value);
        }

        o.pack_array(16);
        o.pack(event.Time);
        o.pack(event.ActiveTime);
        o.pack(event.InActiveTime);
        o.pack(event.AckedTime);
        o.pack(event.RecId);
        o.pack(event.Severity);
        o.pack((int)event.UpdateType);
        o.pack(event.Active);
        o.pack(event.Acked);
        o.pack(event.Message);
        o.pack(event.Comment);
        o.pack(event.ClientAddress);
        o.pack(event.ClientUserId);
        o.pack(event.CopyArchiveFields);
        o.pack(names);
        o.pack(values);
        return o;
    }
};
template<>
struct msgpack::adaptor::convert<mplc::vm::ItemID> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::object const& o, mplc::vm::ItemID& itemId) {
        if (o.type != msgpack::type::ARRAY) {
            throw msgpack::type_error();
        }
        const auto& array = o.via.array;

        itemId.id = array.ptr[0].via.i64;
        itemId.path = { array.ptr[1].via.str.ptr, array.ptr[1].via.str.size};
    }
};
