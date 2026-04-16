#pragma once
#include <stack>
#include <mplc/libs/containers.hpp>
#include <rapidjson/document.h>
#include "pack_base.hpp"
// --- mplc::lib::string_view

template<>
struct msgpack::adaptor::convert<mplc::OpcUa_Json> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::OpcUa_Json& val) const {
        val.from_msgpack(o);
        return o;
    }
};

template<>
struct msgpack::adaptor::pack<rapidjson::Value> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, const rapidjson::Value& v) const {
        std::stack<const Value*> stack;
        stack.push(&v);
        while (!stack.empty()) {
            const Value& ref = *stack.top();
            stack.pop();
            switch (ref.GetType()) {
            case rapidjson::kNullType:
                o.pack_nil();
                break;
            case rapidjson::kFalseType:
            case rapidjson::kTrueType:
                o.pack(ref.GetBool());
                break;
            case rapidjson::kStringType:
                o.pack(mplc::lib::string_view(ref.GetString(), ref.GetStringLength()));
                break;
            case rapidjson::kNumberType:
                if (ref.IsInt()) {
                    o.pack(ref.GetInt64());
                } else {
                    o.pack(ref.GetDouble());
                }
                break;
            case rapidjson::kObjectType:
                o.pack_array(ref.MemberCount());
                for (auto it = ref.MemberBegin(); it != ref.MemberEnd(); ++it) {
                    stack.push(&it->name);
                    stack.push(&it->value);
                }
                break;
            case rapidjson::kArrayType:
                o.pack_array(ref.Size());
                for (auto it = ref.Begin(); it != ref.End(); ++it) {
                    stack.push(&*it);
                }
                break;
            default:;
            }
        }
        return o;
    }
};

template<>
struct msgpack::adaptor::object_with_zone<rapidjson::Value> {
    void operator()(msgpack::object::with_zone& o, const rapidjson::Value& v) const {}
};
