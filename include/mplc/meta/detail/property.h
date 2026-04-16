#pragma once
#include "type.h"
#include "method.h"
namespace mplc { namespace meta { namespace detail {
    template<typename T, typename C>
    struct FieldBase {
        using Storage = T;
        using Class = C;
    };

    template<typename T>
    struct Field;

    template<typename T, typename C>
    struct Field<T(C::*)> : public FieldBase<T, C> {};

    template<typename Member, Member field>
    struct Accessor {
        using Class = typename Field<Member>::Class;
        using Field = typename Field<Member>::Storage;
        using Storage = typename Type<Field>::Storage;

        inline static void get(const void* obj, const void* ret) {
            *(Storage*)ret = static_cast<Class*>(const_cast<void*>(obj))->*field;
        }

        inline static void set(const void* obj, const void* ret) {
            static_cast<Class*>(const_cast<void*>(obj))->*field = *(Storage*)ret;
        }
        inline static void deserialize(const void* dst, const msgpack::object& src) {
            Deserializer<Storage>::call(static_cast<Class*>(const_cast<void*>(dst))->*field, src);
        }
        inline static void serialize(const void* src, msgpack::object::with_zone& dst) {
            Serializer<Storage>::call(static_cast<Class*>(const_cast<void*>(src))->*field, dst);
        }
        inline static void* ptr(const void* obj) {
            return &(static_cast<Class*>(const_cast<void*>(obj))->*field);
        }
    };

    template<typename Method, Method method>
    struct Getter {
        using Args = MethodArguments<Method>;
        using Return = typename Args::Return;
        using Class = typename Args::Class;
        using Storage = typename Type<Return>::Storage;

        static_assert(Args::count == 0, "Getter method should not receive arguments");

        inline static void get(const void* obj, const void* ret) {
            *(Storage*)ret = (static_cast<Class*>(const_cast<void*>(obj))->*method)();
        }
        inline static void serialize(const void* src, msgpack::object::with_zone& dst) {
            Serializer<Storage>::call((static_cast<Class*>(const_cast<void*>(src))->*method)(), dst);
        }
    };

    template<typename Method, Method method>
    struct Setter {
        using Args = MethodArguments<Method>;
        using Class = typename Args::Class;
        using Storage = raw_type_t<typename Args::SetArg>;

        static_assert(Args::count == 1, "Setter method should have one argument");

        inline static void set(const void* obj, const void* val) {
            (static_cast<Class*>(const_cast<void*>(obj))->*method)(*(Storage*)val);
        }
        inline static void deserialize(const void* dst, const msgpack::object& src) {
            Storage tmp{};
            Deserializer<Storage>::call(tmp, src);
            (static_cast<Class*>(const_cast<void*>(dst))->*method)(tmp);
        }
    };

    template<typename Member, Member member>
    struct Property {
        using IsMethod = typename std::is_member_function_pointer<Member>::type;
        using IsField = typename std::is_member_object_pointer<Member>::type;

        static_assert(!std::is_same<IsField, IsMethod>::value, "Property should be field or method");

        struct Get {
            using ReadMethod = Getter<Member, member>;
            using MemberField = Accessor<Member, member>;
            using Read = typename std::conditional<IsMethod::value, ReadMethod, MemberField>::type;
            using Storage = typename Read::Storage;

            inline static const TypeTable* type() {
                return &TypeInfo<Storage>::table();
            }

            inline static void get(const void* obj, const void* ret) {
                Read::get(obj, ret);
            }

            inline static void serialize(const void* src, msgpack::object::with_zone& dst) {
                Read::serialize(src, dst);
            }
        };

        struct Set {
            using WriteMethod = Setter<Member, member>;
            using MemberField = Accessor<Member, member>;
            using Write = typename std::conditional<IsMethod::value, WriteMethod, MemberField>::type;
            using Storage = typename Write::Storage;

            inline static const TypeTable* type() {
                return &TypeInfo<Storage>::table();
            }

            inline static void set(const void* obj, const void* ret) {
                Write::set(obj, ret);
            }

            inline static void deserialize(const void* dst, const msgpack::object& src) {
                Write::deserialize(dst, src);
            }
        };
        struct None {
            inline static void* ptr(const void* obj) {
                return nullptr;
            }
        };
        using Ptr = typename std::conditional<IsField::value, Accessor<Member, member>, None>::type;
        inline static void* ptr(const void* obj) {
            return Ptr::ptr(obj);
        }
    };
}}}  // namespace mplc::meta::detail
