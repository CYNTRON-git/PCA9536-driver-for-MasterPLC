#pragma once
#include <boost/core/span.hpp>
#include "bind_base.hpp"

namespace mplc { namespace lua {

    template<class T>
    struct Bind<boost::span<T>> {
        template<class Tag>
        static int push(lua_State* L, const boost::span<T>& arr, bool* ok = nullptr) {
            lua_createtable(L, arr.size(), 0);
            size_t i = 1;
            bool r_ok = true;
            for (const auto& elem: arr) {
                int count = lua::set(L, i, &r_ok);
                if (r_ok) {
                    count += lua::set<Tag, T>(L, elem, &r_ok);
                }
                if (!r_ok || count != 2) {
                    lua_pop(L, count);
                    break;
                }
                lua_rawset(L, -3);
                ++i;
            }
            if (!r_ok) {
                lua_pop(L, 1);
            }
            if (ok)
                *ok = r_ok;
            return 1;
        }
    };

}}  // namespace mplc::lua
