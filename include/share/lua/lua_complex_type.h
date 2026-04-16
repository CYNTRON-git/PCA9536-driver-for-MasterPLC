#include<string>


class LuaComplexType
{
	std::string Name;
public:
	LuaComplexType(void);
	~LuaComplexType(void);
	static LuaComplexType *FromLua(lua_State *L,int Index);

};

