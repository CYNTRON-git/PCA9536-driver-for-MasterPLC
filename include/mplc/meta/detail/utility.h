#pragma once
#include "../meta_config.h"

namespace mplc { namespace meta { namespace tag {
    struct object_t {};
    struct array_t {};
}}}  // namespace mplc::meta::tag
namespace mplc { namespace meta { namespace detail {

    template<class T, std::size_t N>
    constexpr std::size_t TableSize(const T (&array)[N]) noexcept {
        return N;
    }

    // Callable traits
    template<typename T>
    struct is_callable_impl {
    private:
        typedef char (&yes)[1];
        typedef char (&no)[2];

        struct Fallback {
            void operator()();
        };
        struct Derived : T, Fallback {};

        template<typename U, U>
        struct Check;

        template<typename>
        static yes test(...);

        template<typename C>
        static no test(Check<void (Fallback::*)(), &C::operator()>*);

    public:
        static const bool value = sizeof(test<Derived>(0)) == sizeof(yes);
    };

    template<typename T>
    struct is_method : boost::conditional<boost::is_member_function_pointer<T>::value || boost::is_function<T>::value,
                                          boost::true_type,
                                          boost::false_type>::type {};

    template<typename T>
    struct is_callable : boost::conditional<boost::is_class<T>::value, is_callable_impl<T>, is_method<T>>::type {};
    template<typename T>
    using raw_type_t = boost::remove_const_t<boost::remove_reference_t<T>>;
    // Helper stuff
    namespace unpack {
        template<unsigned...>
        struct indices {};

        template<unsigned N, unsigned... Is>
        struct indices_gen : indices_gen<N - 1, N - 1, Is...> {};

        template<unsigned... Is>
        struct indices_gen<0, Is...> : indices<Is...> {};

        template<unsigned N, typename... R>
        struct type_at;

        template<unsigned N, typename T, typename... R>
        struct type_at<N, T, R...> {
            using type = typename type_at<N - 1, R...>::type;
        };

        template<typename T, typename... R>
        struct type_at<0, T, R...> {
            using type = T;
        };

        template<unsigned N>
        struct type_at<N> {
            using type = void;
        };
    }  // namespace unpack

    template<typename T>
    struct has_table {
        template<class C>
        static boost::true_type test(decltype(C::table)*);
        template<class C>
        static boost::false_type test(...);
        using value = decltype(test<T>(0));
    };
    template<class T>
    using has_table_t = typename has_table<T>::value;
    template<class T, class = void>
    struct table {
        constexpr static decltype(nullptr) ptr() {
            return nullptr;
        }

        constexpr static unsigned int size() {
            return 0;
        }
    };
    template<class T>
    struct table<T, boost::enable_if_t<has_table_t<T>::value>> {
        static auto ptr() -> boost::remove_extent_t<decltype(T::table)>* {
            return T::table;
        }

        static unsigned int size() {
            return TableSize(T::table);
        }
    };

    template<typename, typename = void>
    struct meta_type : boost::false_type {
        using type = void;
    };
    template<typename T>
    struct meta_type<T, boost::void_t<typename T::_meta_type>> : boost::true_type {
        using type = typename T::_meta_type;
    };
    template<class T>
    using meta_type_t = typename meta_type<T>::type;
    template<typename, typename = void>
    struct is_object : boost::false_type {};
    template<typename T>
    struct is_object<T, boost::enable_if_t<boost::is_same<tag::object_t, meta_type_t<T>>::value>> : boost::true_type {};
    template<class T>
    using is_object_t = typename is_object<T>::type;
    
    template<typename, typename = void>
    struct is_array : boost::false_type {};
    template<typename T>
    struct is_array<T, boost::enable_if_t<boost::is_same<tag::array_t, meta_type_t<T>>::value>> : boost::true_type {};
    template<class T>
    using is_array_t = typename is_array<T>::type;

}}}  // namespace mplc::meta::detail
