#pragma once
#include <share/mplcshare.h>
#include <mplc/lua/bind/bind_timespan.hpp>
#include <mplc/lua/lua_parser.h>
#include <mplc/lua/lua_table.h>
#include <mplc/lua/visitor/create_msgpack_visitor.h>
#include <mplc/msgpack/visitor/lua_write_visitor.h>
#include "bind_base.hpp"
#include "bind_string.hpp"

namespace mplc { namespace glob_data {
    struct IData;
    MPLCSHARE_API void gdata_to_msgpack(IData* ref, msgpack::object::with_zone& o);
}}  // namespace mplc::glob_data

namespace mplc { namespace lua {

    namespace tag {
        class NoCopy {};
    }  // namespace tag

    namespace _detail {
        template<class Tag>
        OpcUa_StatusCode get_mp_string(lua_State* L, msgpack::object_str& str, msgpack::zone& z, int i = -1) {
            size_t len = 0;
            const char* lua_str = lua_tolstring(L, i, &len);
            str.size = len;
            auto ptr = static_cast<char*>(z.allocate_no_align(str.size));
            memcpy(ptr, lua_str, len);
            str.ptr = ptr;
            return OpcUa_Good;
        }

        template<>
        inline OpcUa_StatusCode get_mp_string<tag::NoCopy>(lua_State* L,
                                                           msgpack::object_str& str,
                                                           msgpack::zone&,
                                                           int i) {
            size_t len = 0;
            str.ptr = lua_tolstring(L, i, &len);
            str.size = len;
            return OpcUa_Good;
        }
    }  // namespace _detail
    template<>
    struct Bind<msgpack::object::with_zone> {
        template<class Tag>
        static int push(lua_State* L, const msgpack::object& val, bool* ok = nullptr) {
            bool is_ok{true};
            switch (val.type) {
            case msgpack::type::NIL:
                lua_pushnil(L);
                break;
            case msgpack::type::BOOLEAN:
                lua_pushboolean(L, val.via.boolean);
                break;
            case msgpack::type::POSITIVE_INTEGER:
                lua_pushinteger(L, val.via.u64);
                break;
            case msgpack::type::NEGATIVE_INTEGER:
                lua_pushinteger(L, val.via.i64);
                break;
            case msgpack::type::FLOAT32:
            case msgpack::type::FLOAT64:
                lua_pushnumber(L, val.via.f64);
                break;
            case msgpack::type::STR:
                lua_pushlstring(L, val.via.str.ptr, val.via.str.size);
                break;
            case msgpack::type::BIN:
            case msgpack::type::ARRAY:
            case msgpack::type::MAP:
            case msgpack::type::EXT: {
                msgpack::lua_wirite_visitor write(L);
                msgpack::object_parser(val).parse(write);
                is_ok = write.is_ok();
                break;
            }
            default:;
            }
            if (ok)
                *ok = is_ok;
            return 1;
        }

        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, msgpack::object::with_zone& o, int idx = -1) {
            switch (lua_type(L, idx)) {
            case LUA_TNIL:
                o.type = msgpack::type::NIL;
                break;
            case LUA_TBOOLEAN:
                o.type = msgpack::type::BOOLEAN;
                o.via.boolean = lua_toboolean(L, idx);
                break;
            case LUA_TSTRING: {
                o.type = msgpack::type::STR;
                return _detail::get_mp_string<Tag>(L, o.via.str, o.zone, idx);
            }
            case LUA_TNUMBER: {
                if (lua_isinteger(L, idx)) {
                    o.via.i64 = lua_tointegerx(L, idx, nullptr);
                    o.type = o.via.i64 >= 0 ? msgpack::type::POSITIVE_INTEGER : msgpack::type::NEGATIVE_INTEGER;
                } else {
                    o.via.f64 = lua_tonumberx(L, idx, nullptr);
                    o.type = msgpack::type::FLOAT64;
                }
                break;
            }
            case LUA_TUSERDATA:
            case LUA_TTABLE: {
                detail::create_msgpack_visitor visitor(o, std::is_same<tag::NoCopy, Tag>::value);
                lua::lua_parser(L).parse(visitor, idx);
                break;
            }
            case LUA_TLIGHTUSERDATA:
            case LUA_TFUNCTION:
            case LUA_TTHREAD:
            default:
                return OpcUa_BadTypeMismatch;
                // printf("type: %s", lua_typename(L, lua_type(L, idx)));
                // break;
            }
            return OpcUa_Good;
        }
    };

    template<>
    struct Bind<msgpack::object> {
        template<class Tag>
        static int push(lua_State* L, const msgpack::object& val, bool* ok = nullptr) {
            return Bind<msgpack::object::with_zone>::push<Tag>(L, val, ok);
        }
    };
    template<>
    struct Bind<msgpack::object_handle> {
        template<class Tag>
        static int push(lua_State* L, const msgpack::object_handle& oh, bool* ok = nullptr) {
            return Bind<msgpack::object>::push<Tag>(L, oh.get(), ok);
        }

        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, msgpack::object_handle& oh, int idx = -1) {
            auto type = lua_type(L, idx);
            if (type == LUA_TNIL) {
                oh.set(msgpack::object());
            } else if (type == LUA_TBOOLEAN) {
                oh.set(msgpack::object((bool)lua_toboolean(L, idx)));
            } else if (type == LUA_TNUMBER) {
                if (lua_isinteger(L, idx)) {
                    oh.set(msgpack::object{lua_tointegerx(L, idx, nullptr)});
                } else {
                    oh.set(msgpack::object{lua_tonumberx(L, idx, nullptr)});
                }
            } else {
                if (!oh.zone()) {
                    oh.zone().reset(new msgpack::zone(256));
                }
                msgpack::object::with_zone oz(*oh.zone());
                lua::detail::create_msgpack_visitor vis(oz, std::is_same<tag::NoCopy, Tag>::value);
                lua::lua_parser(L).parse(vis, idx);
                oh.set(oz);
            }
            return OpcUa_Good;
        }
    };
    // template<class Tag, class T>
    // boost::enable_if_t<boost::is_same<rapidjson::Value, T>::type, OpcUa_StatusCode>  //
    //     get(lua_State* L, T& val, int idx = -1) {
    //     return Bind<json::Wrapper>::get<Tag>(L, json::wrap(val), idx);
    // }
    // template<class Tag, class T>
    // boost::enable_if_t<boost::is_same<rapidjson::Value, T>::type, int>  //
    //     set(lua_State* L, T& val, bool* ok = nullptr) {
    //     return Bind<json::Wrapper>::push<Tag>(L, json::wrap(val), ok);
    // }
}}  // namespace mplc::lua
