#pragma once
#include <share/mplcshare.h>
#include <share/filetime.h>
#include <boost/type_traits/is_base_of.hpp>
#include "scada_fields.h"
#include "scada_api_macros.h"
#include "scada_obj.hpp"
#include "lua/type_traits.h"
#include <mplc/libs/containers.hpp>

class BaseLuaObj;
template<class T>
struct is_lua_type : boost::is_base_of<BaseLuaObj, T> {};

template<class T>
struct GetBaseLuaType<T, typename boost::enable_if<is_lua_type<T>>::type> {
    typedef ::SCADA_API::ScadaObj<T> LuaType;
};

template<typename, typename = void>
struct use_dynamic_fields : boost::false_type {};
template<typename T>
struct use_dynamic_fields<T, typename boost::make_void<typename T::MPLC_TMPL_MARKER_dyn_fields>::type>
    : boost::true_type {};

template<typename, typename = void>
struct has_inherited_fields : boost::false_type {};
template<typename T>
struct has_inherited_fields<T, typename boost::make_void<typename T::MPLC_TMPL_MARKER_inh_fields>::type>
    : boost::true_type {};

template<class T>
const char* get_lua_type(const T*);

template<class T>
const char* get_lua_type() {
    const T* dummy = nullptr;
    return get_lua_type(dummy);
}

template<class T, class Convert>
const char* get_lua_type() {
    const T* dummy = nullptr;
    return Convert::lua_type(dummy);
}

// ---- Get string of Lua Type from Cpp Type
template<class T>
const char* get_lua_type(const T*);
// ---- Get string of Lua Type from Cpp Type

template<>
inline boost::enable_if_c<!boost::is_same<unsigned long, uint32_t>::value, const char*>::type
    get_lua_type<unsigned long>() {
    return get_lua_type<uint32_t>();
}

namespace SCADA_API {
    template<class T>
    const char* get_base_lua_type(const T*);
}  // namespace SCADA_API

template<class T, class Enable = void>
struct LuaTypeName {
    static const char* get(const T* dummy) {
        return "";
    }
};

template<class T>
struct LuaTypeName<T, typename boost::enable_if<boost::is_enum<T>>::type> {
    static const char* get(const T*) {
        return get_lua_type<int32_t>();
    }
};

template<class T>
struct LuaTypeName<T, typename boost::enable_if<is_simple_wrapper<T>>::type> {
    static const char* get(const T*) {
        return get_lua_type<typename T::lua_type>();
    }
};
template<class T>
struct LuaTypeName<T, typename boost::enable_if<is_lua_type<T>>::type> {
    typedef ::SCADA_API::ScadaObj<T> LuaType;
    static const char* get(const T*) {
        return "nil";
        // LuaType::LuaFullName();
    }
};
template<class T>
struct LuaTypeName<T, typename boost::enable_if<is_iterable<T>>::type> {
    static const char* get(const T*) {
        static const std::string res = std::string("ArrayMetadata({oftype = ") +
                                       get_lua_type<typename T::value_type>() +
                                       ", subranges = { {Begin = 0, End = 0, IsDynamic = true}}})";
        return res.c_str();
    }
};
template<class T>
struct LuaTypeName<T, typename boost::enable_if<is_self_get_set<T>>::type> {
    static const char* get(const T*) {
        return T::lua_type();
    }
};
template<class T>
struct LuaTypeName<T, typename boost::enable_if<is_lua_proxy_t<T>>::type> {
    static const char* get(const T*) {
        return T::lua_type();
    }
};
template<class T>
const char* get_lua_type(const T* dummy) {
    return LuaTypeName<T>::get(dummy);
}

struct AsUtf8 {
    // static void get(std::string& value, lua_State* L) { get_lua_u8string(value, L); }
    static void get(std::string& value, lua_State* L) {
        get_lua_value(value, L);
    }
    // static void set(const std::string& value, lua_State* L) { set_lua_u8string(value.c_str(), value.size(), L); }
    static void set(const std::string& value, lua_State* L) {
        set_lua_value(value, L);
    }
    static void get(TemplateString& value, lua_State* L) {
        get(value.m_str_tmpl, L);
    }
    static void set(const TemplateString& value, lua_State* L) {
        set(value.m_str_tmpl, L);
    }
    static const char* lua_type(const void*) {
        return get_lua_type<std::string>();
    }
};
namespace mplc {

    inline void set_json_data(JsonWrapper& json, const RefTo& val) {
        if (val.raw.empty()) {
            json.value.SetString(val.to_string(), json.allocator);
        } else {
            json.value.SetString(val.raw, json.allocator);
        }
    }
    inline void set_json_data(JsonWrapper& json, const vm::ObjectID& val) {
        if (!val.path.empty()) {
            json.value.SetString(val.to_string(), json.allocator);
        } else {
            json.value.SetInt64(val.id);
        }
    }
    template<class T>
    typename boost::enable_if<is_lua_type<T>, void>::type set_json_data(JsonWrapper& json, const T& val) {
        SCADA_API::ScadaObj<T>::to_json(json, val);
    }
    template<class T>
    typename boost::enable_if<is_lua_proxy_t<T>, void>::type set_json_data(mplc::JsonWrapper& json, const T& v) {
        v.to_json(json);
    }
}  // namespace mplc

namespace SCADA_API {
    template<class T>
    struct system_param final : mplc::lua_proxy_t {
        // typedef int self_get_set;
        // typedef int is_disable_json;
        FileTime SourceTime;
        OpcUa_StatusCode StatusCode;
        T Value;
        system_param(): StatusCode(0), Value(T()) {}
        void lua_set(lua_State* L) const override {
            static const mplc::lib::string_view f_val(SYSTEM_PARAM_VALUE_FIELD);
            static const mplc::lib::string_view f_time(SYSTEM_PARAM_TIME_FIELD);
            static const mplc::lib::string_view f_sc(SYSTEM_PARAM_QUALITY_FIELD);
            lua_createtable(L, 0, 3);
            set_lua_value_kv(f_val, Value, L);
            set_lua_value_kv(f_time, SourceTime, L);
            set_lua_value_kv(f_sc, StatusCode, L);
        }
        void lua_get(lua_State* L) override {
            if (::lua_type(L, -1) != LUA_TTABLE)
                return;
            lua_getfield(L, -1, SYSTEM_PARAM_VALUE_FIELD);
            get_lua_value(Value, L);
            lua_pop(L, 1);

            lua_getfield(L, -1, SYSTEM_PARAM_TIME_FIELD);
            get_lua_value(SourceTime, L);
            lua_pop(L, 1);

            lua_getfield(L, -1, SYSTEM_PARAM_QUALITY_FIELD);
            get_lua_value(StatusCode, L);

            lua_pop(L, 1);
        }
        void to_json(mplc::JsonWrapper& json) const override {}
        static const char* lua_type() {
            return get_base_lua_type<system_param<T>>(nullptr);
        }
    };
    MPLC_GENERATE_SCADA_API(GetLuaType, EnumType, FromEnum);

    template<class T>
    struct HasDynamicFields<T, typename boost::enable_if<use_dynamic_fields<T>>::type> {
        static void bind() {
            T::EnableDynamicFields(&ScadaObj<T>::fields);
        }
    };

    namespace detail {
        template<class T>
        const T* dummy_v() {
            return nullptr;
        }
        template<typename, typename = void>
        struct has_parent : boost::false_type {};
        template<typename T>
        struct has_parent<T, typename boost::make_void<typename T::__parent>::type> : boost::true_type {};

        template<class Child, class Parent>
        struct BindFields<Child, Parent, typename boost::enable_if<has_parent<Parent>>::type> {
            static void bind(boost::true_type) {
                Parent::template BindFields<Child>();
            }
            static void bind(boost::false_type) {}
            static void bind() {
                BindFields<Child, typename Parent::__parent::next>::bind();
                Parent::__parent::type::template BindFields<Child>();
                bind(boost::is_same<Parent, Child>());
            }
        };

        template<typename, typename = void>
        struct is_lockable : boost::false_type {};
        template<typename T>
        struct is_lockable<T, typename boost::make_void<typename T::MPLC_TMPL_MARKER_lockable>::type>
            : boost::true_type {};

        template<class T, class R = void>
        struct ObjMutex {
            static void Lock(const T*) {}
            static void Unlock(const T*) {}
        };
        template<class T>
        struct ObjMutex<T, typename boost::enable_if<is_lockable<T>>::type> {
            static void Lock(const T* obj) {
                obj->Lock();
            }
            static void Unlock(const T* obj) {
                obj->Unlock();
            }
        };

        template<class T>
        struct ObjLockGuard {
            const T* obj;
            ObjLockGuard(const T* obj): obj(obj) {
                ObjMutex<T>::Lock(obj);
            }
            ~ObjLockGuard() {
                ObjMutex<T>::Unlock(obj);
            }
        };

        template<class T, class R = void>
        struct CLuaTypes {
            static void add() {}
        };
        template<class T>
        struct CLuaTypes<T, typename boost::enable_if<is_lua_type<T>>::type> {
            static void add() {
                T::RegAsLuaType();
            }
        };
        template<class T>
        struct CLuaTypes<T, typename boost::enable_if<is_iterable<T>>::type> {
            static void add() {
                CLuaTypes<typename T::value_type>::add();
            }
        };
        template<class T>
        struct CLuaTypes<T, typename boost::enable_if<is_pair<T>>::type> {
            static void add() {
                CLuaTypes<typename T::first_type>::add();
                CLuaTypes<typename T::second_type>::add();
            }
        };
    }  // namespace detail
    template<EnumType EType>
    struct LuaData {
        template<class T>
        static void get(std::vector<T>& value, lua_State* L) {
            int len = luaL_getn(L, -1);
            value.resize(len);
            for (int i = 1; i <= len; i++) {
                lua_rawgeti(L, -1, i);
                LuaData<EType>::get(value[i - 1], L);
                lua_pop(L, 1);
            }
        }
        template<class T>
        static void set(const std::vector<T>& value, lua_State* L) {
            lua_createtable(L, 0, value.size());
            for (int i = 0; i < value.size(); i++) {
                lua_pushnumber(L, i + 1); /* Push the table index */
                LuaData<EType>::set(value[i], L);
                lua_rawset(L, -3); /* Stores the pair in the table */
            }
        }

        template<class T>
        static void get(T& value, lua_State* L) {
            typename FromEnum<EType>::type tmp;
            get_lua_value(tmp, L);
            value = tmp;
        }
        template<class T>
        static void set(const T& value, lua_State* L) {
            typename FromEnum<EType>::type tmp = value;
            set_lua_value(tmp, L);
        }
        template<class T>
        static const char* lua_type(const T*) {
            return GetLuaType(EType);
        }
        template<class T>
        static const char* lua_type(const std::vector<T>*) {
            const T* dummy = nullptr;
            static const std::string res = std::string("ArrayMetadata({oftype = ") + lua_type(dummy) +
                                           ", subranges = { {Begin = 0, End = 0, IsDynamic = true}}})";
            return res.c_str();
        }
    };

    template<class ScadaType, class FieldType>
    struct BaseField : ScadaFields::IField {
        FieldType ScadaType::*param;
        BaseField(FieldType ScadaType::*field, const char* name, bool retain): IField(name, retain), param(field) {}
        void set(const void* ptr, lua_State* L) const override {
            const ScadaType* obj = static_cast<const ScadaType*>(ptr);
            set_lua_value(obj->*param, L);
        }
        void get(void* ptr, lua_State* L) const override {
            ScadaType* obj = static_cast<ScadaType*>(ptr);
            get_lua_value(obj->*param, L);
        }
        void to_json(mplc::JsonWrapper& json, const void* ptr) const override {
            const ScadaType* obj = static_cast<const ScadaType*>(ptr);
            mplc::set_json_data(json, obj->*param);
        }
        virtual const char* lua_type() const {
            const FieldType* dummy = nullptr;
            return get_lua_type(dummy);
        }
    };

    template<class ScadaType, class FieldType, class Converter>
    struct ConvTypeField : ScadaFields::IField {
        FieldType ScadaType::*param;
        ConvTypeField(FieldType ScadaType::*field, const char* name, bool retain): IField(name, retain), param(field) {}
        void set(const void* ptr, lua_State* L) const {
            const ScadaType* obj = static_cast<const ScadaType*>(ptr);
            Converter::set(obj->*param, L);
        }
        void get(void* ptr, lua_State* L) const {
            ScadaType* obj = static_cast<ScadaType*>(ptr);
            Converter::get(obj->*param, L);
        }
        void to_json(mplc::JsonWrapper& json, const void* ptr) const override {
            const ScadaType* obj = static_cast<const ScadaType*>(ptr);
            mplc::set_json_data(json, obj->*param);
        }
        virtual const char* lua_type() const {
            const FieldType* dummy = nullptr;
            return Converter::lua_type(dummy);
        }
    };
    template<class ScadaType, class FieldType, EnumType type>
    struct FakeTypeField : ScadaFields::IField {
        FieldType ScadaType::*param;
        FakeTypeField(FieldType ScadaType::*field, const char* name, bool retain): IField(name, retain), param(field) {}
        void set(const void* ptr, lua_State* L) const {
            const ScadaType* obj = static_cast<const ScadaType*>(ptr);
            LuaData<type>::set(obj->*param, L);
        }
        void get(void* ptr, lua_State* L) const {
            ScadaType* obj = static_cast<ScadaType*>(ptr);
            LuaData<type>::get(obj->*param, L);
        }
        void to_json(mplc::JsonWrapper& json, const void* ptr) const override {
            const ScadaType* obj = static_cast<const ScadaType*>(ptr);
            typename FromEnum<type>::type tmp = obj->*param;
            mplc::set_json_data(json, tmp);
        }

        virtual const char* lua_type() const {
            const FieldType* dummy = nullptr;
            return LuaData<type>::lua_type(dummy);
        }
    };
    template<class Type>
    void RegNewLuaType() {
        detail::CLuaTypes<Type>::add();
    }

    template<class ScadaType, class FieldType>
    void bind_field(const char* name, FieldType ScadaType::*param, bool retain = false) {
        RegNewLuaType<FieldType>();
        ScadaObj<ScadaType>::fields.add(new BaseField<ScadaType, FieldType>(param, name, retain));
    }
    template<class ScadaType, class FieldType, EnumType type>
    void bind_field(const char* name, FieldType ScadaType::*param, bool retain = false) {
        RegNewLuaType<FieldType>();
        ScadaObj<ScadaType>::fields.add(new FakeTypeField<ScadaType, FieldType, type>(param, name, retain));
    }
    template<class ScadaType, class FieldType, class Converter>
    void bind_field(const char* name, FieldType ScadaType::*param, bool retain = false) {
        RegNewLuaType<FieldType>();
        ScadaObj<ScadaType>::fields.add(new ConvTypeField<ScadaType, FieldType, Converter>(param, name, retain));
    }
    MPLC_GENERATE_BASE_LUA_TYPES(get_base_lua_type)
    template<class T>
    const char* get_base_lua_type(const T*) {
        BOOST_STATIC_ASSERT_MSG(sizeof(T) != 0, "get_lua_type not defined for this type");
        return "";
    }
}  // namespace SCADA_API

template<>
inline const char* get_lua_type<std::string>(const std::string*) {
    return SCADA_API::get_base_lua_type<std::string>(nullptr);
}
