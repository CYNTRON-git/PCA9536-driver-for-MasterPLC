#pragma once

#include <mplc/vm/vminfo.h>
#include "bind_base.hpp"
#include "mplc/api/ref_to.h"

namespace mplc { namespace lua {

    template<>
    struct Bind<api::RefTo> {
        template<class Tag>
        static int push(lua_State* L, const api::RefTo& value, bool* ok = nullptr) {
            if (!value.raw.empty())
                return lua::set<Tag>(L, value.raw, ok);
            if (value.path.empty())
                return lua::set<Tag>(L, value.id, ok);
            return lua::set<Tag>(L, mplc::to_string(value.id) + "." + value.path, ok);
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, api::RefTo& value, int idx = -1) {
            switch (lua_type(L, idx)) {
            case LUA_TNUMBER:
                lua::get<Tag>(L, value.id, idx);
                break;
            case LUA_TSTRING: {
                lua::get<Tag>(L, value.raw, idx);
                if (value.raw.empty())
                    return OpcUa_BadInvalidArgument;
                vm::ItemID tmp(value.raw);
                value.id = tmp.id;
                value.path.swap(tmp.path);
                break;
            }
            default:
                value.id = -1;
                value.path.clear();
                value.raw.clear();
                return OpcUa_BadInvalidArgument;
            }
            return OpcUa_Good;
        }
    };

}}  // namespace mplc::lua
