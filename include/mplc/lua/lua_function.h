#pragma once
#include <share/config.h>
#include <mplc/libs/containers.hpp>
#include <mplc/lua/lua_bind.hpp>
// --------------------------------
//
namespace mplc { namespace lua {
    // TODO: добавить luaL_ref(L, LUA_REGISTRYINDEX); для возможности использовать несколько биндов одновременно
    class lua_function final {
    public:
        lua_function(const lua_function&) = delete;
        MPLCSHARE_API lua_function(lua_function&& val) noexcept;
        lua_function& operator=(const lua_function&) = delete;
        MPLCSHARE_API lua_function& operator=(lua_function&& val) noexcept;

        MPLCSHARE_API lua_function(lua_State* L, const char* global_table, const char* function);
        MPLCSHARE_API lua_function(lua_State* L, lib::string_view lua_path);
        bool inited() const {
            return m_inited;
        }
        /*template<class Head>
        bool bind(int type, Head&& head) {
            int type = 0;
            if (lua_gettop(L) == stack_top) {
                type = lua_getglobal(L, )
            }
        }

        template<class Head, class... Tail>
        bool bind(int type, Head&& head, Tail&&... tail) {
            bool ok = true;
            int count = lua::set(L, head, &ok);
            if (count != 1) {
                lua_pop(L, count);
                return false;
            }
            lua_rawget(L, -2);
        }
        template<class... Path>
        MPLCSHARE_API lua_function(lua_State* L, Path&&... path): L(L), stack_top(lua_gettop(L)) {
            lua_getglobal(L, "_G");
            if (bind(std::forward<Path>(path)...)) {
                inited = true;
            } else {
                lua_settop(L, stack_top);
            }
        }*/
        MPLCSHARE_API ~lua_function();

        // return pcall status
        template<class... Args>
        int operator()(Args&&... args);

        // 0 - OK, -1  - Error
        template<class... Args>
        int pop(Args&&... args);

    protected:
        MPLCSHARE_API int set_function_args(lua_State*, bool& ok);

        template<class Head, class... Tail>
        int set_function_args(lua_State* L, bool& ok, Head&& arg, Tail&&... tail);

        template<size_t Argn, class Head>
        void pop_args(Head& arg);

        template<size_t Argn, class Head, class... Tail>
        void pop_args(Head& arg, Tail&... tail);

        MPLCSHARE_API void bad_stack() const;

    private:
        std::string fun_name;
        lua_State* L{nullptr};
        int stack_top{0};  // stack size before execute
        bool m_inited{false};
    };

    template<class Head, class... Tail>
    int lua_function::set_function_args(lua_State* L, bool& ok, Head&& arg, Tail&&... tail) {
        if (ok) {
            // lua::set(L, arg, &ok) + set_function_args(L, ok, std::forward<Tail>(tail)...);
            // оптимизатор меняет порядок вычисления аргументов в operator+()
            int count = lua::set(L, arg, &ok);
            return count + set_function_args(L, ok, std::forward<Tail>(tail)...);
        } else {
            return 0;
        }
    }

    template<class... Args>
    int lua_function::operator()(Args&&... args) {
        if (!m_inited) {
            return -1;
        }
        int prev_res_count = lua_gettop(L) - stack_top - 1;
        if (prev_res_count > 0) {
            lua_pop(L, prev_res_count);
        } else if (prev_res_count < 0) {
            bad_stack();
            return -1;
        }
        lua_pushvalue(L, -1); /* push lua func on top */
        bool ok = true;
        int args_count = set_function_args(L, ok, std::forward<Args>(args)...);
        if (!ok || args_count != sizeof...(Args)) {
            PRINTLN("Bad push args in function: %s", fun_name.c_str());
            lua_pop(L, args_count);
            return -1;
        }
        int status = lua_pcall(L, args_count, LUA_MULTRET, 0);
        if (status) {
            PRINTLN("%s", lua_tolstring(L, -1, nullptr));
            lua_pop(L, 1);
        }
        return status;
    }
    template<size_t Argn, class Head>
    void lua_function::pop_args(Head& arg) {
        lua::get(L, arg, -1);
        lua_pop(L, static_cast<int>(Argn));
    }
    template<size_t Argn, class Head, class... Tail>
    void lua_function::pop_args(Head& arg, Tail&... tail) {
        lua::get(L, arg, -1 - static_cast<int>(sizeof...(tail)));
        pop_args<Argn, Tail...>(std::forward<Tail>(tail)...);
    }

    template<class... Args>
    int lua_function::pop(Args&&... args) {
        // count - stack on init
        // 1 - function
        int ret_count = lua_gettop(L) - stack_top - 1;
        if (sizeof...(Args) > ret_count) {
            return -1;
        }
        pop_args<sizeof...(Args), Args...>(std::forward<Args>(args)...);
        return 0;
    }
}}  // namespace mplc::lua
