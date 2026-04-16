#pragma once
#include <boost/type_traits/make_void.hpp>
#include "utility.h"

namespace mplc { namespace meta { namespace detail {

    template<class T, class N>
    struct TypeList {
        using type = T;
        using next = N;
    };

    template<class T, class = void>
    struct AddType {
        using list = TypeList<T, void>;
    };

    template<class T>
    struct AddType<T, boost::void_t<typename T::_type_list::next>> {
        using list = TypeList<T, typename T::_type_list::type>;
    };
    template<class T, class = void>
    struct ParentApi {
        using type = void;
        static nullptr_t table() {
            return nullptr;
        }
    };

}}}  // namespace mplc::meta::detail
