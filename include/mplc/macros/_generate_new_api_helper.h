#pragma once
#include <boost/preprocessor.hpp>
#undef MPLC_OBJECT
#undef MPLC_PROTOCOL_TYPE
#undef MPLC_FB_TYPE
#undef MPLC_DECLARE_PROPERTIES
#undef MPLC_In
#undef MPLC_Out
#undef MPLC_InOut
#undef MPLC_FB_PARAMS
#define MPLC_FB_PARAMS(...) BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)
#define _MPLC_F_BIND_IN(Name, Type) MPLC_In(Name),
#define _MPLC_F_BIND_OUT(Name, Type) MPLC_Out(Name),

#define _MPLC_BIND_FIELD(r, _, UserParam) MPLC_PP_CAT(_MPLC_F_BIND_, UserParam)
#define _MPLC_GENERATE_BIND(Class, PARAMS_SEQ)                                                                         \
    MPLC_DECLARE_PROPERTIES(Class){BOOST_PP_SEQ_FOR_EACH(_MPLC_BIND_FIELD, _, PARAMS_SEQ)};

#define _MPLC_F_IN(Name, Type) Type Name;
#define _MPLC_F_OUT(Name, Type) Type Name;

#define _MPLC_CREATE_FIELD(r, _, UserParam) BOOST_PP_EXPAND(MPLC_PP_CAT(_MPLC_F_, UserParam))

#define _MPLC_CREATE_FILEDS(PARAMS_SEQ) BOOST_PP_SEQ_FOR_EACH(_MPLC_CREATE_FIELD, _, PARAMS_SEQ)

#define _MPLC_GENERATE_BASE(Class, LuaName, OLD_PARAMS_MACRO, TYPE)                                                    \
    "------------- Class definition for .h -------------";                                                             \
    class Class final : public mplc::api::ScadaFB {                                                                    \
    public:                                                                                                            \
        MPLC_OBJECT(Class);                                                                                            \
        _MPLC_CREATE_FILEDS(OLD_PARAMS_MACRO)                                                                          \
        void Execute() override;                                                                                       \
    };                                                                                                                 \
    "---------- Property definition for .cpp -----------";                                                             \
    "#include <mplc/api/api_storage.h>";                                                                               \
    _MPLC_GENERATE_BIND(Class, OLD_PARAMS_MACRO)                                                                       \
    TYPE(LuaName, Class);

#define MPLC_GENERATE_FB(Class, LuaName, OLD_PARAMS_MACRO)                                                             \
    "include <mplc/api/scada_fb.h>";                                                                                   \
    _MPLC_GENERATE_BASE(Class, LuaName, OLD_PARAMS_MACRO, MPLC_FB_TYPE)

#define MPLC_GENERATE_PROTOCOL(Class, LuaName, OLD_PARAMS_MACRO)                                                       \
    "#include <mplc/api/scada_protocol.h>";                                                                            \
    _MPLC_GENERATE_BASE(Class, LuaName, OLD_PARAMS_MACRO, MPLC_PROTOCOL_TYPE)
