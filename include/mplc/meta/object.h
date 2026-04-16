#pragma once

#include "meta_config.h"
#include "property.h"
#include "detail/table.h"
#include "detail/object.h"

namespace mplc { namespace meta {

    class MPLCSHARE_API Object {
    public:
        Object();
        Object(const Object& api): Object(api._table) {}
        Object& operator=(const Object& api) {
            _table = api._table;
            return *this;
        }
        Object(Object&& api) noexcept: _table(api._table) {}
        Object& operator=(Object&& api) noexcept {
            _table = api._table;
            return *this;
        }
        lib::string_view name() const;
        bool valid() const {
            return _table != nullptr;
        }
        const Object super() const;

        Property property(lib::string_view name) const;

        size_t propertyCount() const;

        size_t propertyOffset() const;

        Type type() const;

        void deserialize(const void* obj, const msgpack::object& value) const;
        void serialize(const void* obj, msgpack::object::with_zone& oz) const;

        const detail::PropertiesMap& props() const;

        template<typename T>
        static const Object from();

        using Table = detail::ObjectTable;
        template<typename T>
        struct Holder {
            static_assert(sizeof(T) == -1, "Api is not declared");
            //static const Table& table();
        };

    private:
        template<typename T>
        friend struct Holder;
        friend class Type;

        Object(const Table* table);

    private:
        const Table* _table;
    };

    template<typename T>
    const Object Object::from() {
        static_assert(detail::is_object_t<T>::value, "<T> is not like an object");
        return {&detail::TypeInfo<T>::table()};
    }
    namespace detail {
        template<class T>
        struct TypeInfo<T, boost::enable_if_t<is_object_t<T>::value>> {
            static const ObjectTable& table() {
                return Object::Holder<T>::table();
            }
        };
        template<class T>
        struct ParentApi<T, boost::enable_if_t<!boost::is_void<typename T::_type_list::next>::value>> {
            using type = typename T::_type_list::next;
            static const ObjectTable* table() {
                return &TypeInfo<type>::table();
            }
        };
    }  // namespace detail
}}     // namespace mplc::meta

template<class T>
struct msgpack::adaptor::object_with_zone<T, boost::enable_if_t<mplc::meta::detail::is_object<T>::value>> {
    void operator()(msgpack::object::with_zone& oz, const T& v) const {
        using namespace mplc;
        return meta::Object::from<T>().serialize(&v, oz);
    }
};
template<class T>
struct msgpack::adaptor::convert<T, boost::enable_if_t<mplc::meta::detail::is_object<T>::value>> {
    msgpack::object const& operator()(msgpack::object const& o, T& v) const {
        using namespace mplc;
        meta::Object::from<T>().deserialize(&v, o);
        return o;
    }
};
