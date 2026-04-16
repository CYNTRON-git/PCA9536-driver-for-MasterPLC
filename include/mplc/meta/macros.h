#pragma once
// clang-format off
#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/stringize.hpp>
#include "macros_fwd.h"

#define META_PP_DECLARE(Member, Class) \
namespace mplc { namespace meta { \
    template<> \
    struct Member::Holder<Class> { \
        using UClass = Class; \
        static const Table table[]; \
    }; \
}} \
const mplc::meta::Member::Table mplc::meta::Member::Holder<Class>::table[]

#define META_PP_DECLARE_PUBLIC_API(Name, Class) \
const mplc::meta::detail::ObjectTable& mplc::meta::Object::Holder<Class>::table() { \
    static const mplc::meta::detail::ObjectTable _table = { \
        detail::TypeFuncs<Storage>::name, \
        detail::CreateFunc<Storage>::call, \
        detail::ConstructFunc<Storage>::call, \
        detail::TypeFuncs<Storage>::destroy, \
        detail::TypeFuncs<Storage>::destruct, \
        detail::CloneFunc<Storage>::call, \
        detail::MoveFunc<Storage>::call, \
        detail::SerializeFunc<Storage>::call, \
        detail::DeserializeFunc<Storage>::call, \
        sizeof(Storage), \
        BOOST_PP_STRINGIZE(Name), \
        detail::ParentApi<Storage>::table(), \
        Properties<ClassT>::ptr(), \
        Properties<ClassT>::size(), \
    }; \
    return _table; \
}


#define META_PP_DECLARE_API(Name, Class) \
    META_PP_DEFINE_PUBLIC_API(PP_EMPTY(), Class); \
    META_PP_DECLARE_PUBLIC_API(Name, Class)

#define META_PP_METHOD(name, method) \
{ \
    #name, { \
	    #method, \
	    &mplc::meta::detail::MethodCall<decltype(&UClass::method), &UClass::method>::call, \
	    mplc::meta::detail::MethodArguments<decltype(&UClass::method)>::count, \
	    mplc::meta::detail::MethodArguments<decltype(&UClass::method)>::types(), \
    } \
}

#define META_PP_OVERLOAD(method, signature) \
{ \
	#method, \
	&mplc::meta::detail::MethodCall<signature, &UClass::method>::call, \
	mplc::meta::detail::MethodArguments<signature>::count, \
	mplc::meta::detail::MethodArguments<signature>::types() \
}

#define META_PP_FUNCTION(function) \
{ \
    #function, \
    &mplc::meta::detail::MethodCall<decltype(&function), &function>::call, \
    mplc::meta::detail::MethodArguments<decltype(&function)>::count, \
    mplc::meta::detail::MethodArguments<decltype(&function)>::types() \
}

#define META_PP_CONSTRUCTOR(...) \
{ \
    mplc::meta::Object::Holder<UClass>::UApi::table.name, \
    &mplc::meta::detail::ConstructorCall<UClass, __VA_ARGS__>::call, \
    mplc::meta::detail::MethodArguments<decltype(&Constructor<UClass, __VA_ARGS__>::call)>::count, \
    mplc::meta::detail::MethodArguments<decltype(&Constructor<UClass, __VA_ARGS__>::call)>::types() \
}
#define META_PP_PROPERTY_TYPE(member) \
    mplc::meta::detail::Property<decltype(&UClass::member), &UClass::member>::Get::type(), \
    &mplc::meta::detail::Property<decltype(&UClass::member), &UClass::member>::ptr

#define META_PP_MEMBER(member) \
    META_PP_READ(member), \
    META_PP_WRITE(member)

#define META_PP_READ_ONLY(member) \
    META_PP_PROPERTY_TYPE(member), \
    &mplc::meta::detail::Property<decltype(&UClass::member), &UClass::member>::Get::get, \
    &mplc::meta::detail::Property<decltype(&UClass::member), &UClass::member>::Get::serialize, \
    nullptr, nullptr

#define META_PP_WRITE_ONLY(member) \
    META_PP_PROPERTY_TYPE(member), \
    nullptr, nullptr, \
    &mplc::meta::detail::Property<decltype(&UClass::member), &UClass::member>::Set::set, \
    &mplc::meta::detail::Property<decltype(&UClass::member), &UClass::member>::Set::deserialize

#define META_PP_READ(member) \
    META_PP_PROPERTY_TYPE(member), \
    &mplc::meta::detail::Property<decltype(&UClass::member), &UClass::member>::Get::get, \
    &mplc::meta::detail::Property<decltype(&UClass::member), &UClass::member>::Get::serialize
#define META_PP_WRITE(member) \
    &mplc::meta::detail::Property<decltype(&UClass::member), &UClass::member>::Set::set, \
    &mplc::meta::detail::Property<decltype(&UClass::member), &UClass::member>::Set::deserialize

#define META_PP_PROPERTY(name, ...) \
{ \
	#name, \
	__VA_ARGS__, \
}
