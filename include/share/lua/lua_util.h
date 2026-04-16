#ifndef _LUA_UTIL_H_
#define _LUA_UTIL_H_

//#include "lua_type.h"
//
//struct BaseKindDsc {
//    const char *Name;
//    LuaTypes Kind;
//};
//
//
//
//struct ArraySubRange
//{
//    int Begin;
//    int End;
//};
//
//
//struct TypeInfo
//{
//    std::string Name;
//    std::vector<ArraySubRange> subranges;
//    LuaTypes kind;
//    LuaType *fileldLType;
//	TypeInfo() : 
//        kind(UnknownType), 
//        fileldLType(nullptr)
//    {}
//};
//
//struct FieldInfo
//{
//    TypeInfo TInfo;
//    std::string Name;
//};
//
//
//class ParseLuaTypesUtil
//{
//    LuaType *unknownType;
//    LuaType *stringType;
//
//    void ParseFields(lua_State *L, LuaInfo &LInfo, std::map<std::string, std::vector<LuaVar> > &unresolvedTypes, std::vector<LuaVar> &tmpVarsList, LuaTypes type);
//    TypeInfo MakeType(lua_State *L, LuaInfo &LInfo, std::map<std::string, std::vector<LuaVar> > &unresolvedTypes, std::vector<LuaVar> &tmpVarsList, LuaTypes type);
//public:
//	ParseLuaTypesUtil()
//	{
//		unknownType = NULL;
//		stringType = NULL;
//	}
//    void FillLuaTypes(lua_State *L, const LuaTypes type, LuaInfo &LInfo);
//};
//
//enum BreakPointTypes
//{
//	AtLine =1 
//};
//
//
//class BreakPoint
//{
//public:
//	std::string Souurce;
//	bool Enabled;
//};
//
//class LuaDebug
//{
//	typedef std::vector<LuaDataProvider*> LuaDataProviderVector;
//	static LuaDataProviderVector _LuaDataProviderList;
//	
//	typedef std::vector<LuaDebug*> LuaDebugVector;
//	static LuaDebugVector _LuaDebugList;
//
//	typedef std::map<unsigned int, char> AtLineBPointsMap;
//	AtLineBPointsMap AtLineBPointsList;
//
//	unsigned int _CurrentLine;
//
//
//	static void Hook(lua_State *L, lua_Debug *ar);
//	static bool CheckBrerakpoint(int num, const lua_Debug *ar);
//	
//public:
//	LuaDebug(lua_State *L);
//	//
//	// TODO: Надо лдобавить в класс информацию о усиановленых hook 
//	// 
//
//	MPLCSHARE_API LuaDebug(LuaDataProvider *tm);
//	
//	MPLCSHARE_API void SetHook(size_t taskIdx, int hookMask);
//	
//	// Функция отключения hook
//	MPLCSHARE_API void HookOff(size_t taskIdx);
//
//
//	MPLCSHARE_API void SetBPAtLine(unsigned int line)
//	{
//		AtLineBPointsList[line] = 1;
//	}
//};

#endif // _LUA_UTIL_H_
