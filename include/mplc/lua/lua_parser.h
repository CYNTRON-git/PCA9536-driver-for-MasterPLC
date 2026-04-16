#pragma once
#include <vector>
#include <lua.hpp>
#include "lua_table.h"
#include <mplc/libs/string_view.hpp>
#include <boost/container/small_vector.hpp>
namespace mplc { namespace lua {
    /*
    static thread_local char g_log_print_buf[16 * 1024];
    static thread_local char* g_log_print_pos = g_log_print_buf;
    inline int log_print(bool flush, const char* fmt, ...) {
        auto len = g_log_print_pos - g_log_print_buf;
        va_list ap;

        va_start(ap, fmt);
        auto res = vsnprintf(g_log_print_pos, sizeof(g_log_print_buf) - len, fmt, ap);
        va_end(ap);
        if (res < 0 || res + len >= sizeof(g_log_print_buf)) {
            flush = true;
        }
        g_log_print_pos += res;
        if (flush) {
            fputs(g_log_print_buf, stdout);
            g_log_print_buf[0] = 0;
            g_log_print_pos = g_log_print_buf;
        }
        return res;
    }
    */
#define DEBUG_PRINT(Msg, ...)  // PRINTLN(Msg, __VA_ARGS__)
#define DUMP_PRINT(Msg, ...)   // log_print(false, Msg, __VA_ARGS__)
#define DUMP_PRINT_END()       // log_print(true, "\r\n")

    class lua_parser {
    private:
        enum next_ret { cont, finish, abort };
        struct elem {
            enum Type : uint8_t { Array, Map } type;
            bool is_key;
            int32_t pos{};
            int32_t len;
            elem(Type type, int32_t len): type(type), len(len), is_key(false) {}

            template<typename Visitor>
            next_ret next(lua_State* L, Visitor& v) {
                DEBUG_PRINT("elem::next: type = %s, pos = %d, len=%d, stack size = %d",
                            (type == Map ? "map" : "array"),
                            pos,
                            len,
                            lua_gettop(L));
                if (pos >= len) {
                    if (type == Map) {
                        DUMP_PRINT("}");
                        if (!v.end_map())
                            return abort;
                    } else {
                        DUMP_PRINT("]");
                        if (!v.end_array())
                            return abort;
                    }
                    return finish;
                }
                if (type == Map) {
                    if (is_key) {
                        DUMP_PRINT(":");
                        if (!v.end_map_key())
                            return abort;
                        if (!v.start_map_value())
                            return abort;
                    } else {
                        DUMP_PRINT(",");
                        if (!v.end_map_value())
                            return abort;
                        lua_pop(L, 1);
                        DEBUG_PRINT("lua_pop, stak size = %d", lua_gettop(L));
                        ++pos;
                        if (pos >= len) {
                            if (!v.end_map())
                                return abort;
                            return finish;
                        }
                        if (!v.start_map_key())
                            return abort;
                    }
                } else {
                    DUMP_PRINT(",");
                    if (!v.end_array_item())
                        return abort;
                    ++pos;
                    if (pos == len) {
                        if (!v.end_array())
                            return abort;
                        return finish;
                    }
                    lua_pop(L, 1);
                    DEBUG_PRINT("lua_pop, stak size = %d", lua_gettop(L));
                    if (!v.start_array_item())
                        return abort;
                }
                return cont;
            }
            int get(lua_State* L) {
                if (type == Map) {
                    if (!is_key) {
                        lua::next_field(L, -2);
                        DEBUG_PRINT("lua_next, stak size = %d, key_type = %s, val_type = %s",
                                    lua_gettop(L),
                                    lua_typename(L, lua_type(L, -2)),
                                    lua_typename(L, lua_type(L, -1)));
                        is_key = true;
                        return -2;
                    } else {
                        is_key = false;
                        return -1;
                    }
                } else {
                    lua_rawgeti(L, -1, pos + 1);
                    DEBUG_PRINT("lua_rawgeti(%d), stak size = %d", pos + 1, lua_gettop(L));
                    return -1;
                }
            }
        };

    public:
        explicit lua_parser(lua_State* L): L(L) {}
        ~lua_parser() {}
        template<typename Visitor>
        void parse(Visitor& v, int idx = -1) {
            DUMP_PRINT("Start parse(v: %s, idx: %d, top: %d, type: %s\n",
                       typeid(v).name(),
                       idx,
                       lua_gettop(L),
                       lua_typename(L, lua_type(L, idx)));
            auto top = lua_gettop(L);
            DEBUG_PRINT("Current top: %d", top);
            if (idx != -1 && !(top == 1 && idx == 1)) {
                lua_pushvalue(L, idx);
                DEBUG_PRINT("Push value by idx = %d on top", top);
            }
            idx = -1;
            auto clear_stack = [this, top, &v] {
                lua_settop(L, top);
                DUMP_PRINT(" -> End Parse(v: %s, top: %d)\n", typeid(v).name(), lua_gettop(L));
                DUMP_PRINT_END();
                DEBUG_PRINT("clear_stack, current top: %d", lua_gettop(L));
            };
            while (true) {
                bool start_collection = false;
                DEBUG_PRINT("Stack: %d, Start type: %s", lua_gettop(L), lua_typename(L, lua_type(L, idx)));
                switch (lua_type(L, idx)) {
                case LUA_TNIL:
                    DUMP_PRINT("nil");
                    if (!v.visit_nil())
                        return clear_stack();
                    break;
                case LUA_TBOOLEAN:
                    DUMP_PRINT("%s", lua_toboolean(L, idx) ? "True" : "False");
                    if (!v.visit_boolean(lua_toboolean(L, idx)))
                        return clear_stack();
                    DEBUG_PRINT("visit_boolean(%s)", (lua_toboolean(L, idx) ? "True" : "False"));
                    break;
                case LUA_TNUMBER: {
                    if (lua_isinteger(L, idx)) {
                        DUMP_PRINT("%s", lua_toboolean(L, idx) ? "True" : "False");
                        if (!v.visit_integer(lua_tointegerx(L, idx, nullptr)))
                            return clear_stack();

                        DEBUG_PRINT(" visit_integer(%lld)", lua_tointegerx(L, idx, nullptr));
                    } else {
                        DUMP_PRINT("%f", lua_tonumberx(L, idx, nullptr));
                        if (!v.visit_float64(lua_tonumberx(L, idx, nullptr)))
                            return clear_stack();

                        DEBUG_PRINT("visit_float64(%lf)", lua_tonumberx(L, idx, nullptr));
                    }
                    break;
                }
                case LUA_TSTRING: {
                    size_t len = 0;
                    const char* lua_str = lua_tolstring(L, idx, &len);
                    DUMP_PRINT("\"%s\"", lua_str);
                    if (!v.visit_str(lib::string_view(lua_str, len)))
                        return clear_stack();
                    DEBUG_PRINT("visit_str(\"%s\")", lua_str);
                    break;
                }
                case LUA_TTABLE: {
                    bool is_array;
                    auto len = lua::table_len(L, is_array, idx);
                    if (is_array) {
                        DUMP_PRINT("[");
                        if (!v.start_array(len))
                            return clear_stack();

                        DEBUG_PRINT("start_array(%lld), m_ctx size: %lld", len, m_ctx.size());
                        start_collection = len != 0;
                        if (start_collection) {
                            m_ctx.push_back(elem(elem::Array, len));
                            if (!v.start_array_item())
                                return clear_stack();
                        } else {
                            DUMP_PRINT("]");
                            if (!v.end_array()) {
                                return clear_stack();
                            }
                        }
                    } else {
                        DUMP_PRINT("{");
                        if (!v.start_map(len))
                            return clear_stack();

                        DEBUG_PRINT("start_map(%lld), m_ctx size: %lld", len, m_ctx.size());
                        start_collection = len != 0;
                        if (start_collection) {
                            m_ctx.push_back(elem(elem::Map, len));
                            if (!v.start_map_key())
                                return clear_stack();
                        } else {
                            DUMP_PRINT("}");
                            if (!v.end_map()) {
                                return clear_stack();
                            }
                        }
                    }
                    break;
                }
                case LUA_TLIGHTUSERDATA:
                case LUA_TUSERDATA: {
                    lib::string_view mt_name = "";

                    if (lua_getmetatable(L, idx)) {
                        if (lua_getfield(L, -1, "__name") == LUA_TSTRING) {
                            size_t len;
                            auto name = lua_tolstring(L, -1, &len);
                            mt_name = {name, len};
                        }
                        lua_pop(L, 2); /* remove metatable and name*/
                    }
                    DUMP_PRINT("userdata<%s:%p>", mt_name.empty() ? "" : mt_name.data(), lua_touserdata(L, idx));
                    DEBUG_PRINT("start_ud(0x%p, %s)", lua_touserdata(L, idx), mt_name.data());
                    if (!v.visit_ud(lua_touserdata(L, idx), mt_name))
                        return clear_stack();
                    break;
                }
                case LUA_TFUNCTION:
                    DUMP_PRINT("<func>");
                case LUA_TTHREAD:
                    DUMP_PRINT("<thread>");
                default:
                    break;
                }
                if (m_ctx.empty())
                    return clear_stack();
                if (!start_collection) {
                    while (true) {
                        next_ret r = m_ctx.back().next(L, v);
                        if (r == finish) {
                            DUMP_PRINT("%s", m_ctx.back().type == elem::Array ? "]" : "}");
                            m_ctx.pop_back();
                            lua_pop(L, 1);
                            DEBUG_PRINT("collection finished, stak size = %d, m_ctx size = %lld",
                                        lua_gettop(L),
                                        m_ctx.size());
                            if (m_ctx.empty())
                                return clear_stack();
                        } else if (r == cont) {
                            break;
                        } else {
                            // abort
                            return clear_stack();
                        }
                    }
                } else {
                    if (m_ctx.back().type == elem::Map) {
                        lua_pushnil(L);
                        DEBUG_PRINT("start map collection: lua_pushnil stak size = %d", lua_gettop(L));
                    }
                }
                idx = m_ctx.back().get(L);
            }
            clear_stack();
        }

    private:
        lua_State* L;
        boost::container::small_vector<elem, 4> m_ctx;
        // std::vector<elem> m_ctx;
    };
}}  // namespace mplc::lua
