#pragma once

#include "meta_config.h"
#include "arg.h"
#include "type.h"
#include "detail/table.h"
namespace mplc { namespace meta {
    class Object;

    /*! \breif The Method class provides meta information for property.
     */
    class MPLCSHARE_API Property {
    public:
        template<typename T>
        struct Holder;
        using Table = detail::PropertyTable;
        // using HashTable = detail::PropertyHashTable;

        Property(): _table(nullptr) {}
        Property(const Table* table);
        Property(const Property& prop): Property(prop._table) {}
        Property& operator=(const Property& prop) {
            new (this) Property(prop._table);
            return *this;
        }
        Property(Property&& prop) noexcept: Property(prop._table) {}
        Property& operator=(Property&& prop) noexcept {
            new (this) Property(prop._table);
            return *this;
        }

        bool valid() const;
        bool isObject() const;
        bool isArray() const;

        /*! Returns the name of the property.
            \sa type()
        */
        lib::string_view name() const;

        /*! Returns the type of the property.
            \sa name()
        */
        Type type() const;
        
        MetaType metaType() const;
        /*! Return the value of the property.
         */
        void get(Arg obj, Arg ret) const;

        /*! Sets the given value to the property.
         */
        void set(Arg obj, Arg value) const;

        void serialize(const void* obj, msgpack::object::with_zone& dst) const;
        void deserialize(const void* obj, const msgpack::object& value) const;
        const void* ptr(const void* obj) const;
        Arg arg(const void* obj) const;

    private:
        template<typename T>
        friend struct Holder;

        friend class Object;

    private:
        const Table* _table;
    };
}}  // namespace mplc::meta
