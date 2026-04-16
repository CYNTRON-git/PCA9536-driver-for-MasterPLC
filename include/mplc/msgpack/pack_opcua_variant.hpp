#pragma once
#include <share/mplcshare.h>
#include <opcua.h>
#include <opcua/opcua_guid.h>
#include <stack>
#include "pack_base.hpp"
#include "pack_filetime.hpp"
#include "pack_rapidjson.hpp"

#define MP_WRITE(Type)                                                                                                 \
    case OpcUaType_##Type: {                                                                                           \
        ::detail::opc_type<OpcUa_##Type, OpcUaType_##Type>::template write<Stream>(variant.Value.Type, o);             \
    } break
#define MP_WRITE_REF(Type)                                                                                             \
    case OpcUaType_##Type: {                                                                                           \
        ::detail::opc_type<OpcUa_##Type, OpcUaType_##Type>::template write<Stream>(*variant.Value.Type, o);            \
    } break

namespace detail {

    template<class T, OpcUa_BuiltInType _OpcType>
    struct opc_type {
        template<class Stream>
        static void write(const T& val, msgpack::packer<Stream>& o) {
            o.pack(val);
        }
    };

    template<>
    struct opc_type<OpcUa_JsonElement, OpcUaType_JsonElement> {
        template<class Stream>
        static void write(const OpcUa_JsonElement& val, msgpack::packer<Stream>& o) {
            auto& json_v = mplc::OpcUa_Json::ref(val);
            std::stack<const Value*> stack;
            stack.push(&json_v.get());
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
                    o.pack_map(ref.MemberCount());
                    for (auto it = ref.MemberBegin(); it != ref.MemberEnd(); ++it) {
                        stack.push(&it->value);
                        stack.push(&it->name);
                    }
                    break;
                case rapidjson::kArrayType:
                    o.pack_array(ref.Size());
                    for (int i = ref.Size() - 1; i >= 0; --i) {
                        stack.push(&ref[i]);
                    }
                    // for (auto it = ref.Begin(); it != ref.End(); ++it) {
                    //     stack.push(&*it);
                    // }
                    break;
                default:;
                }
            }
        }
    };
    template<>
    struct opc_type<OpcUa_String, OpcUaType_String> {
        template<class Stream>
        static void write(const OpcUa_String& val, msgpack::packer<Stream>& o) {
            o.pack(mplc::lib::string_view(OpcUa_String_GetRawString(&val), OpcUa_StrLen(&val)));
        }
    };
    template<>
    struct opc_type<OpcUa_ByteString, OpcUaType_ByteString> {
        template<class Stream>
        static void write(const OpcUa_ByteString& val, msgpack::packer<Stream>& o) {
            o.pack_bin(val.Length);
            o.pack_bin_body((const char*)val.Data, val.Length);
        }
    };
    template<>
    struct opc_type<OpcUa_DataValue, OpcUaType_DataValue> {
        template<class Stream>
        static void write(const OpcUa_DataValue& val, msgpack::packer<Stream>& o) {
            o.pack_map(3);
            o.pack("Value");
            o.pack(val.Value);
            o.pack("SourceTimestamp");
            o.pack(val.SourceTimestamp);
            o.pack("StatusCode");
            o.pack(val.StatusCode);
        }
    };

}  // namespace detail

template<>
struct msgpack::adaptor::convert<OpcUa_DateTime> {
    msgpack::object const& operator()(msgpack::object const& o, OpcUa_DateTime& val) const {
        switch (o.type) {
        case type::EXT: {
            switch ((ExtType)o.via.ext.type()) {
            case ExtType::DateTime: {
                FileTime ft;
                o.convert(ft);
                val = ft;
                break;
            }
                /*case ExtType::OpcUa_Variant: {
                    OpcUa_VariantHlp tmp;
                    o.convert(tmp);
                    tmp.GetTime(val);
                    break;
                }*/

            case ExtType::None:
            default:
                OpcUa_DateTime_Clear(&val);
                break;
            }
            break;
        }
        case type::BOOLEAN:
            break;
        case type::POSITIVE_INTEGER:
        case type::NEGATIVE_INTEGER:
            val = OpcUa_DateTime_FromInt64(o.via.i64);
            break;
        case type::FLOAT32:
        case type::FLOAT64: {
            val = FileTime(o.via.f64);
            break;
        }
        case type::STR:
            break;
        case type::BIN:
            break;
        case type::ARRAY:
            // TODO: msgpack::adaptor::convert<OpcUa_VariantHlp> -> type::ARRAY
            break;
        case type::MAP:
            break;
        case type::NIL:
        default:
            OpcUa_DateTime_Clear(&val);
            break;
        }
        return o;
    }
};

// template<>
// struct msgpack::adaptor::object_with_zone<OpcUa_VariantHlp> {
//     void operator()(msgpack::object::with_zone& o, const OpcUa_VariantHlp& v) const {
//         uint32_t size = checked_get_container_size(v.size());
//         o.type = msgpack::type::STR;
//         char* ptr = static_cast<char*>(o.zone.allocate_align(size, MSGPACK_ZONE_ALIGNOF(char)));
//         o.via.str.ptr = ptr;
//         o.via.str.size = size;
//         std::memcpy(ptr, v.data(), v.size());
//     }
// };
template<>
struct msgpack::adaptor::pack<OpcUa_DateTime> {
    template<typename Stream>
    packer<Stream>& operator()(packer<Stream>& o, const OpcUa_DateTime& val) {
        FileTime ft = val;
        o.pack(ft);
        return o;
    }
};

template<>
struct msgpack::adaptor::convert<OpcUa_Variant> {
    msgpack::object const& operator()(msgpack::object const& o, OpcUa_Variant& variant) const {
        // if (o.type != msgpack::type::EXT || o.via.ext.type() !=
        // msgpack::type_id_v<OpcUa_Variant>())
        //     throw msgpack::type_error();

        OpcUa_Variant_Clear(&variant);
        switch (o.type) {
        case type::EXT: {
            switch ((ExtType)o.via.ext.type()) {
            case ExtType::DateTime: {
                FileTime ft;
                o.convert(ft);
                variant.Datatype = OpcUaType_DateTime;
                variant.Value.DateTime = ft;
                break;
            }
            case ExtType::None:
            default:
                OpcUa_Variant_Clear(&variant);
                break;
            }
            break;
        }
        case type::BOOLEAN:
            variant.Datatype = OpcUaType_Boolean;
            variant.Value.Boolean = o.via.boolean;
            break;
        case type::POSITIVE_INTEGER:
            variant.Datatype = OpcUaType_UInt64;
            variant.Value.UInt64 = o.via.u64;
            break;
        case type::NEGATIVE_INTEGER:
            variant.Datatype = OpcUaType_Int64;
            variant.Value.Int64 = o.via.i64;
            break;
        case type::FLOAT32:
            variant.Datatype = OpcUaType_Float;
            variant.Value.Float = o.via.f64;
            break;
        case type::FLOAT64:
            variant.Datatype = OpcUaType_Double;
            variant.Value.Double = o.via.f64;
            break;
        case type::STR:
            variant.Datatype = OpcUaType_String;
            OpcUa_String_Initialize(&variant.Value.String);
            OpcUa_String_AttachToString((char*)o.via.str.ptr,
                                        o.via.str.size,
                                        0,
                                        OpcUa_True,
                                        OpcUa_True,
                                        &variant.Value.String);
            break;
        case type::BIN: {
            variant.Datatype = OpcUaType_ByteString;
        }
            OpcUa_ByteString_Initialize(&variant.Value.ByteString);
            variant.Value.ByteString.Length = o.via.bin.size;
            variant.Value.ByteString.Data = (OpcUa_Byte*)OpcUa_Alloc(o.via.bin.size);
            OpcUa_Memory_MemCpy(variant.Value.ByteString.Data,
                                o.via.bin.size * sizeof(char),
                                (void*)o.via.bin.ptr,
                                o.via.bin.size);
            break;
        case type::MAP: {
            while (o.via.map.size == 3) {
                auto time_pos = find_key("SourceTimestamp", o.via.map);
                if (time_pos == -1) {
                    break;
                }
                auto status_pos = find_key("StatusCode", o.via.map);
                if (status_pos == -1) {
                    break;
                }
                auto val_pos = find_key("Value", o.via.map);
                if (val_pos == -1) {
                    break;
                }
                variant.Datatype = OpcUaType_DataValue;
                variant.Value.DataValue = OpcUa_DataValue_AllocAndInit();
                o.via.map.ptr[val_pos].val.convert(variant.Value.DataValue->Value);
                o.via.map.ptr[time_pos].val.convert(variant.Value.DataValue->SourceTimestamp);
                o.via.map.ptr[status_pos].val.convert(variant.Value.DataValue->StatusCode);
                return o;
            }
            // break специально пропущен
        case type::ARRAY:
            variant.Datatype = OpcUaType_JsonElement;
            variant.Value.JsonElement = mplc::Cpp2CProxy<mplc::OpcUa_Json>::create();
            auto& json = mplc::OpcUa_Json::ref(variant.Value.JsonElement);
            json.from_msgpack(o);
            break;
        }
        case type::NIL:
        default:
            OpcUa_Variant_Clear(&variant);
            break;
        }
        return o;
    }
};
template<>
struct msgpack::adaptor::pack<OpcUa_Variant> {
    template<typename Stream>
    packer<Stream>& operator()(packer<Stream>& o, const OpcUa_Variant& variant) {
        if (variant.ArrayType == OpcUa_VariantArrayType_Scalar) {
            switch (variant.Datatype) {
            case OpcUaType_Null:
            case OpcUaType_Variant:
                o.pack_nil();
                break;
                MP_WRITE(Boolean);
                MP_WRITE(SByte);
                MP_WRITE(Byte);
                MP_WRITE(Int16);
                MP_WRITE(UInt16);
                MP_WRITE(Int32);
                MP_WRITE(UInt32);
                MP_WRITE(Int64);
                MP_WRITE(UInt64);
                MP_WRITE(Float);
                MP_WRITE(Double);
                MP_WRITE(String);
                MP_WRITE(DateTime);
                MP_WRITE(ByteString);
                // MP_WRITE(XmlElement);
                MP_WRITE(StatusCode);
                // MP_WRITE_REF(NodeId);
                // MP_WRITE_REF(Guid);
                // MP_WRITE_REF(ExpandedNodeId);
                // MP_WRITE_REF(QualifiedName);
                // MP_WRITE_REF(LocalizedText);
                // MP_WRITE_REF(ExtensionObject);
                MP_WRITE_REF(DataValue);
                MP_WRITE(JsonElement);
            default:
                OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "<-- Invalid var type %d\n", variant.Datatype);
                break;
            }
        } else {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING,
                        "<-- unsuppot  OpcUa_Variant non scalar type %d\n",
                        variant.ArrayType);
        }
        return o;
    }
};

template<>
struct msgpack::adaptor::object_with_zone<OpcUa_Variant> {
    void operator()(msgpack::object::with_zone& o, const OpcUa_Variant& v) const {
        if (v.ArrayType == OpcUa_VariantArrayType_Scalar) {
            switch ((_OpcUa_BuiltInType)v.Datatype) {
            case OpcUaType_Null:
                break;
            case OpcUaType_Boolean:
                o.type = type::BOOLEAN;
                o.via.boolean = v.Value.Boolean;
                break;
            case OpcUaType_SByte:
                o.type = v.Value.SByte >= 0 ? type::POSITIVE_INTEGER : type::NEGATIVE_INTEGER;
                o.via.i64 = v.Value.SByte;
                break;
            case OpcUaType_Int16:
                o.type = v.Value.Int16 >= 0 ? type::POSITIVE_INTEGER : type::NEGATIVE_INTEGER;
                o.via.i64 = v.Value.Int16;
                break;
            case OpcUaType_Int32:
                o.type = v.Value.Int32 >= 0 ? type::POSITIVE_INTEGER : type::NEGATIVE_INTEGER;
                o.via.i64 = v.Value.Int32;
                break;
            case OpcUaType_Int64:
                o.type = v.Value.Int64 >= 0 ? type::POSITIVE_INTEGER : type::NEGATIVE_INTEGER;
                o.via.i64 = v.Value.Int64;
                break;
            case OpcUaType_Byte:
                o.type = type::POSITIVE_INTEGER;
                o.via.i64 = v.Value.Byte;
                break;
            case OpcUaType_UInt16:
                o.type = type::POSITIVE_INTEGER;
                o.via.i64 = v.Value.UInt16;
                break;
            case OpcUaType_UInt32:
                o.type = type::POSITIVE_INTEGER;
                o.via.i64 = v.Value.UInt32;
                break;
            case OpcUaType_UInt64:
                o.type = type::POSITIVE_INTEGER;
                o.via.i64 = v.Value.UInt64;
                break;
            case OpcUaType_Float:
                o.type = type::FLOAT32;
                o.via.f64 = v.Value.Float;
                break;
            case OpcUaType_Double:
                o.type = type::FLOAT64;
                o.via.f64 = v.Value.Double;
                break;
            case OpcUaType_String: {
                auto len = OpcUa_String_StrSize(&v.Value.String);
                auto ptr = o.zone.allocate_no_align(len);
                o.type = type::STR;
                memcpy(ptr, OpcUa_String_GetRawString(&v.Value.String), len);
                o.via.str.ptr = reinterpret_cast<const char*>(ptr);
                o.via.str.size = len;
                break;
            }
            case OpcUaType_DateTime: {
                mplc::DateTime dt = v.Value.DateTime;
                o << dt;
                break;
            }
            case OpcUaType_DataValue: {
                // o.zone.allocate_align(sizeof(msgpack::object_kv) * 3);
                break;
            }
            case OpcUaType_Guid:
            case OpcUaType_ByteString:
            case OpcUaType_XmlElement:
            case OpcUaType_NodeId:
            case OpcUaType_ExpandedNodeId:
            case OpcUaType_StatusCode:
            case OpcUaType_QualifiedName:
            case OpcUaType_LocalizedText:
            case OpcUaType_ExtensionObject:
            case OpcUaType_Variant:
            case OpcUaType_DiagnosticInfo:
            case OpcUaType_JsonElement:
                break;
            default:;
            }
        } else {
            OpcUa_Trace(OPCUA_TRACE_LEVEL_WARNING, "<-- unsuppot  OpcUa_Variant non scalar type %d\n", v.ArrayType);
        }
    }
};

template<>
struct msgpack::adaptor::convert<OpcUa_VariantHlp> {
    msgpack::object const& operator()(msgpack::object const& o, OpcUa_VariantHlp& val) const {
        OpcUa_Variant& ref = val;
        o.convert(ref);
        return o;
    }
};

template<>
struct msgpack::adaptor::pack<OpcUa_VariantHlp> {
    template<typename Stream>
    packer<Stream>& operator()(packer<Stream>& o, const OpcUa_VariantHlp& val) {
        const OpcUa_Variant& ref = val;
        o.pack(ref);
        return o;
    }
};

template<>
struct msgpack::adaptor::object_with_zone<OpcUa_VariantHlp> {
    void operator()(msgpack::object::with_zone& o, const OpcUa_VariantHlp& v) const {
        o << static_cast<const OpcUa_Variant&>(v);
    }
};
