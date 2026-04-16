#pragma once

#include <share/share_addin.h>
#include <mplc/msgpack/pack_datetime.hpp>
#include <mplc/msgpack/pack_timespan.hpp>

struct json_write_visitor /*: private msgpack::null_visitor */ {
    json_write_visitor(ResponseWriter& wr, GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::None)
        : writer(wr), flags_(flags) {}

    bool visit_nil() const {
        writer.Null();
        return true;
    }

    bool visit_boolean(bool v) const {
        writer.Bool(v);
        return true;
    }

    bool visit_positive_integer(uint64_t v) const {
        writer.Uint64(v);
        return true;
    }

    bool visit_negative_integer(int64_t v) const {
        writer.Int64(v);
        return true;
    }

    bool visit_float32(float v) const {
        writer.Double(v);
        return true;
    }

    bool visit_float64(double v) const {
        writer.Double(v);
        return true;
    }

    bool visit_str(const char* v, uint32_t size) const {
        writer.String(v, size);
        return true;
    }

    bool visit_bin(const char* v, uint32_t size) const {
        return false;
    }

    bool visit_ext(const char* v, uint32_t size) try {
        msgpack::object tmp;
        tmp.via.ext.ptr = v;
        tmp.via.ext.size = size;
        tmp.type = msgpack::type::EXT;
        msgpack::object_ext ext{size, v};
        switch (msgpack::ExtType(ext.type())) {
        case msgpack::ExtType::None:
            writer.Null();
            break;
        case msgpack::ExtType::DateTime: {
            auto dt = tmp.as<mplc::DateTime>();
            switch (flags_) {
            case GetValueFlags::ReadTimeAsLinuxTime:
                WriteDateValueDbl(dt.ticks(), writer);
                break;
            case GetValueFlags::None:
            default:
                writer.Int64(dt.ticks());
            }
        } break;
        case msgpack::ExtType::TimeSpan: {
            auto time_span = tmp.as<mplc::TimeSpan>();
            writer.Double(time_span.msec_f());
            break;
        }
        default:;
        }
        return true;
    } catch (std::exception& ex) {
        writer.Null();
        OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "visit_ext error %s", ex.what());
        return false;
    }

    bool start_array(uint32_t num_elements) {
        writer.StartArray();
        return true;
    }

    bool start_array_item() {
        return true;
    }

    bool end_array_item() {
        return true;
    }

    bool end_array() {
        writer.EndArray();
        return true;
    }

    bool start_map(uint32_t num_kv_pairs) {
        writer.StartObject();
        return true;
    }

    bool start_map_key() {
        m_is_key = true;
        return true;
    }

    bool end_map_key() {
        m_is_key = false;
        return true;
    }

    bool start_map_value() {
        return true;
    }

    bool end_map_value() {
        return true;
    }

    bool end_map() {
        writer.EndObject();
        return true;
    }

    bool is_ok() const {
        return m_is_ok;
    }

private:
    bool m_is_ok{true};
    bool m_is_key{};
    ResponseWriter& writer;
    GetValueFlags::GetValueFlagsEnum flags_;
};
