#pragma once

#include "meta_config.h"
#include "detail/type.h"
#include "array.h"

namespace mplc { namespace meta {
    class Any;
    class Method;
    class Property;

    /* \breif The Type class manages types.
     */
    class MPLCSHARE_API Type {
    public:
        using Table = detail::TypeTable;
        /*! Construct the meta information for the given type T.
         */
        template<class T>
        static Type from();
        Type(const Table* table);
        Type(const Type& type): _table(type._table) {}
        Type& operator=(const Type& type) {
            _table = type._table;
            return *this;
        }
        Type(Type&& type) noexcept: _table(type._table) {}
        Type& operator=(Type&& type) noexcept {
            _table = type._table;
            return *this;
        }
        /*! Return true if type are equal.
            Any qualifiers will be omitted.
            So int, int&, int&&, const int& are all the same types.
        */
        bool operator==(const Type& other);
        bool operator!=(const Type& other);

        /*! Checks whether Type is valid object.
            Call to invalid type will result in application crash.
        */
        bool valid() const;

        /*! Returns type name
         */
        const char* name() const;

        /*! Returns the size of type.
         */
        int size() const;

        /*! Construct the value of the type in the existing address in memory.
            If copy is provided it will be copy constructed, otherwise it will
            be default constructed. The function returns where.
        */
        void* construct(void* where, void* const copy = nullptr) const;

        /*! Creates the value of the type.
         */
        void* create(void* const copy = nullptr) const;

        /*! Destructs the value located at data.
         */
        void destruct(void* data) const;

        /*! Destruct the value and free memory.
         */
        void destroy(void* data) const;

        void serialize(const void* data, msgpack::object::with_zone& ret) const;
        void deserialize(const msgpack::object& ret, void* data) const;
        MetaType metaType() const {
            if (_table) {
                return _table->meta_type;
            }
            return MetaType::None;
        }
        Object object() const;
        Array array() const;

    private:
        friend class Any;
        friend class Method;
        friend class Property;
        friend class Object;
        friend class Array;

    private:
        const Table* _table;
    };

    template<class T>
    Type Type::from() {
        return {&detail::TypeInfo<T>::table()};
    }

}}  // namespace mplc::meta
