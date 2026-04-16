#pragma once
#include <mplc/macros/maybe_unused.h>
#include <mplc/macros/private_name.h>
#include "lua/type_traits_macros.h"
// undef windef.h
#undef IN
#undef OUT

// Presets
#define MPLC_PP_PARAM_SETTER core_get_lua_value
#define MPLC_PP_PARAM_GETTER core_set_lua_value
#define MPLC_FB_PARAM_DIRECT_IN 1
#define MPLC_FB_PARAM_DIRECT_OUT 2
#define MPLC_FB_PARAM_LUA_TYPE get_lua_type
#define MPLC_FB_PARAM_PROP_RETAIN 0
#define MPLC_FB_PARAM_PROP_DIRECTION 1
#define MPLC_FB_PARAM_PROP_NAME 2
#define MPLC_FB_PARAM_PROP_TYPE 3
#define MPLC_FB_PARAM_PROP_CONVERT 4
#define MPLC_FB_TYPE_ENUM_PREFIX __
#define MPLC_FB_NAMESPACE TYPES.StandardFB.
#define MPLC_LUA_STRUCT_NAMESPACE TYPES.Names.
#define MPLC_LUA_ELEMENTARY_NAMESPACE TYPES.Elementary.

// First - Need Convertion, Second - Type in Lua
#define MPLC_FB_SCADA_TYPE_BOOL 0, bool
#define MPLC_FB_SCADA_TYPE_BYTE 1, uint8_t

#define MPLC_FB_SCADA_TYPE_DATE 1, FileTime
#define MPLC_FB_SCADA_TYPE_DT 0, FileTime
#define MPLC_FB_SCADA_TYPE_TIME 1, double
#define MPLC_FB_SCADA_TYPE_TOD 1, double

#define MPLC_FB_SCADA_TYPE_REAL 0, float
#define MPLC_FB_SCADA_TYPE_LREAL 0, double

#define MPLC_FB_SCADA_TYPE_USINT 0, uint8_t
#define MPLC_FB_SCADA_TYPE_UINT 0, uint16_t
#define MPLC_FB_SCADA_TYPE_UDINT 0, uint32_t
#define MPLC_FB_SCADA_TYPE_ULINT 0, uint64_t

#define MPLC_FB_SCADA_TYPE_SINT 0, int8_t
#define MPLC_FB_SCADA_TYPE_INT 0, int16_t
#define MPLC_FB_SCADA_TYPE_DINT 0, int32_t
#define MPLC_FB_SCADA_TYPE_LINT 0, int64_t

#define MPLC_FB_SCADA_TYPE_WORD 1, uint16_t
#define MPLC_FB_SCADA_TYPE_DWORD 1, uint32_t
#define MPLC_FB_SCADA_TYPE_LWORD 1, uint64_t
#define MPLC_FB_SCADA_TYPE_STRING 0, std::string
#define MPLC_FB_SCADA_TYPE_VARIANT 0, OpcUa_VariantHlp
#define MPLC_FB_SCADA_TYPE_REF_TO 0, RefTo
#define MPLC_FB_SCADA_TYPE_WSTRING 0, std::wstring

#define MPLC_FB_SCADA_TYPE_SYSTEM_BOOL_PARAM 0, SCADA_API::system_param<BOOL>
#define MPLC_FB_SCADA_TYPE_SYSTEM_BYTE_PARAM 0, SCADA_API::system_param<BYTE>

#define MPLC_FB_SCADA_TYPE_SYSTEM_DT_PARAM 0, SCADA_API::system_param<DT>

#define MPLC_FB_SCADA_TYPE_SYSTEM_REAL_PARAM 0, SCADA_API::system_param<REAL>
#define MPLC_FB_SCADA_TYPE_SYSTEM_LREAL_PARAM 0, SCADA_API::system_param<LREAL>

#define MPLC_FB_SCADA_TYPE_SYSTEM_INT_PARAM 0, SCADA_API::system_param<INT>
#define MPLC_FB_SCADA_TYPE_SYSTEM_DINT_PARAM 0, SCADA_API::system_param<DINT>
#define MPLC_FB_SCADA_TYPE_SYSTEM_LINT_PARAM 0, SCADA_API::system_param<LINT>

#define MPLC_FB_SCADA_TYPE_SYSTEM_WORD_PARAM 0, SCADA_API::system_param<WORD>
#define MPLC_FB_SCADA_TYPE_SYSTEM_DWORD_PARAM 0, SCADA_API::system_param<DWORD>
#define MPLC_FB_SCADA_TYPE_SYSTEM_LWORD_PARAM 0, SCADA_API::system_param<LWORD>
#define MPLC_FB_SCADA_TYPE_SYSTEM_STRING_PARAM 0, SCADA_API::system_param<STRING>
// clang-format off
// (SCADA_Type, CPP_TYPE, LUA_TYPE)
#undef BOOL
#undef DATE
#undef UINT
#undef INT
#define MPLC_FB_SCADA_TYPES_d \
    (BOOL, bool)(BYTE, uint8_t) \
    (DATE, FileTime)(DT, FileTime, DATE_AND_TIME)(TIME, FileTime)(TOD, FileTime, TIME_OF_DAY) \
    (REAL, float)(LREAL, double) \
    (USINT, uint8_t)(UINT, uint16_t)(UDINT, uint32_t)(ULINT,uint64_t) \
    (SINT, int8_t)(INT, int16_t)(DINT, int32_t)(LINT, int64_t) \
    (WORD, uint16_t)(DWORD, uint32_t)(LWORD, uint64_t)(STRING, std::string) \
    (VARIANT, OpcUa_VariantHlp, ANY)(REF_TO, RefTo)(WSTRING, std::wstring) \
    (SYSTEM_BOOL_PARAM, system_param<BOOL>)(SYSTEM_BYTE_PARAM, system_param<BYTE>) \
    (SYSTEM_DT_PARAM, system_param<DT>) \
    (SYSTEM_REAL_PARAM, system_param<REAL>)(SYSTEM_LREAL_PARAM, system_param<LREAL>) \
    (SYSTEM_INT_PARAM, system_param<INT>)(SYSTEM_DINT_PARAM, system_param<DINT>)(SYSTEM_LINT_PARAM, system_param<LINT>) \
    (SYSTEM_WORD_PARAM, system_param<WORD>)(SYSTEM_DWORD_PARAM, system_param<DWORD>)(SYSTEM_LWORD_PARAM, system_param<LWORD>)(SYSTEM_STRING_PARAM, system_param<STRING>)
// clang-format on

#define MPLC_FB_TYPE_ENUM(Type) SCADA_API::__##Type
#define MPLC_FB_SCADA_TYPE(Type) SCADA_API::Type

#define MPLC_TMPL_MARKER_dyn_fields mplc_use_dynamic_fields
#define MPLC_TMPL_MARKER_inh_fields mplc_has_inherited_fields
#define MPLC_TMPL_MARKER_lockable mplc_lockable_type

#define MPLC_PP_EXPAND_AS_(Prefix, ...) MPLC_PP_EXPAND(MPLC_PP_CAT_N(Prefix, __VA_ARGS__)(__VA_ARGS__))
#define MPLC_PP_EXPAND_AS2(Prefix, ...) MPLC_PP_EXPAND(MPLC_PP_CAT_N(Prefix, __VA_ARGS__)(__VA_ARGS__))

#define MPLC_FB_PARAM_GET_PROP(Prop, ParamTuple)                                                                       \
    BOOST_PP_TUPLE_ELEM(BOOST_PP_CAT(MPLC_FB_PARAM_PROP_, Prop), ParamTuple)

#define MPLC_FB_PARAM_4(Retein, Direct, Name, Type) (Retein, Direct, Name, MPLC_FB_PARAM_GET_TYPE(Type, 1))
#define MPLC_FB_PARAM_5(Retein, Direct, Name, Type, Converter)                                                         \
    (Retein, Direct, Name, MPLC_FB_PARAM_GET_TYPE(Type, 0), Converter)

#define MPLC_FB_PARAM_RETAIN_IN(...) MPLC_PP_EXPAND_AS_(MPLC_FB_PARAM_, true, MPLC_FB_PARAM_DIRECT_IN, __VA_ARGS__)
#define MPLC_FB_PARAM_RETAIN_OUT(...) MPLC_PP_EXPAND_AS_(MPLC_FB_PARAM_, true, MPLC_FB_PARAM_DIRECT_OUT, __VA_ARGS__)
#define MPLC_FB_PARAM_IN(...) MPLC_PP_EXPAND_AS_(MPLC_FB_PARAM_, false, MPLC_FB_PARAM_DIRECT_IN, __VA_ARGS__)
#define MPLC_FB_PARAM_OUT(...) MPLC_PP_EXPAND_AS_(MPLC_FB_PARAM_, false, MPLC_FB_PARAM_DIRECT_OUT, __VA_ARGS__)
#define MPLC_FB_PARAM_F(...) MPLC_PP_EXPAND_AS_(MPLC_FB_PARAM_, false, MPLC_FB_PARAM_DIRECT_IN, __VA_ARGS__)
#define MPLC_FB_PARAM_PROXY(...) MPLC_PP_EXPAND_AS_(MPLC_FB_PARAM_, false, MPLC_FB_PARAM_DIRECT_IN, __VA_ARGS__)

#define MPLC_FB_PARAM_FUNCTION_TMPL_4(ParamTuple) MPLC_FB_PARAM_GET_PROP(TYPE, ParamTuple)
#define MPLC_FB_PARAM_FUNCTION_TMPL_5(ParamTuple)                                                                      \
    MPLC_FB_PARAM_GET_PROP(TYPE, ParamTuple), MPLC_FB_PARAM_GET_PROP(CONVERT, ParamTuple)

#define MPLC_FB_PARAM_FUNCTION_TMPL(ParamTuple)                                                                        \
    BOOST_PP_CAT(MPLC_FB_PARAM_FUNCTION_TMPL_, BOOST_PP_TUPLE_SIZE(ParamTuple))                                        \
    (ParamTuple)
//------------------------- not used

#define MPLC_PP_LUA_META_d(r, LuaType, ParamTuple)                                                                     \
    {BOOST_PP_STRINGIZE(MPLC_FB_PARAM_GET_PROP(NAME, ParamTuple)),                                                     \
                        LuaType<MPLC_FB_PARAM_FUNCTION_TMPL(ParamTuple)>,                                              \
                        MPLC_FB_PARAM_GET_PROP(RETAIN, ParamTuple)},

// clang-format off
#define MPLC_PP_LUA_META(seq)                                                                      \
    static const ScadaFields::LuadMeta* GetMetaTable() {                                           \
        static const ScadaFields::LuadMeta _entries[] = {                                          \
            BOOST_PP_SEQ_FOR_EACH(MPLC_PP_LUA_META_d, MPLC_FB_PARAM_LUA_TYPE, seq)                 \
            {nullptr, nullptr, false}};                                                            \
        return _entries;                                                                           \
    }
// clang-format on

#define MPLC_PP_LUA_REG_FIELD_d(RegFunction, Type) RegFunction<Type>();
#define MPLC_PP_LUA_REG_FIELD(r, RegFunction, ParamTuple)                                                              \
    BOOST_PP_IF(MPLC_FB_PARAM_IS_BASE_TYPE(MPLC_FB_PARAM_GET_PROP(TYPE, ParamTuple)),                                  \
                MPLC_PP_EMPTY,                                                                                         \
                MPLC_PP_LUA_REG_FIELD_d)                                                                               \
    (RegFunction, MPLC_FB_PARAM_GET_PROP(TYPE, ParamTuple))

#define MPLC_PP_LUA_TYPE_REG(ParamsSeq)                                                                                \
    static void RegFields() {                                                                                          \
        BOOST_PP_SEQ_FOR_EACH(MPLC_PP_LUA_REG_FIELD, SCADA_API::RegNewLuaType, ParamsSeq)                              \
    }

#define MPLC_PP_LUA_BIND_PARAM(r, Function, ParamTuple)                                                                \
    Function<This, MPLC_FB_PARAM_FUNCTION_TMPL(ParamTuple)>(BOOST_PP_STRINGIZE(                                        \
                                                                MPLC_FB_PARAM_GET_PROP(NAME, ParamTuple)),                                                                     \
        &This::MPLC_FB_PARAM_GET_PROP(NAME, ParamTuple),                                                               \
        MPLC_FB_PARAM_GET_PROP(RETAIN, ParamTuple));

#define MPLC_PP_LUA_PARAMS_BIND(ParamsSeq)                                                                             \
    template<class This>                                                                                               \
    static void BindFields() {                                                                                         \
        BOOST_PP_SEQ_FOR_EACH(MPLC_PP_LUA_BIND_PARAM, SCADA_API::bind_field, ParamsSeq)                                \
    }

#define MPLC_PP_BASE_LUA_API_DECL()                                                                                    \
    static void RegAsLuaType();                                                                                        \
    static const char* _FullName();                                                                                    \
    static const char* _ShortName();

#define MPLC_PP_LUA_PARAM_TO_SEQ(r, _, UserParam) (MPLC_PP_CAT(MPLC_FB_PARAM_, UserParam))

#define MPLC_PP_LUA_PARAMS_DEFINITION_d(r, _, ParamTuple)                                                              \
    MPLC_FB_PARAM_GET_PROP(TYPE, ParamTuple) MPLC_FB_PARAM_GET_PROP(NAME, ParamTuple);

#define MPLC_PP_LUA_PARAMS_DEFINITION(ParamsSeq) BOOST_PP_SEQ_FOR_EACH(MPLC_PP_LUA_PARAMS_DEFINITION_d, _, ParamsSeq)

#ifdef GTEST_API_
#    define MPLC_PP_GENERATE_LUA_BINDINGS(ParamsSeq)                                                                   \
    public:                                                                                                            \
        MPLC_PP_LUA_PARAMS_DEFINITION(ParamsSeq)                                                                       \
        MPLC_PP_LUA_PARAMS_BIND(ParamsSeq)
#else
#    define MPLC_PP_GENERATE_LUA_BINDINGS(ParamsSeq)                                                                   \
        MPLC_PP_LUA_PARAMS_DEFINITION(ParamsSeq)                                                                       \
    public:                                                                                                            \
        MPLC_PP_LUA_PARAMS_BIND(ParamsSeq)
#endif

#define MPLC_LUA_BINDINGS(...)                                                                                         \
    MPLC_PP_GENERATE_LUA_BINDINGS(                                                                                     \
        BOOST_PP_SEQ_FOR_EACH(MPLC_PP_LUA_PARAM_TO_SEQ, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

// Generating type convertion

#define MPLC_FB_PARAM_IS_CONV_TYPE_2(NeedConvert, ...) NeedConvert
#define MPLC_FB_PARAM_IS_CONV_TYPE_1(Type, ...) 0
#define MPLC_FB_PARAM_IS_CONV_TYPE(Type)                                                                               \
    MPLC_PP_EXPAND_AS2(MPLC_FB_PARAM_IS_CONV_TYPE_, BOOST_PP_CAT(MPLC_FB_SCADA_TYPE_, Type))

#define MPLC_FB_PARAM_IS_BASE_TYPE_2(NeedConvert, ...) 1
#define MPLC_FB_PARAM_IS_BASE_TYPE_1(Type, ...) 0
#define MPLC_FB_PARAM_IS_BASE_TYPE(Type)                                                                               \
    MPLC_PP_EXPAND_AS_(MPLC_FB_PARAM_IS_BASE_TYPE_, BOOST_PP_CAT(MPLC_FB_SCADA_TYPE_, Type))

#define MPLC_FB_PARAM_GET_TYPE_2_d(Type) , MPLC_FB_TYPE_ENUM(Type)
#define MPLC_FB_PARAM_GET_TYPE_1(Type, ...) Type
#define MPLC_FB_PARAM_GET_TYPE_2(Type, Full)                                                                           \
    MPLC_FB_SCADA_TYPE(Type)                                                                                           \
    BOOST_PP_IF(BOOST_PP_AND(Full, MPLC_FB_PARAM_IS_CONV_TYPE(Type)), MPLC_FB_PARAM_GET_TYPE_2_d, MPLC_PP_EMPTY)       \
    (Type)

#define MPLC_FB_PARAM_GET_TYPE(Type, Full)                                                                             \
    MPLC_PP_CAT_N(MPLC_FB_PARAM_GET_TYPE_, BOOST_PP_CAT(MPLC_FB_SCADA_TYPE_, Type))                                    \
    (Type, Full)

#define MPLC_PP_LUA_TYPE_TO_STR(BasePath, LuaType) BOOST_PP_STRINGIZE(BasePath) BOOST_PP_STRINGIZE(LuaType)

#define MPLC_FB_GET_LUA_TYPE_2(ScadaName, CppName) ScadaName
#define MPLC_FB_GET_LUA_TYPE_3(ScadaName, CppName, LuaName) LuaName
#define MPLC_FB_GET_LUA_TYPE(TypeTuple) MPLC_PP_EXPAND_AS_(MPLC_FB_GET_LUA_TYPE_, BOOST_PP_TUPLE_ENUM(TypeTuple))

#define MPLC_FB_GENERATE_GETTER_LUA_TYPE(FunName, LuaName, CppName)                                                    \
    template<>                                                                                                         \
    inline const char* FunName<CppName>(const CppName*) {                                                              \
        return MPLC_PP_LUA_TYPE_TO_STR(TYPES.Elementary., LuaName);                                                    \
    }

// Create default get_lua_type for NOT converteble SCADA types
#define MPLC_INIT_GET_LUA_TYPES_d(r, FunName, Tuple)                                                                   \
    BOOST_PP_IF(MPLC_FB_PARAM_IS_CONV_TYPE(BOOST_PP_TUPLE_ELEM(0, Tuple)),                                             \
                MPLC_PP_EMPTY,                                                                                         \
                MPLC_FB_GENERATE_GETTER_LUA_TYPE)                                                                      \
    (FunName, MPLC_FB_GET_LUA_TYPE(Tuple), BOOST_PP_TUPLE_ELEM(1, Tuple))

#define MPLC_GENERATE_BASE_LUA_TYPES(FunName)                                                                          \
    BOOST_PP_SEQ_FOR_EACH(MPLC_INIT_GET_LUA_TYPES_d, FunName, MPLC_FB_SCADA_TYPES())

#define MPLC_SCADA_TYPE_CREATE_ENUM_NAME(n, prefix, TypeTuple) BOOST_PP_CAT(prefix, BOOST_PP_TUPLE_ELEM(0, TypeTuple))

#define MPLC_SCADA_TYPE_CREATE_ENUM_NAME_c(r, prefix, n, TypeTuple)                                                    \
    BOOST_PP_COMMA_IF(n)                                                                                               \
    BOOST_PP_CAT(prefix, BOOST_PP_TUPLE_ELEM(0, TypeTuple))

#define MPLC_FB_FILTER_CONV_TYPES(n, TypePrefix, TypeTuple)                                                            \
    MPLC_FB_PARAM_IS_CONV_TYPE(BOOST_PP_TUPLE_ELEM(0, TypeTuple))

#define MPLC_FB_GET_CONV_TYPES(TypePrefix, AllTypeSeq)                                                                 \
    BOOST_PP_SEQ_FILTER(MPLC_FB_FILTER_CONV_TYPES, TypePrefix, AllTypeSeq())

#define MPLC_GENERATE_SCADA_API_ENUM(Name)                                                                             \
    enum Name {                                                                                                        \
        BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(MPLC_SCADA_TYPE_CREATE_ENUM_NAME,                                     \
                                                 MPLC_FB_TYPE_ENUM_PREFIX,                                             \
                                                 MPLC_FB_GET_CONV_TYPES(_, MPLC_FB_SCADA_TYPES)))                      \
    }

#define MPLC_ARRAY_1(Type) std::vector<Type>
#define MPLC_ARRAY_2(Type, Enum) std::vector<Type>, Enum
#define MPLC_ARRAY(Type) MPLC_PP_EXPAND_AS_(MPLC_ARRAY_, MPLC_FB_PARAM_GET_TYPE(Type, 1))

// Generate Typedef for SCADA Types
#define MPLC_FB_SCADA_TYPES() BOOST_PP_VARIADIC_SEQ_TO_SEQ(MPLC_FB_SCADA_TYPES_d)

#define TYPEDEF_SCADA_TYPES_d(r, data, Tuple) typedef BOOST_PP_TUPLE_ELEM(1, Tuple) BOOST_PP_TUPLE_ELEM(0, Tuple);

#define TYPEDEF_SCADA_TYPES() BOOST_PP_SEQ_FOR_EACH(TYPEDEF_SCADA_TYPES_d, _, MPLC_FB_SCADA_TYPES())
//---------

#define MPLC_GENERATE_SCADA_API_swich(r, BasePath, TypeTuple)                                                          \
    case MPLC_SCADA_TYPE_CREATE_ENUM_NAME(r, MPLC_FB_TYPE_ENUM_PREFIX, TypeTuple):                                     \
        return MPLC_PP_LUA_TYPE_TO_STR(BasePath, MPLC_FB_GET_LUA_TYPE(TypeTuple));

// Generate Convertion "Functions" from enum value to c++ type
#define MPLC_GENERATE_ENUM_CONVERTER(Struct, Type)                                                                     \
    template<>                                                                                                         \
    struct Struct<MPLC_FB_TYPE_ENUM(Type)> {                                                                           \
        typedef BOOST_PP_VARIADIC_ELEM(1, BOOST_PP_CAT(MPLC_FB_SCADA_TYPE_, Type)) type;                               \
    };

#define MPLC_GENERATE_SCADA_TYPE_CONV_d(r, Struct, TypeTuple)                                                          \
    MPLC_GENERATE_ENUM_CONVERTER(Struct, BOOST_PP_TUPLE_ELEM(0, TypeTuple))

#define MPLC_FB_GENERATE_ENUM_TYPE_CONVERTER(EnumTypes, Struct, TypesSeq)                                              \
    template<EnumTypes type>                                                                                           \
    struct Struct {};                                                                                                  \
    BOOST_PP_SEQ_FOR_EACH(MPLC_GENERATE_SCADA_TYPE_CONV_d, Struct, TypesSeq)

// Main generate API macro
#define MPLC_GENERATE_SCADA_API_d(FunName, EnumName, ConvStruct, TypesSeq)                                             \
    enum EnumName { BOOST_PP_SEQ_FOR_EACH_I(MPLC_SCADA_TYPE_CREATE_ENUM_NAME_c, MPLC_FB_TYPE_ENUM_PREFIX, TypesSeq) }; \
    static const char* FunName(EnumName type) {                                                                        \
        switch (type) {                                                                                                \
            BOOST_PP_SEQ_FOR_EACH(MPLC_GENERATE_SCADA_API_swich, MPLC_LUA_ELEMENTARY_NAMESPACE, TypesSeq)              \
        default:                                                                                                       \
            return nullptr;                                                                                            \
        }                                                                                                              \
    }                                                                                                                  \
    MPLC_FB_GENERATE_ENUM_TYPE_CONVERTER(EnumName, ConvStruct, TypesSeq)

#define MPLC_GENERATE_SCADA_API(FunName, EnumName, ConvStruct)                                                         \
    TYPEDEF_SCADA_TYPES()                                                                                              \
    MPLC_GENERATE_SCADA_API_d(FunName, EnumName, ConvStruct, MPLC_FB_GET_CONV_TYPES(_, MPLC_FB_SCADA_TYPES))

#define MPLC_PP_SCADA_STRUCT_NAME(Lib, Name) BOOST_PP_STRINGIZE(MPLC_LUA_STRUCT_NAMESPACE) #Lib "_" #Name

#define MPLC_PP_SCADA_FB_NAME(Name) BOOST_PP_STRINGIZE(MPLC_FB_NAMESPACE) #Name

#define MPLC_PP_SCADA_SHORTNAME_1(Name) #Name
#define MPLC_PP_SCADA_SHORTNAME_2(Lib, Name) #Lib "_" #Name
#define MPLC_PP_SCADA_FULLNAME(Namespace, ...)                                                                         \
    BOOST_PP_STRINGIZE(Namespace) MPLC_PP_EXPAND_AS_(MPLC_PP_SCADA_SHORTNAME_, __VA_ARGS__)

#define MPLC_PP_BASE_LUA_API_DEF(Class, MplcRegFunction, LuaRegTypeFun, Namespace, ...)                                \
    const char* Class::_ShortName() {                                                                                  \
        return MPLC_PP_EXPAND_AS_(MPLC_PP_SCADA_SHORTNAME_, __VA_ARGS__);                                               \
    }                                                                                                                  \
    const char* Class::_FullName() {                                                                                   \
        return MPLC_PP_SCADA_FULLNAME(Namespace, __VA_ARGS__);                                                         \
    }                                                                                                                  \
    void Class::RegAsLuaType() {                                                                                       \
        using namespace SCADA_API;                                                                                     \
        static bool registred = false;                                                                                 \
        if (registred)                                                                                                 \
            return;                                                                                                    \
        registred = true;                                                                                              \
        if (MplcRegFunction(_ShortName(), ScadaObj<Class>::LuaRegTypeFun) != S_OK) {                                   \
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR,                                                                       \
                        BOOST_STRINGIZE(Class) ": Already registred other type with name '%s'",                        \
                        _ShortName());                                                                                 \
        }                                                                                                              \
    }

/*
 * Use in private section
 * // Write data into Lua
 * int SetField(const std::string& key, lua_State* L);
 * // Read data from Lua
 * int GetField(const std::string& key, lua_State* L) const;
 * // Count fields
 * size_t Size() const;
 */
#define ENABLE_DYNAMIC_FIELDS(To, SetField, GetField)                                                                  \
public:                                                                                                                \
    typedef int MPLC_TMPL_MARKER_dyn_fields;                                                                           \
    static void EnableDynamicFields(ScadaFields* fields) {                                                             \
        fields->dynamic_fields = new DynamicFields<To>(&To::SetField, &To::GetField);                                  \
    }                                                                                                                  \
                                                                                                                       \
private:

#define MPLC_INIT_TYPELIST(Type) typedef SCADA_API::detail::TypeList<Type> __parent;
#define MPLC_ADD_IN_TYPELIST(Type) typedef SCADA_API::detail::TypeList<Type, __parent::type> __parent;
#define MPLC_INHERIT_THIS_FIELDS(Type) MPLC_ADD_IN_TYPELIST(Type)

// Use in private section
#define MPLC_LOCKABLE_TYPE()                                                                                           \
public:                                                                                                                \
    typedef int MPLC_TMPL_MARKER_lockable;                                                                             \
    void Lock() const {                                                                                                \
        __fields_lock.lock();                                                                                          \
    }                                                                                                                  \
    void Unlock() const {                                                                                              \
        __fields_lock.unlock();                                                                                        \
    }                                                                                                                  \
    bool TryLock() const {                                                                                             \
        return __fields_lock.try_lock();                                                                               \
    }                                                                                                                  \
    mplc::lib::mutex& GetFieldsMutex() const {                                                                         \
        return __fields_lock;                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
private:                                                                                                               \
    mutable mplc::lib::mutex __fields_lock

#define DEFINE_SCADA_STRUCT_AS(Class, Lib, ScadaName)                                                                  \
    MPLC_PP_BASE_LUA_API_DEF(Class, RegisterExternalSTStruct, RegStructType, MPLC_LUA_STRUCT_NAMESPACE, Lib, ScadaName)

#define DEFINE_SCADA_STRUCT(Class, Lib) DEFINE_SCADA_STRUCT_AS(Class, Lib, Class)
#define DEFINE_SCADA_STRUCT_AS_LOCAL(Class) DEFINE_SCADA_STRUCT_AS(Class, Lokaljnaya, Class)

#define DEFINE_SCADA_FB_AS(Class, ScadaName)                                                                           \
    MPLC_PP_BASE_LUA_API_DEF(Class, RegisterExternalSTLib, RegFBType, MPLC_FB_NAMESPACE, ScadaName)

#define DEFINE_SCADA_FB(Class) DEFINE_SCADA_FB_AS(Class, Class)

#define DEFINE_SCADA_DRIVER_AS(Class, ScadaName)                                                                       \
    MPLC_PP_BASE_LUA_API_DEF(Class, RegisterExternalSTLib, RegFBType, MPLC_FB_NAMESPACE, ScadaName)

#define DEFINE_SCADA_DRIVER(Class) DEFINE_SCADA_FB_AS(Class, Class)

// Main MACROS
#define MPLC_FB_PARAMS(...)                                                                                            \
    MPLC_LUA_BINDINGS(__VA_ARGS__)                                                                                     \
    MPLC_PP_BASE_LUA_API_DECL()                                                                                        \
    // static const char* FBName();

#define MPLC_STRUCT_FIELDS(...)                                                                                        \
    MPLC_LUA_BINDINGS(__VA_ARGS__)                                                                                     \
    MPLC_PP_BASE_LUA_API_DECL()

#define REG_NEW_SCADA_TYPE(Class) SCADA_API::ScadaObj<Class>::RegSTLib()

// ------------ NEW REGISTER MACROS

#define MPLC_PP_BASE_REG_TYPE(Class, MplcRegFunction, Namespace, ...)                                                  \
    const char* Class::_ShortName() {                                                                                  \
        return MPLC_PP_EXPAND_AS_(MPLC_PP_SCADA_SHORTNAME_, __VA_ARGS__);                                               \
    }                                                                                                                  \
    const char* Class::_FullName() {                                                                                   \
        return MPLC_PP_SCADA_FULLNAME(Namespace, __VA_ARGS__);                                                         \
    }                                                                                                                  \
    void Class::RegAsLuaType() {}                                                                                      \
    static const int MPLC_PRIVATE_NAME(Class) MPLC_UNUSED = MplcRegFunction<Class>()

#define MPLC_REG_PROTOCOL(Class) MPLC_PP_BASE_REG_TYPE(Class, Class)
#define MPLC_REG_PROTOCOL_AS(Class, ScadaName)                                                                         \
    MPLC_PP_BASE_REG_TYPE(Class, SCADA_API::RegisterFbType, MPLC_FB_NAMESPACE, ScadaName)

#define MPLC_REG_FB(Class) MPLC_REG_FB_AS(Class, Class)
#define MPLC_REG_FB_AS(Class, ScadaName)                                                                               \
    MPLC_PP_BASE_REG_TYPE(Class, SCADA_API::RegisterFbType, MPLC_FB_NAMESPACE, ScadaName)

#define MPLC_REG_STRUCT_LOCAL(Class) MPLC_REG_STRUCT(Class, Lokaljnaya)
#define MPLC_REG_STRUCT(Class, Lib) MPLC_REG_STRUCT_AS(Class, Lib, Class)
#define MPLC_REG_STRUCT_AS(Class, Lib, ScadaName)                                                                      \
    MPLC_PP_BASE_REG_TYPE(Class, SCADA_API::RegisterStructType, Lib, ScadaName)
