#pragma once
#include "pack_base.hpp"
#include <mplc/lua/lua_context.hpp>
#include <mplc/lua/lua_bind.hpp>


template<>
struct msgpack::adaptor::convert<mplc::lua::LuaContext> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::lua::LuaContext& ctx) const {
        switch (o.type) {
        case type::NIL:
            ctx.pushNil();
            break;
        case type::BOOLEAN:
            ctx.push(o.via.boolean);
            break;
        case type::POSITIVE_INTEGER:
        case type::NEGATIVE_INTEGER:
            ctx.push(o.via.i64);
            break;
        case type::FLOAT32:
        case type::FLOAT64:
            ctx.push(o.via.f64);
            break;
        case type::STR:
            ctx.push(mplc::lib::string_view(o.via.str.ptr, o.via.str.size));
            break;
        case type::BIN: {
            OpcUa_VariantHlp tmp;
            tmp.SetByteString(o.via.bin.ptr, o.via.bin.size);
            ctx.push(tmp);
            break;
        }
        case type::ARRAY: {
            lua_createtable(ctx.L, o.via.array.size, 0);
            for (size_t i = 0; i < o.via.array.size; ++i) {
                lua_pushinteger(ctx.L, i + 1);
                operator()(o.via.array.ptr[i], ctx);
                lua_rawset(ctx.L, -3);
            }
        } break;
        case type::MAP:
            lua_createtable(ctx.L, 0, o.via.map.size);
            for (size_t i = 0; i < o.via.map.size; ++i) {
                operator()(o.via.map.ptr[i].key, ctx);
                operator()(o.via.map.ptr[i].val, ctx);
                lua_rawset(ctx.L, -3);
            }
            break;
        case type::EXT: {
            switch (static_cast<msgpack::ExtType>(o.via.ext.type())) {
            case ExtType::None:
                lua_pushnil(ctx.L);
                break;
            case ExtType::DateTime: {
                const FileTime* ft = reinterpret_cast<const FileTime*>(o.via.ext.data());
                ft->to_lua(ctx.L);
                break;
            }
            case ExtType::TimeSpan: {
                const TimeSpan* ts = reinterpret_cast<const TimeSpan*>(o.via.ext.data());
                lua_pushnumber(ctx.L, ts->msec_f());
                break;
            }
            /*case ExtType::OpcUa_Variant: {
                try {
                    OpcUa_Variant var;
                    o.convert(var);
                    bool ok;
                    mplc::lua::set(ctx.L, var, &ok);
                    if (!ok) {
                        lua_pushnil(ctx.L);
                    }
                } catch (...) {
                    lua_pushnil(ctx.L);
                }
                break;
            }*/
            default:;
            }
            break;
        }
        default:;
        }
        return o;
    }
};

template<>
struct msgpack::adaptor::pack<mplc::lua::LuaContext> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, mplc::lua::LuaContext const& ctx) const {
        lua_State* L = ctx.L;
        switch (lua_type(L, -1)) {
        case LUA_TNUMBER:
            if (lua_isinteger(L, -1)) {
                o.pack(lua_tointeger(L, -1));
            } else {
                o.pack(lua_tonumber(L, -1));
            }
            break;
        case LUA_TSTRING: {
            size_t len;
            const char* str = lua_tolstring(L, -1, &len);
            /* const size_t size_diff = cp1251_to_utf8_diff(str, len);
            if (size_diff) {
                char* windows1251 = new char[len + size_diff + 1];
                const size_t new_len = cp1251_to_utf8(windows1251, str, len);
                o.pack_str(new_len);
                o.pack_str_body(windows1251, new_len);
                delete[] windows1251;
                break;
            }*/
            o.pack_str(len);
            o.pack_str_body(str, len);
            break;
        }
        case LUA_TBOOLEAN: {
            o.pack((bool)lua_toboolean(L, -1));
            break;
        }
        case LUA_TUSERDATA: {
            // BoxType* ud = (BoxType*)luaL_checkudata(L, -1, "BoxType");
            //if (ud) {
            //    // ud->api->pack(ud->obj, o);
            //}
            break;
        }
        case LUA_TTABLE: {
            size_t arr_len = lua_rawlen(L, -1);
            size_t map_len = 0;
            if (arr_len == 0) {
                lua_pushnil(L);
                while (lua_next(L, -2)) {
                    ++map_len;
                    lua_pop(L, 1);
                }
                lua_pop(L, 1);
            }
            if (arr_len > 0 || map_len == 0) {
                o.pack_array(arr_len);
                for (size_t i = 0; i < arr_len; ++i) {
                    lua_rawgeti(L, -1, i + 1);
                    o.pack(ctx);
                    lua_pop(L, 1);
                }
            } else {
                o.pack_map(map_len);
                lua_pushnil(L);
                while (lua_next(L, -2)) {
                    size_t len;
                    auto str = lua_tolstring(L, -2, &len);
                    o.pack_str(len);
                    o.pack_str_body(str, len);
                    o.pack(ctx);
                    lua_pop(L, 1);
                }
            }
        }
        default: {
            static const mplc::lib::string_view err("Unsupported type");
            o.pack(err);
        }
        }
        return o;
    }
};
//
template<>
struct msgpack::adaptor::object_with_zone<mplc::lua::LuaContext> {
    void operator()(msgpack::object::with_zone& o, const mplc::lua::LuaContext& ctx) const {
        lua_State* L = ctx.L;
        switch (lua_type(L, -1)) {
        case LUA_TNUMBER:
            if (lua_isinteger(L, -1)) {
                o.via.i64 = lua_tointeger(L, -1);
                o.type = o.via.i64 >= 0 ? type::POSITIVE_INTEGER : type::NEGATIVE_INTEGER;
            } else {
                o.via.f64 = lua_tonumber(L, -1);
                o.type = type::FLOAT64;
            }
            break;
        case LUA_TSTRING: {
            size_t len;
            const char* str = lua_tolstring(L, -1, &len);
            /*const size_t size_diff = cp1251_to_utf8_diff(str, len);
            char* ptr;
            if (size_diff) {
                ptr = (char*)o.zone.allocate_no_align(len + size_diff);
                len = cp1251_to_utf8(ptr, str, len);
            } else {
                ptr = (char*)o.zone.allocate_no_align(len);
                memcpy(ptr, str, len);
            }*/
            char* ptr = (char*)o.zone.allocate_no_align(len);
            memcpy(ptr, str, len);
            o.via.str.ptr = ptr;
            o.via.str.size = len;
            o.type = type::STR;
            break;
        }
        case LUA_TBOOLEAN: {
            o.via.boolean = (bool)lua_toboolean(L, -1);
            o.type = type::BOOLEAN;
            break;
        }
        case LUA_TUSERDATA: {
            const void* ud = luaL_testudata(L, -1, FileTimeName);
            if (ud) {
                FileTime ft;
                ft.from_lua(L, -1);
                o << ft;
            }
            // BoxType* ud = (BoxType*)luaL_checkudata(L, -1, "BoxType");
            // if (ud) {
            //     // ud->api->pack(ud->obj, o);
            // }
            break;
        }
        case LUA_TTABLE: {
            size_t arr_len = lua_rawlen(L, -1);
            size_t map_len = 0;
            if (arr_len == 0) {
                int top = lua_gettop(L);
                lua_pushnil(L);
                while (lua_next(L, -2)) {
                    ++map_len;
                    lua_pop(L, 1);
                }
                lua_settop(L, top);
                // lua_pop(L, 2);
            }
            if (arr_len > 0 || map_len == 0) {
                o.type = type::ARRAY;
                o.via.array.size = arr_len;
                o.via.array.ptr = static_cast<msgpack::object*>(
                    o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size,
                                          MSGPACK_ZONE_ALIGNOF(msgpack::object)));
                for (size_t i = 0; i < arr_len; ++i) {
                    lua_rawgeti(L, -1, i + 1);
                    o.via.array.ptr[i] = msgpack::object(ctx, o.zone);
                    lua_pop(L, 1);
                }
            } else {
                o.type = type::MAP;
                o.via.map.size = map_len;
                o.via.map.ptr = static_cast<msgpack::object_kv*>(
                    o.zone.allocate_align(sizeof(msgpack::object_kv) * o.via.map.size,
                                          MSGPACK_ZONE_ALIGNOF(msgpack::object_kv)));
                size_t pos = 0;
                int top = lua_gettop(L);
                lua_pushnil(L);
                while (lua_next(L, -2)) {
                    o.via.map.ptr[pos].val = msgpack::object(ctx, o.zone);
                    lua_pop(L, 1);
                    o.via.map.ptr[pos].key = msgpack::object(ctx, o.zone);
                    ++pos;
                }
                lua_settop(L, top);
            }
        }
        default: {
            static const mplc::lib::string_view err("Unsupported type");
            // o.pack(err);
        }
        }
        return;
    }
};
