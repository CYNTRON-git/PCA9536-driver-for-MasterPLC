#pragma once

#define META_PP_PUSH_TYPE_LIST(Class)                                                                                  \
    using _type_list = ::mplc::meta::detail::AddType<Class>::list;                                                     \
    using _meta_type = ::mplc::meta::tag::object_t

#define META_PP_OBJECT(Class)                                                                                          \
    META_PP_PUSH_TYPE_LIST(Class);                                                                                     \
    static ::mplc::meta::Object classApi();                                                                            \
    friend ::mplc::meta::Property::Holder<Class>

#define META_PP_DEFINE_PUBLIC_API(ExportMacros, Class)                                                                 \
    inline mplc::meta::Object Class::classApi() {                                                                      \
        return ::mplc::meta::Object::from<Class>();                                                                    \
    }                                                                                                                  \
    template<>                                                                                                         \
    struct mplc::meta::Object::Holder<Class> {                                                                         \
        using ClassT = Class;                                                                                          \
        using Storage = typename detail::Type<ClassT>::Storage;                                                        \
        template<class T>                                                                                              \
        using Properties = detail::table<meta::Property::Holder<T>>;                                                   \
        ExportMacros static const detail::ObjectTable& table();                                                        \
    }
