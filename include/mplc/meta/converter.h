#pragma once

#include "any.h"
#include "detail/converter.h"

namespace mplc { namespace meta {
    template<class T>
    struct Converter {
        static Any convert(const Any& a) {
            return any_cast<T>(a);
        }

        static bool canConvert(const Any& a) {
            return Type::from<T>() == a.type();
        }
    };

    template<>
    struct Converter<int> {
        static Any convert(const Any& a);
        static bool canConvert(const Any& a);
    };

    template<>
    struct Converter<float> {
        static Any convert(const Any& a);
        static bool canConvert(const Any& a);
    };
}}  // namespace mplc::meta
