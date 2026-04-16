#pragma once
#include "bind_base.hpp"
#include "bind_json.hpp"
#include "opcua_guid.h"
#include "opcua/bind_boolean.hpp"
#include "opcua/bind_byte_string.hpp"
#include "opcua/bind_data_value.hpp"
#include "opcua/bind_datetime.hpp"
#include "opcua/bind_expanded_node_id.hpp"
#include "opcua/bind_extension_object.hpp"
#include "opcua/bind_guid.hpp"
#include "opcua/bind_node_id.hpp"
#include "opcua/bind_opcua_string.hpp"
#include "opcua/bind_localized_text.hpp"
#include "opcua/bind_qualified_name.hpp"
#include "opcua/bind_xml_element.hpp"
#include "share/lua/global_data.h"

namespace mplc { namespace lua {

#define SET_OPCUA_VAL(Type)                                                                                            \
    case OpcUaType_##Type: {                                                                                           \
        switch (val.ArrayType) {                                                                                       \
        case OpcUa_VariantArrayType_Scalar:                                                                            \
            return lua::set<Tag, OpcUaType_##Type>(L, val.Value.Type, ok);                                             \
        case OpcUa_VariantArrayType_Array:                                                                             \
            return lua::set<Tag, OpcUaType_##Type>(L, val.Value.Array.Value.Type##Array, val.Value.Array.Length, ok);  \
        case OpcUa_VariantArrayType_Matrix:                                                                            \
        default:                                                                                                       \
            if (ok)                                                                                                    \
                *ok = false;                                                                                           \
            return 0;                                                                                                  \
        }                                                                                                              \
        break;                                                                                                         \
    }
#define SET_OPCUA_VAL_REF(Type)                                                                                        \
    case OpcUaType_##Type: {                                                                                           \
        switch (val.ArrayType) {                                                                                       \
        case OpcUa_VariantArrayType_Scalar:                                                                            \
            return lua::set<Tag, OpcUaType_##Type>(L, *val.Value.Type, ok);                                            \
        case OpcUa_VariantArrayType_Array:                                                                             \
            return lua::set<Tag, OpcUaType_##Type>(L, val.Value.Array.Value.Type##Array, val.Value.Array.Length, ok);  \
        case OpcUa_VariantArrayType_Matrix:                                                                            \
        default:                                                                                                       \
            if (ok)                                                                                                    \
                *ok = false;                                                                                           \
            return 0;                                                                                                  \
        }                                                                                                              \
        break;                                                                                                         \
    }

    template<>
    struct Bind<OpcUa_Variant> {
        template<class Tag>
        static int push(lua_State* L, const OpcUa_Variant& val, bool* ok = nullptr) {
            BOOST_ASSERT_MSG(val.ArrayType != OpcUa_VariantArrayType_Matrix, "Unsuported Type Matrix");
            switch (val.Datatype) {
                SET_OPCUA_VAL(SByte);
                SET_OPCUA_VAL(Byte);
                SET_OPCUA_VAL(Int16);
                SET_OPCUA_VAL(Int32);
                SET_OPCUA_VAL(UInt16);
                SET_OPCUA_VAL(UInt32);
                SET_OPCUA_VAL(Int64);
                SET_OPCUA_VAL(UInt64);
                SET_OPCUA_VAL(Float);
                SET_OPCUA_VAL(Double);
                SET_OPCUA_VAL(String);
                SET_OPCUA_VAL(DateTime);
                SET_OPCUA_VAL(StatusCode);
                SET_OPCUA_VAL(ByteString);
                SET_OPCUA_VAL_REF(DataValue);
                SET_OPCUA_VAL(Boolean); /*
                 SET_OPCUA_VAL(XmlElement);
                 SET_OPCUA_VAL_REF(Guid);
                 SET_OPCUA_VAL_REF(NodeId);
                 SET_OPCUA_VAL_REF(ExpandedNodeId);
                 SET_OPCUA_VAL_REF(QualifiedName);
                 SET_OPCUA_VAL_REF(LocalizedText);*/
            case OpcUaType_JsonElement: {
                const mplc::OpcUa_Json& json = mplc::OpcUa_Json::ref(val.Value.JsonElement);
                // mplc::print_json(json.get(), "set_lua");
                return set<Tag>(L, json.get(), ok);
            }
            case OpcUaType_Null:
            default:
                lua_pushnil(L);
                return 1;
            }
        }
#undef SET_OPCUA_VAL
#define GET_OPCUA_VAL(Type)                                                                                            \
    case OpcUaType_##Type: {                                                                                           \
        switch (arr_type) {                                                                                            \
        case OpcUa_VariantArrayType_Scalar:                                                                            \
            lua::get<Tag, OpcUaType_##Type>(L, val.Value.Type, idx);                                                   \
            break;                                                                                                     \
        case OpcUa_VariantArrayType_Array:                                                                             \
            lua::get<Tag, OpcUaType_##Type>(L, val.Value.Array.Value.Type##Array, val.Value.Array.Length, idx);        \
            break;                                                                                                     \
        case OpcUa_VariantArrayType_Matrix:                                                                            \
        default:                                                                                                       \
            break;                                                                                                     \
        }                                                                                                              \
        break;                                                                                                         \
    }
#define GET_OPCUA_VAL(Type)                                                                                            \
    case OpcUaType_##Type: {                                                                                           \
        switch (arr_type) {                                                                                            \
        case OpcUa_VariantArrayType_Scalar:                                                                            \
            lua::get<Tag, OpcUaType_##Type>(L, val.Value.Type, idx);                                                   \
            break;                                                                                                     \
        case OpcUa_VariantArrayType_Array:                                                                             \
            lua::get<Tag, OpcUaType_##Type>(L, val.Value.Array.Value.Type##Array, val.Value.Array.Length, idx);        \
            break;                                                                                                     \
        case OpcUa_VariantArrayType_Matrix:                                                                            \
        default:                                                                                                       \
            break;                                                                                                     \
        }                                                                                                              \
        break;                                                                                                         \
    }
#define GET_OPCUA_VAL_REF(Type)                                                                                        \
    case OpcUaType_##Type: {                                                                                           \
        switch (arr_type) {                                                                                            \
        case OpcUa_VariantArrayType_Scalar:                                                                            \
            val.Value.Type = (OpcUa_##Type*)OpcUa_Alloc(sizeof(OpcUa_##Type));                                         \
            OpcUa_##Type##_Initialize(val.Value.Type);                                                                 \
            lua::get<Tag, OpcUaType_##Type>(L, val.Value.Type, idx);                                                   \
            break;                                                                                                     \
        case OpcUa_VariantArrayType_Array:                                                                             \
            lua::get<Tag, OpcUaType_##Type>(L, val.Value.Array.Value.Type##Array, val.Value.Array.Length, idx);        \
            break;                                                                                                     \
        case OpcUa_VariantArrayType_Matrix:                                                                            \
        default:                                                                                                       \
            break;                                                                                                     \
        }                                                                                                              \
        break;                                                                                                         \
    }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_Variant& val, int idx = -1) {
            BOOST_ASSERT_MSG(val.ArrayType != OpcUa_VariantArrayType_Matrix, "Unsuported Type Matrix");
            OpcUa_Variant_Clear(&val);
            auto type = val.Datatype;
            auto arr_type = val.ArrayType;
            switch (type) {
                GET_OPCUA_VAL(Boolean);
                GET_OPCUA_VAL(SByte);
                GET_OPCUA_VAL(Byte);
                GET_OPCUA_VAL(Int16);
                GET_OPCUA_VAL(Int32);
                GET_OPCUA_VAL(UInt16);
                GET_OPCUA_VAL(UInt32);
                GET_OPCUA_VAL(Int64);
                GET_OPCUA_VAL(UInt64);
                GET_OPCUA_VAL(Float);
                GET_OPCUA_VAL(Double);
                GET_OPCUA_VAL(String);
                GET_OPCUA_VAL(DateTime);
                GET_OPCUA_VAL(StatusCode);
                GET_OPCUA_VAL(ByteString);
                GET_OPCUA_VAL_REF(DataValue);
                GET_OPCUA_VAL(XmlElement);
                // GET_OPCUA_VAL_REF(Guid);
                // GET_OPCUA_VAL_REF(NodeId);
                // GET_OPCUA_VAL_REF(ExpandedNodeId);
                // GET_OPCUA_VAL_REF(QualifiedName);
                // GET_OPCUA_VAL_REF(LocalizedText);
                // GET_OPCUA_VAL_REF(ExtensionObject);
            case OpcUaType_JsonElement: {
                mplc::OpcUa_Json& json = mplc::OpcUa_Json::ref(val.Value.JsonElement);
                // mplc::print_json(json.get(), "set_lua");
                auto wrap = json::wrap(json.get(), nullptr, &json.GetAllocator());
                return lua::get<Tag>(L, wrap, idx);
            }
            case OpcUaType_Null:
                switch (lua_type(L, idx)) {
                case LUA_TNUMBER: {
                    if (lua_isinteger(L, idx)) {
                        val.Datatype = OpcUaType_Int64;
                        return lua::get<Tag, OpcUaType_Int64>(L, val.Value.Int64, idx);
                    } else {
                        val.Datatype = OpcUaType_Double;
                        return lua::get<Tag, OpcUaType_Double>(L, val.Value.Double, idx);
                    }
                }
                case LUA_TBOOLEAN:
                    val.Datatype = OpcUaType_Boolean;
                    return lua::get<Tag, OpcUaType_Boolean>(L, val.Value.Boolean, idx);
                case LUA_TSTRING:
                    val.Datatype = OpcUaType_String;
                    return lua::get<Tag, OpcUaType_String>(L, val.Value.String, idx);
                case LUA_TTABLE: {
                    val.Datatype = OpcUaType_JsonElement;
                    val.Value.JsonElement = mplc::Cpp2CProxy<mplc::OpcUa_Json>::create();
                    mplc::OpcUa_Json& json = mplc::OpcUa_Json::ref(val.Value.JsonElement);
                    auto wrap = json::wrap(json.get(), nullptr, &json.GetAllocator());
                    return lua::get<Tag>(L, wrap, idx);
                }
                case LUA_TNIL:
                    break;
                case LUA_TUSERDATA: {
                    void* ud = luaL_testudata(L, idx, FileTimeName);
                    if (ud) {
                        val.Datatype = OpcUaType_DateTime;
                        val.Value.DateTime.dwLowDateTime = static_cast<FILETIME*>(ud)->dwLowDateTime;
                        val.Value.DateTime.dwHighDateTime = static_cast<FILETIME*>(ud)->dwHighDateTime;
                        return OpcUa_Good;
                    }
                    ud = luaL_testudata(L, idx, glob_data::TableData::name());
                    if (ud) {
                        glob_data::TableData* obj = *static_cast<mplc::glob_data::TableData**>(ud);

                        if (type == OpcUaType_DataValue) {
                            OpcUa_VariantHlp tmp;
                            OpcUa_StatusCode code = obj->to_data_value(tmp);
                            tmp.Swap(val);
                            return code;
                        } else {
                            OpcUa_VariantHlp tmp;
                            OpcUa_StatusCode code = obj->to_variant(tmp);
                            tmp.Swap(val);
                            return code;
                        }
                    } else {
                        lua_getfield(L, idx, "__clone");
                        if (!lua_isfunction(L, -1)) {
                            lua_pop(L, 1);
                            return OpcUa_BadTypeMismatch;
                        }
                        lua_pushvalue(L, -2);
                        if (lua_pcall(L, 1, 1, 0) != 0) {
                            OpcUa_CheckErrorText(OpcUa_BadInternalError,
                                                 "Error running function `__clone': %s",
                                                 lua_tostring(L, -1));
                            lua_pop(L, 1);
                            return OpcUa_BadInternalError;
                        }
                        mplc::lua::get(L, mplc::OpcUa_Json::ref(val.Value.JsonElement));
                        return OpcUa_Good;
                    }
                    //return OpcUa_BadInternalError;
                }
                default:
                    break;
                }
                // lua::get<Tag, detail::type_wrap<OpcUa_Variant, OpcUaType_Null>>(L, val, idx);
                break;
            default:
                break;
            }
            return OpcUa_Good;
        }
    };
    template<>
    struct Bind<OpcUa_VariantHlp> {
        template<class Tag>
        static int push(lua_State* L, const OpcUa_VariantHlp& val, bool* ok = nullptr) {
            return Bind<OpcUa_Variant>::template push<Tag>(L, val, ok);
        }

        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_VariantHlp& val, int idx = -1) {
            return Bind<OpcUa_Variant>::template get<Tag>(L, val, idx);
        }
    };
#undef GET_OPCUA_VAL
#undef GET_OPCUA_VAL_REF

    // OpcUa_StatusCode set_lua_value(const OpcUa_VariantHlp& v, lua_State* L, const OpcUa_BuiltInTypeHlp& type) {
    //     if (type.Type == OpcUaType_Null || type.Type == OpcUaType_Variant) {
    //         return set_lua_value(static_cast<const OpcUa_Variant>(v), L);
    //     }
    //     // if (v.GetType() != OpcUaType_JsonElement && type.ArrayType != v.ArrayType)
    //     //     return OpcUa_BadTypeMismatch;

    //    switch (v.GetType()) {
    //    case OpcUaType_JsonElement: {
    //        const mplc::OpcUa_Json& json = mplc::OpcUa_Json::ref(v.Value.JsonElement);
    //        OpcUa_ReturnErrorIfBad(set_lua_value(json.get(), L, &type));
    //    } break;
    //    case OpcUaType_ExtensionObject: {
    //        int len = 0;
    //        OpcUa_Byte* pOpcUaExtObjectValue = v.Value.ExtensionObject->Body.Binary.Data;
    //        len = CopyOpcUaBinaryObjectToLua(pOpcUaExtObjectValue, L, type);
    //    } break;
    //    default: {
    //        if (type.Type == v.GetType()) {
    //            set_lua_value(static_cast<const OpcUa_Variant>(v), L);
    //        } else {
    //            OpcUa_VariantHlp tmp = v;
    //            OpcUa_ReturnErrorIfBad(tmp.ChangeType(type));
    //            return set_lua_value(static_cast<const OpcUa_Variant>(tmp), L);
    //        }
    //    }
    //    }
    //    return OpcUa_Good;
    //}
}}  // namespace mplc::lua
