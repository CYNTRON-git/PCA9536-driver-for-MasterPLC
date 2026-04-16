#ifndef __BASE_LUA_FB_H__
#define __BASE_LUA_FB_H__
//#define _CRT_SECURE_NO_WARNINGS 1

#include "st/lSTlib.h"
#define lua_pushstring_stl(s) lua_pushlstring(L, s.c_str(), s.size())
// Базовый класс для представления C/C++ ФБ в Lua
class BaseLuaFB {
protected:
    FBData _data;
    std::string _Name;  // Имя типа ФБ

public:
    MPLCSHARE_API BaseLuaFB(lua_State* L, const char* name);
    MPLCSHARE_API virtual ~BaseLuaFB();
    MPLCSHARE_API virtual void Init(lua_State* L);

    virtual int getField(lua_State* L) = 0;  //Обработчик __index из lua
    virtual int setField(lua_State* L) = 0;  //Обработчик __newindex из lua

    //Обработчик __call из lua, вызывается на каждом цикле программы Lua
    virtual int call(lua_State* L) = 0;
    std::string Name(void) const { return _Name; }
    void SetEnO(bool v) { _data.EnO = v ? 1 : 0; }
};

enum BaseAsyncLuaFBState { Stopped, Running, Finished };

template<class T>
class BaseAsyncLuaFB : public BaseLuaFB {
    mplc::lib::thread* _thread;

protected:
    BaseAsyncLuaFBState _state;

public:
    BaseAsyncLuaFB(lua_State* L, const char* name): BaseLuaFB(L, name) {
        _state = Stopped;
        _thread = new mplc::lib::thread(BaseAsyncLuaFB<T>::ThreadFuncImpl, this);
    }

    virtual ~BaseAsyncLuaFB() {
        _thread->join();
        _state = Finished;
    }
    virtual void ThreadFuncImpl() = 0;
};

class SimpeTON {
    bool _state;

public:
    SimpeTON() { _state = false; }
    bool Check(bool state) {
        bool false_true = !_state && state;
        _state = state;
        return false_true;
    }
    bool Get(void) const { return _state; }
};

// Служкбный класс для работы с C/C++ ФБ, представленным в Lua
class LuaFBUtil {
    static const char CheckErrStrPostfix[];  // = "expected.";
    std::string _Name;
    char* _ErrStr;

public:
    /*	luaL_reg *Lib_f;
        luaL_reg *Lib_m;*/
    MPLCSHARE_API LuaFBUtil(const std::string& name);

    ~LuaFBUtil() { free(_ErrStr); }

    BaseLuaFB* check(lua_State* L) const {
        void* ud = luaL_checkudata(L, 1, _Name.c_str());
        luaL_argcheck(L, ud != NULL, 1, _ErrStr);
        return static_cast<BaseLuaFB*>(ud);
    }
};
#endif  //__BASE_LUA_FB_H__
