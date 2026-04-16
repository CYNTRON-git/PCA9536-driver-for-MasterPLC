#pragma once
#include <stack>
#include "bind/bind_base.hpp"

namespace mplc { namespace lua {
    struct LuaWriter {
        lua_State* L;
        enum State { Array, Object, None };
        std::stack<std::pair<int, State> > stack;
        LuaWriter(lua_State* L): L(L) {
            stack.push(std::make_pair(0, None));
        }
        ~LuaWriter() {}
        LuaWriter& StartArray(int elem = 0) {
            switch (stack.top().second) {
            case Array:
                lua::set(L, ++stack.top().first);
                break;
            case Object:
            case None:
                ++stack.top().first;
                break;
            }
            stack.push(std::make_pair(0, Array));
            lua_createtable(L, elem, 0);
            return *this;
        }
        LuaWriter& EndArray() {
            stack.pop();
            switch (stack.top().second) {
            case Array:
            case Object:
                lua_rawset(L, -3);
                break;
            case None:
                break;
            }
            return *this;
        }
        LuaWriter& StartObject(int elem = 0) {
            switch (stack.top().second) {
            case Array:
                lua::set(L, ++stack.top().first);
                break;
            case Object:
            case None:
                ++stack.top().first;
                break;
            }
            stack.push(std::make_pair(0, Object));
            lua_createtable(L, 0, elem);
            return *this;
        }
        LuaWriter& EndObject() {
            stack.pop();
            switch (stack.top().second) {
            case Array:
            case Object:
                lua_rawset(L, -3);
                break;
            case None:
                break;
            }
            return *this;
        }

        template<class Tag, class T>
        LuaWriter& Value(const T& v, bool* ok = nullptr) {
            auto type = stack.top().second;
            if (type == Array) {
                lua_pushinteger(L, ++stack.top().first);
            }
            bool is_ok = true;
            int count = lua::set<Tag>(L, v, &is_ok);
            if (is_ok && (type == None || count == 1)) {
                if (type == Object || type == Array) {
                    lua_rawset(L, -3);
                }
            } else {
                if (type == Array) {
                    --stack.top().first;
                }
                lua_pop(L, count + (type == None ? 0 : 1));
                is_ok = false;
            }
            if (ok)
                *ok = is_ok;
            return *this;
        }
        template<class T>
        LuaWriter& Value(const T& v, bool* ok = nullptr) {
            auto type = stack.top().second;
            if (type == Array) {
                lua_pushinteger(L, ++stack.top().first);
            }
            bool is_ok = true;
            int count = lua::set<void>(L, v, &is_ok);
            if (is_ok && (type == None || count == 1)) {
                if (type == Object || type == Array) {
                    lua_rawset(L, -3);
                }
            } else {
                if (type == Array) {
                    --stack.top().first;
                }
                lua_pop(L, count + (type == None ? 0 : 1));
                is_ok = false;
            }
            if (ok)
                *ok = is_ok;
            return *this;
        }

        template<class Tag = void>
        LuaWriter& Key(lib::string_view key, bool* ok = nullptr) {
            bool is_ok = true;
            if (stack.top().second == Object) {
                int count = lua::set<Tag>(L, key, &is_ok);
                if (count != 1) {
                    lua_pop(L, count);
                    is_ok = false;
                }
            }

            if (ok)
                *ok = is_ok;
            return *this;
        }
        template<class Tag, class T>
        LuaWriter& Field(lib::string_view key, T&& v, bool* ok = nullptr) {
            bool is_ok = stack.top().second == Object;
            if (is_ok) {
                Key<Tag>(key, &is_ok);
            }
            if (is_ok) {
                Value<Tag>(v, &is_ok);
            }
            if (ok)
                *ok = is_ok;
            return *this;
        }
        template<class T>
        LuaWriter& Field(lib::string_view key, T&& v, bool* ok = nullptr) {
            bool is_ok = stack.top().second == Object;
            if (is_ok) {
                Key<void>(key, &is_ok);
            }
            if (is_ok) {
                Value<void>(v, &is_ok);
            }
            if (ok)
                *ok = is_ok;
            return *this;
        }
    };
}}  // namespace mplc::lua
