#pragma once
#include <mplc/libs/string_view.hpp>
#include <lua.hpp>
#include <opcua.h>
#include <boost/type_traits.hpp>
#include <boost/core/enable_if.hpp>
#include "mplc/macros/not_implemented.h"
#include <vector>
namespace mplc { namespace lua {

    template<class Tag, class T>
    OpcUa_StatusCode get(lua_State* L, T& val, int i = -1);
    template<class Tag, class T>
    int set(lua_State* L, const T& val, bool* ok = nullptr);
    template<class T>
    OpcUa_StatusCode get(lua_State* L, T& val, int i = -1) {
        return ::mplc::lua::get<void>(L, val, i);
    }
    template<class T>
    int set(lua_State* L, const T& val, bool* ok = nullptr) {
        return set<void>(L, val, ok);
    }

    template<class T, class _V = void>
    struct Bind {
        template<class Tag>
        static int push(lua_State* L, const T& val, bool* ok = nullptr) {
            MPLC_NOT_IMPLEMENTED(sizeof(T) == 0);
            return 0;
        }

        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, const T& val, int i = -1) {
            MPLC_NOT_IMPLEMENTED(sizeof(T) == 0);
            return 0;
        }
    };

    template<>
    struct Bind<bool> {
        template<class Tag>
        static int push(lua_State* L, const bool& val, bool* ok = nullptr) {
            lua_pushboolean(L, val);
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, bool& val, int i = -1) {
            val = lua_toboolean(L, i);
            return OpcUa_Good;
        }
    };

    template<class First, class Second>
    struct Bind<std::pair<First, Second>> {
        template<class Tag>
        static int push(lua_State* L, const std::pair<First, Second>& val, bool* ok = nullptr) {
            bool _ok = false;
            int n = set<Tag>(L, val.first, &_ok);
            if (_ok)
                n += set<Tag>(L, val.second, &_ok);
            if (!_ok) {
                lua_pop(L, n);
                n = 0;
            }
            if (ok)
                *ok = _ok;
            return n;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, std::pair<First, Second>& val, int i = -1) {
            int f_i = (i < 0) ? i - 1 : i;
            int s_i = (i < 0) ? i : i + 1;
            OpcUa_ReturnErrorIfBad(Bind<First>::template get<Tag>(L, val.first, f_i));
            OpcUa_ReturnErrorIfBad(Bind<First>::template get<Tag>(L, val.second, s_i));
            return OpcUa_Good;
        }
    };
    template<>
    struct Bind<lua_CFunction> {
        template<class Tag>
        static int push(lua_State* L, const lua_CFunction& val, bool* ok = nullptr) {
            lua_pushcfunction(L, val);
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, lua_CFunction& val, int i = -1) {
            val = lua_tocfunction(L, i);
            return OpcUa_Good;
        }
    };

    template<class T>
    struct Bind<T, typename boost::enable_if<boost::is_enum<T>>::type> {
        template<class Tag>
        static int push(lua_State* L, const T& val, bool* ok = nullptr) {
            lua_pushinteger(L, val);
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, T& val, int i = -1) {
            val = static_cast<T>(lua_tointeger(L, i));
            return OpcUa_Good;
        }
    };

    template<class T>
    struct Bind<T, typename boost::enable_if<boost::is_integral<T>>::type> {
        template<class Tag>
        static int push(lua_State* L, const T& val, bool* ok = nullptr) {
            lua_pushinteger(L, val);
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, T& val, int i = -1) {
            val = static_cast<T>(lua_tointeger(L, i));
            return OpcUa_Good;
        }
    };

    template<class T>
    struct Bind<T, typename boost::enable_if<boost::is_floating_point<T>>::type> {
        template<class Tag>
        static int push(lua_State* L, const T& val, bool* ok = nullptr) {
            lua_pushnumber(L, val);
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, T& val, int i = -1) {
            val = lua_tonumber(L, i);
            return OpcUa_Good;
        }
    };

    template<class Tag, class T>
    OpcUa_StatusCode get(lua_State* L, T& val, int i) {
        return Bind<T>::template get<Tag>(L, val, i);
    }

    template<class Tag, class T>
    int set(lua_State* L, const T& val, bool* ok) {
        return Bind<T>::template push<Tag>(L, val, ok);
    }

    template<>
    inline OpcUa_StatusCode get<void, const void*>(lua_State* L, const void*& val, int idx) {
        val = lua_topointer(L, idx);
        return OpcUa_Good;
    }
    template<>
    inline int set<void, void*>(lua_State* L, void* const& val, bool* ok) {
        lua_pushlightuserdata(L, val);
        if (ok)
            *ok = true;
        return 1;
    }

    template<>
    struct Bind<void*> {
        template<class Tag>
        static int push(lua_State* L, void* val, bool* ok = nullptr) {
            lua_pushlightuserdata(L, val);
            if (ok)
                *ok = true;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, const void*& val, int idx = -1) {
            if (lua_type(L, idx) != LUA_TLIGHTUSERDATA) {
                val = nullptr;
                return OpcUa_BadTypeMismatch;
            }
            val = lua_topointer(L, idx);
            return OpcUa_Good;
        }
    };
    // path должен содержать С-строки с завершающим 0
    template<class T>
    OpcUa_StatusCode get_by_path(lua_State* L, const std::vector<lib::string_view>& path, T& val) {
        if (path.empty()) {
            return OpcUa_BadNothingToDo;
        }
        OpcUa_StatusCode sc = OpcUa_Good;
        bool has_value = true;
        auto top = lua_gettop(L);
        lua_getglobal(L, path.front().data());
        for (size_t i = 1; i < path.size() && has_value; ++i) {
            switch (lua_type(L, -1)) {
            case LUA_TTABLE:
            case LUA_TUSERDATA:
                lua_getfield(L, -1, path[i].data());
                break;
            default:
                has_value = false;
                break;
            }
        }
        if (has_value) {
            lua::get(L, val);
        } else {
            sc = OpcUa_BadNotFound;
        }
        lua_settop(L, top);
        return sc;
    }
    /*template<class... Args>
    Wrapper<Args...> wrap(Args&&... args);*/
}}  // namespace mplc::lua
