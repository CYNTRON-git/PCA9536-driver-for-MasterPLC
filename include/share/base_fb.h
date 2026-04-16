#pragma once

#define LUA_USER_H "share/lua/lua_cpp_conf.h"
#include "mplcshare.h"
#include "lua/base_lua_fb.h"
#include "lua/lua_addins.h"
#include "share/lua/lua_utils.h"
#include "fb_macros.h"
#include "inputs_fb.h"

template<class FB>
class BaseFB : public BaseLuaFB, public InputsFB {
protected:
    virtual void Inited() {}
    virtual void InitedInternal(lua_State*) {}

public:
    BaseFB(lua_State* L, const std::string& FBName): BaseLuaFB(L, FBName.c_str()) {
        LoadInputs(FB::GetMetaTable());
    }

    virtual ~BaseFB() {}

    virtual void Init(lua_State* L) override {
        // Push another reference to the table on top of the stack (so we know
        // where it is, and this function can work for negative, positive and
        // pseudo indices
        lua_pushvalue(L, -2);
        // stack now contains: -1 => table
        lua_pushnil(L);
        // stack now contains: -1 => nil; -2 => table
        while(lua_next(L, -2)) {
            // stack now contains: -1 => value; -2 => key; -3 => table
            if(lua_isstring(L, -2)) setField(L);
            // pop value, leaving key
            lua_pop(L, 1);
            // stack now contains: -1 => key; -2 => table
            //		TraceLuaStackD(L, -10, 10, true);
        }
        // stack now contains: -1 => table (when lua_next returns 0 it pops the key
        // but does not push anything.)
        // Pop table
        lua_pop(L, 1);
        // Stack is now the same as it was on entry to this function
        InitedInternal(L);
        Inited();
    }

    virtual int call(lua_State*)
        override { /* обработчик __call из lua, вызывается на каждом цикле программы Lua */
        FB* pFB = static_cast<FB*>(this);
        pFB->Execute();
        return 1;
    }
    virtual int setField(lua_State* L) override { /* обработчик __newindex из lua */
        return ReadFrom(L, _data);
    }
    virtual int getField(lua_State* L) override { /* обработчик __index из lua */
        return WriteTo(L, _data);
    }
    static const std::string& MakeMetadata(const char* name) {
        static std::string meta;
        if(meta.empty()) {
            const fb_input_meta* arr = FB::GetMetaTable();
            std::ostringstream oss;
            oss << "TYPES.StandardFB." << name << "= FBMetadata({ Name = '" << name
                << "', Hash = 1001, \n"
                << "Vars = \n"
                << "	{\n";
            for(int i = 0; arr[i].name; ++i) {
                if(arr[i].type == ParamType::__VARIANT) continue;
                if(i > 0) oss << ",\n";
                oss << "		" << arr[i].name << " = { Hash = " << i
                    << ", Type = " << ParamType(arr[i].type).GetLuaType() << ", Retain = true }";
            }
            oss << "\n	}\n"
                << "})\n";
            std::string tmp = oss.str();
            meta.assign(oss.str());
        }
        return meta;
    }
    virtual void Execute() {}
};
