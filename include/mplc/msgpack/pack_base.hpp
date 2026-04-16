#pragma once
#include <mplc/libs/string_view.hpp>
#include <msgpack/object.hpp>
#include <msgpack/iterator.hpp>
#include <mplc/date_time.h>
#include <mplc/time_span.h>

#define MP_EXT_TYPE(Type, Id)                                                                                          \
    template<>                                                                                                         \
    struct type_id<Type> {                                                                                             \
        static constexpr int8_t value = Id;                                                                            \
    };                                                                                                                 \
                                                                                                                       \
    template<>                                                                                                         \
    struct type_from<(ExtType)type_id<Type>::value> {                                                                  \
        typedef Type type;                                                                                             \
    }
#define MP_EXT_TYPE_ENUM(Type) Type = type_id<Type>::value

namespace msgpack {
    inline bool operator==(const msgpack::object_str& lv, mplc::lib::string_view rv) {
        return mplc::lib::string_view(lv.ptr, lv.size) == rv;
    }

    inline int find_key(mplc::lib::string_view key, const msgpack::object_map& map) {
        int pos = 0;
        for (auto&& kv: map) {
            if (kv.key.type == type::STR && kv.key.via.str == key) {
                return pos;
            }
            pos++;
        }
        return -1;
    }
    inline void update(object_handle& oh, const object& val, size_t chank_size = 1024, bool clear = true) {
        switch (val.type) {
        case type::NIL:
        case type::BOOLEAN:
        case type::POSITIVE_INTEGER:
        case type::NEGATIVE_INTEGER:
        case type::FLOAT32:
        case type::FLOAT64:
            // oh.zone().reset();
            oh.set(val);
            if (clear) {
                oh.zone().reset();
            }
            return;
        case type::EXT:
            if (!oh.zone()) {
                oh.zone().reset(new zone(val.via.ext.size));
            }
        case type::BIN:
        case type::STR:
        case type::ARRAY:
        case type::MAP:
            if (!oh.zone()) {
                oh.zone().reset(new zone(chank_size));
            }
        }
        if (clear) {
            oh.zone()->clear();
        }
        object::with_zone o(*oh.zone());
        o << val;
        oh.set(o);
    }

    enum class ExtType : int8_t;
    template<class T>
    struct type_id;

    template<ExtType _Type>
    struct type_from;

    template<class T>
    constexpr int8_t type_id_v() {
        return type_id<T>::value;
    }
    template<class T>
    constexpr ExtType type_id_enum() {
        return (ExtType)type_id<T>::value;
    }
    using mplc::DateTime;
    using mplc::TimeSpan;

    MP_EXT_TYPE(void, 0);
    MP_EXT_TYPE(DateTime, 1);
    MP_EXT_TYPE(TimeSpan, 2);
    // MP_EXT_TYPE(OpcUa_Variant, 100);

    enum class ExtType : int8_t {
        StdTimePoint = -1,
        None = type_id<void>::value,
        Void = type_id<void>::value,
        MP_EXT_TYPE_ENUM(DateTime),
        MP_EXT_TYPE_ENUM(TimeSpan),
        // MP_EXT_TYPE_ENUM(OpcUa_Variant),
    };

}  // namespace msgpack

#undef MP_EXT_TYPE
#undef MP_EXT_TYPE_ENUM
