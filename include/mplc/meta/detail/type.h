#pragma once

#include <typeinfo>
#include <type_traits>
#include <boost/type_traits/is_abstract.hpp>
#include <boost/type_traits/enable_if.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/decay.hpp>

#include "serialize.h"
#include "table.h"

#if defined(__GNUC__)
#    include <cxxabi.h>
#    include <cstdlib>
struct memfree {
    memfree(const char* n): name(n) {}
    ~memfree() {
        free((void*)name);
    }
    const char* name;
};
#endif

namespace mplc { namespace meta { namespace detail {

    template<class _Ty>
    using remove_pointer_t = typename boost::remove_pointer<_Ty>::type;
    template<class _Ty>
    using add_pointer_t = typename boost::add_pointer<_Ty>::type;

    /*template<class T>
    using IsSmall = boost::false_type;*/
    // typename std::integral_constant<bool, (sizeof(T) <= sizeof(void*))>;
    template<class T>
    using IsSmall = boost::conditional_t<sizeof(T) <= sizeof(void*), boost::true_type, boost::false_type>;

    template<class T, class = void>
    struct MoveFunc {
        static void call(void* const* src, void** dest) {}
    };
    template<class T>
    struct MoveFunc<T, boost::enable_if_t<IsSmall<T>::value && std::is_move_assignable<T>::value>> {
        static void call(void* const* src, void** dest) {
            **reinterpret_cast<T**>(dest) = **reinterpret_cast<T* const*>(src);
        }
    };
    template<class T, class = void>
    struct CloneFunc {
        static void call(void* const* src, void** dest) {}
    };
    template<class T>
    struct CloneFunc<T, boost::enable_if_t<IsSmall<T>::value && std::is_copy_constructible<T>::value>> {
        static void call(void* const* src, void** dest) {
            new (dest) T(*reinterpret_cast<T const*>(src));
        }
    };
    template<class T>
    struct CloneFunc<T, boost::enable_if_t<!IsSmall<T>::value && std::is_copy_constructible<T>::value>> {
        static void call(void* const* src, void** dest) {
            *dest = new T(**reinterpret_cast<T* const*>(src));
        }
    };

    template<class T, class = void>
    struct SerializeFunc {
        static void call(const void* src, msgpack::object::with_zone& dst) {
            Serializer<T>::call(*static_cast<const T*>(src), dst);
        }
    };
    template<>
    struct SerializeFunc<const char*> {
        static void call(const void* src, msgpack::object::with_zone& dst) {
            Serializer<const char*>::call(static_cast<const char*>(src), dst);
        }
    };
    template<class T, class = void>
    struct DeserializeFunc {
        static void call(const void* dst, const msgpack::object& src) {
            Deserializer<T>::call(*static_cast<T*>(const_cast<void*>(dst)), src);
        }
    };
    template<>
    struct DeserializeFunc<char*> {
        using type_call = void (*)(const void* dst, const msgpack::object& src);
        static constexpr type_call call{nullptr};
    };

    template<class T, class = void>
    struct ConstructFunc {
        static void call(void** dest) {
            new (*dest) T();
        }
    };
    template<class T>
    struct ConstructFunc<T, boost::enable_if_t<boost::is_abstract<T>::value>> {
        static void call(void** dest) {}
    };

    template<class T, class = void>
    struct CreateFunc {
        static void call(void** dest) {
            *dest = new T();
        }
    };
    template<class T>
    struct CreateFunc<T, boost::enable_if_t<IsSmall<T>::value>> {
        static void call(void** dest) {
            new (*dest) T();
        }
    };
    template<class T>
    struct CreateFunc<T, boost::enable_if_t<boost::is_abstract<T>::value>> {
        static void call(void** dest) {}
    };

    template<class T, class = void>
    struct TypeFuncs {
        static const char* name() {
#if defined(__GNUC__)
            static int status = -4;
            static memfree mem(abi::__cxa_demangle(typeid(T).name(), 0, 0, &status));
            return (status == 0) ? mem.name : typeid(T).name();
#else
            return typeid(T).name();
#endif
        }
        static void destroy(void** x) {
            // destruct and free memory
            delete (*reinterpret_cast<T**>(x));
        }
        static void destruct(void** x) {
            // destruct only, we'll reuse memory
            (*reinterpret_cast<T**>(x))->~T();
        }
    };
    template<typename T>
    struct TypeFuncs<T, boost::enable_if_t<IsSmall<T>::value>> {
        static const char* name() {
#if defined(__GNUC__)
            static int status = -4;
            static memfree mem(abi::__cxa_demangle(typeid(T).name(), 0, 0, &status));
            return (status == 0) ? mem.name : typeid(T).name();
#else
            return typeid(T).name();
#endif
        }
        static void destroy(void** x) {
            reinterpret_cast<T*>(x)->~T();
        }
        static void destruct(void** x) {
            reinterpret_cast<T*>(x)->~T();
        }
    };

    template<typename T>
    struct Type {
        using IsPointer = typename boost::is_pointer<T>::type;
        using Decay = boost::decay_t<T>;
        using PointerDecay = add_pointer_t<boost::decay_t<remove_pointer_t<Decay>>>;
        using Storage = boost::conditional_t<IsPointer::value, PointerDecay, Decay>;
        using Small = IsSmall<T>;  // typename std::integral_constant<bool, (sizeof(Storage) <= sizeof(void*))>;

        template<typename S = Small>
        inline static typename std::enable_if<S::value, void>::type clone(const Decay** src, void** dest) {
            new (dest) Storage(*reinterpret_cast<Storage const*>(*src));
        }

        template<typename S = Small>
        inline static typename std::enable_if<!S::value, void>::type clone(const Decay** src, void** dest) {
            *dest = new Storage(**src);
        }

        template<typename S = Small>
        inline static typename std::enable_if<S::value, Decay*>::type cast(void** object) {
            return const_cast<Decay*>(reinterpret_cast<Storage*>(object));
        }

        template<typename S = Small>
        inline static typename std::enable_if<!S::value, Decay*>::type cast(void** object) {
            return const_cast<Decay*>(reinterpret_cast<Storage*>(*object));
        }
    };

    template<class T, class = void>
    struct TypeInfo {
        using Storage = typename Type<T>::Storage;
        static const TypeTable& table() {
            static TypeTable tbl{
                TypeFuncs<Storage>::name,
                CreateFunc<Storage>::call /* TypeFuncs<Storage, Small>::create */,
                ConstructFunc<Storage>::call /* TypeFuncs<Storage, Small>::construct */,
                TypeFuncs<Storage>::destroy,
                TypeFuncs<Storage>::destruct,
                CloneFunc<Storage>::call,
                MoveFunc<Storage>::call,
                SerializeFunc<Storage>::call,
                DeserializeFunc<Storage>::call,
                sizeof(Storage),
                IsSmall<T>::value ? MetaType::Small : MetaType::Base,
            };
            return tbl;
        }
    };

}}}  // namespace mplc::meta::detail
