#pragma once
#include "bind_opcua_base.hpp"
#include "../bind_datetime.hpp"
namespace mplc { namespace lua {
    template<>
    struct Bind<OpcUa_DateTime> {
        template<class Tag>
        static int push(lua_State* L, OpcUa_DateTime const& v, bool* ok = nullptr) {
            return set<Tag>(L, DateTime::from(v), ok);
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_DateTime& v, int idx = -1) {
            DateTime ft;
            auto sc = lua::get<Tag>(L, ft, idx);
            v = ft.into<OpcUa_DateTime>();
            return sc;
        }
    };

}}  // namespace mplc::lua
