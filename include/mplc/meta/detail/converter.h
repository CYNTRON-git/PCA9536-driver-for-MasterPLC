#pragma once

#include <sstream>

namespace mplc { namespace meta { namespace detail {
    template<class U, class Y>
    struct Converter {
        inline static U convert(Any a) {
            return static_cast<U>(any_cast<Y>(a));
        }
    };

    template<class U>
    struct Converter<U, const char*> {
        inline static U convert(Any a) {
            U u;
            std::istringstream(any_cast<const char*>(a)) >> u;
            return u;
        }
    };
}}}  // namespace mplc::meta::detail
