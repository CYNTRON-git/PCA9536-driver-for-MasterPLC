#pragma once

//#include "type.h"
#include "detail/type.h"

namespace mplc { namespace meta {

    struct MPLCSHARE_API Arg {
        template<class T>
        Arg(const T& data): data(static_cast<const void*>(&data)), type(&detail::TypeInfo<T>::table()) {}

        Arg(const void* data, const detail::TypeTable* type): data(data), type(type) {}

        Arg(): data(nullptr), type(nullptr) {}
        bool valid() const {
            return data != nullptr && type != nullptr;
        }
        void serialize(msgpack::object::with_zone& oz) const {
            if (type && data && type->serialize) {
                type->serialize(data, oz);
            }
        }
        void deserialize(const msgpack::object& o) const {
            if (type && data && type->deserialize) {
                type->deserialize(data, o);
            }
        }
        /*Type type() {
            return _type;
        }
        const void* data() {
            return _data;
        }

    private:*/
        const void* data;
        const detail::TypeTable* type;
    };
}}  // namespace mplc::meta
