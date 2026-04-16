#pragma once
#include <share/config.h>
#include <cstddef>
typedef struct lua_State lua_State;
namespace mplc { namespace lua {
    enum class LType {
        tNil,
        tBool,
        tLightData,
        tFloat64,
        tString,
        tTable,
        tFunction,
        tUserData,
        tThread,
        tInt64,
    };
    MPLCSHARE_API LType get_type(lua_State* L, int idx);
    MPLCSHARE_API int next_field(lua_State* L, int idx);
    MPLCSHARE_API size_t table_len(lua_State* L, bool& is_array, int idx = -1);
    MPLCSHARE_API bool is_array(lua_State* L, int idx = -1);
}}  // namespace mplc::lua
