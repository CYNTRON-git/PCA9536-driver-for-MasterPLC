#pragma once
#include "bind_opcua_base.hpp"
#include <share/lua/global_data.h>
namespace mplc { namespace lua {

    template<>
    struct Bind<detail::type_wrap<OpcUa_Variant, OpcUaType_Null>> {
        template<class Tag>
        static int push(lua_State* L, const OpcUa_Boolean& val, bool* ok = nullptr) {
            lua_pushnil(L);
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_Variant& val, int idx = -1) {
            OpcUa_Variant_Clear(&val);
            switch (lua_type(L, idx)) {
            case LUA_TNIL:
                break;
            case LUA_TBOOLEAN:
                val.Datatype = OpcUaType_Boolean;
                lua::get<Tag, OpcUaType_Boolean>(L, val.Value.Boolean, idx);
                break;
            case LUA_TNUMBER: {
                if (lua_isinteger(L, idx)) {
                    val.Datatype = OpcUaType_Int64;
                    lua::get<Tag, OpcUaType_Int64>(L, val.Value.Int64, idx);
                } else {
                    val.Datatype = OpcUaType_Double;
                    lua::get<Tag, OpcUaType_Double>(L, val.Value.Double, idx);
                }
                break;
            }
            case LUA_TSTRING: {
                val.Datatype = OpcUaType_String;
                lua::get<Tag, OpcUaType_String>(L, val.Value.String, idx);
                break;
            }
            case LUA_TUSERDATA: {
                void* ud = nullptr;
                if ((ud = luaL_testudata(L, idx, FileTimeName))) {
                    val.Datatype = OpcUaType_DateTime;
                    lua::get<Tag, OpcUaType_DateTime>(L, val.Value.DateTime, idx);
                } else if ((ud = luaL_testudata(L, idx, glob_data::TableData::name()))) {
                    glob_data::TableData* obj = *static_cast<glob_data::TableData**>(ud);
                    OpcUa_VariantHlp tmp;
                    auto sc = obj->to_variant(tmp);
                    tmp.Swap(val);
                    return sc;
                } else {
                    lua_getfield(L, idx, "__clone");
                    if (!lua_isfunction(L, -1)) {
                        lua_pop(L, 1);
                        break;
                    }
                    if (idx > 0) {
                        lua_pushvalue(L, idx);
                    } else {
                        lua_pushvalue(L, idx - 1);
                    }
                    if (lua_pcall(L, 1, 1, 0) != 0) {
                        OpcUa_CheckErrorText(OpcUa_BadInternalError,
                                             "Error running function `__clone': %s",
                                             lua_tostring(L, -1));
                        lua_pop(L, 1);
                        break;
                    }
                    // TODO: Support Read TO JSON
                    // get_lua_value(v.GetJson(), L);

                    MPLC_NOT_IMPLEMENTED(false);
                    break;
                }
                break;
            }

            case LUA_TTABLE: {
                // TODO: Support Read TO JSON
                // get_lua_value(v.GetJson(), L);
            }

            default:
                return OpcUa_BadInternalError;
            }
            return OpcUa_Good;
        }
    };

}}  // namespace mplc::lua
