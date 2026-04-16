#pragma once

#define AddBaseEnumFunctions(Class, Enum, DefValue)                                                                    \
private:                                                                                                               \
    Class::Enum m_enum;                                                                                                \
                                                                                                                       \
public:                                                                                                                \
    Class(): m_enum(DefValue) {}                                                                                       \
    Class(Class::Enum _enum): m_enum(_enum) {}                                                                         \
    Class(const Class& copy) {                                                                                         \
        m_enum = copy.m_enum;                                                                                          \
    }                                                                                                                  \
    Class(int id): m_enum(static_cast<Class::Enum>(id)) {}                                                             \
    operator Class::Enum() const {                                                                                     \
        return m_enum;                                                                                                 \
    }                                                                                                                  \
    Class operator=(const Class& copy) {                                                                               \
        m_enum = copy.m_enum;                                                                                          \
        return *this;                                                                                                  \
    }                                                                                                                  \
    static Class::Enum fromInt(int id) {                                                                               \
        return static_cast<Class::Enum>(id);                                                                           \
    }

/* DEFINES FOR ADD LIB IN WINDOWS */
//#if defined(_M_X64)
//#    define CONF_PLATFORM_PREFIX "x64/"
//#else
//#    define CONF_PLATFORM_PREFIX ""
//#endif

#if defined(PLC_WINPAC) || defined(PLC_WINPAC_9000)
#    define DLLENTRY WINAPI
#    define DLLHANDLE HANDLE
#else
#    define DLLENTRY APIENTRY
#    define DLLHANDLE HMODULE
#endif

#define ADD_PTR_TYPEDEF(Class)                                                                                         \
    typedef boost::shared_ptr<Class> ptr;                                                                              \
    typedef boost::weak_ptr<Class> weak_ptr;

#define ADD_MAKE_PTR(Class)                                                                                            \
    ADD_PTR_TYPEDEF(Class)                                                                                             \
    static ptr make() {                                                                                                \
        return boost::make_shared<Class>();                                                                            \
    }

#define AS_SINGLTON(Class)                                                                                             \
private:                                                                                                               \
    Class::Class();                                                                                                    \
    Class::Class(const Class& obj) = delete;                                                                           \
    Class::Class(Class&& obj) = delete;                                                                                \
    Class& operator=(const Class& obj) = delete;                                                                       \
    Class& operator=(Class&& obj) = delete;
//////////////////////////////////////
///
#define MPLC_DEBUG_TYPE(Type)                                                                                          \
    do {                                                                                                               \
        const std::type_info& ti2 = typeid(Type);                                                                      \
        const char* name = ti2.name();                                                                                 \
        int size = sizeof(Type);                                                                                       \
        PRINTLN(#Type ":{name: '%s', size: %d}", ti2.name(), size);                                                    \
    } while (0)

#ifdef _WIN32
#    define MPLC_DEPRECATED(Msg) [[deprecated(Msg)]]
#else

#    define MPLC_DEPRECATED(Msg) [[deprecated(Msg)]]
#endif
