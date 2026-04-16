#pragma once
#include "detail/array.h"
#include "arg.h"

namespace mplc { namespace meta {

    class MPLCSHARE_API Array {
        using Table = detail::ArrayTable;
        const Table* _table;
        Array(const Table* table): _table(table) {}

        friend class Type;

    public:
        void deserialize(const void* obj, const msgpack::object& o) const;
        void serialize(const void* obj, msgpack::object::with_zone& oz) const;
        bool valid() const;
        Arg get(const void* obj, size_t pos) const;
        const void* ptr(const void* obj, size_t pos) const;
        size_t size(const void* obj) const;
        template<class T>
        static Array from() {
            static_assert(detail::has_array_adapter<T>::value, "Specialization meta::adapter::Array<T> not found");
            return {&detail::TypeInfo<T>::table()};
        }
    };
}}  // namespace mplc::meta
template<class T>
struct msgpack::adaptor::object_with_zone<T, boost::enable_if_t<mplc::meta::detail::has_array_adapter<T>::value>> {
    void operator()(msgpack::object::with_zone& oz, const T& v) const {
        using namespace mplc;
        return meta::Array::from<T>().serialize(&v, oz);
    }
};
template<class T>
struct msgpack::adaptor::convert<T, boost::enable_if_t<mplc::meta::detail::has_array_adapter<T>::value>> {
    msgpack::object const& operator()(msgpack::object const& o, T& v) const {
        using namespace mplc;
        meta::Array::from<T>().deserialize(&v, o);
        return o;
    }
};
