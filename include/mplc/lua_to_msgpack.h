#pragma once
#include <msgpack/object.hpp>
#include <lua.hpp>
#include <mplc/libs/string_view.hpp>
#include <mplc/libs/smart_ptr.hpp>

namespace mplc { namespace vm {
    struct VmType;
}}  // namespace mplc::vm

namespace mplc { namespace lua {
    enum class ChangeType { None, Simple, Full, NotFound, Error };

    ChangeType has_changes(lua_State* L, msgpack::object& o, const vm::VmType* type, int idx = -1);
    ChangeType update_object(lua_State* L,
                             msgpack::object& o,
                             lib::unique_ptr<msgpack::zone>& zone,
                             const vm::VmType* type = nullptr,
                             int idx = -1);
    ChangeType update_object(lua_State* L, msgpack::object_handle& oh, const vm::VmType* type = nullptr, int idx = -1);
}}  // namespace mplc::lua
