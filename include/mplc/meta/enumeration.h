#pragma once


#include "meta_config.h"
#include "detail/table.h"

namespace mplc { namespace meta {
    class Api;

    class MPLCSHARE_API Enumeration {
    public:
        bool valid() const;

        lib::string_view name() const;

        int keyCount() const;
        lib::string_view key(int index) const;
        int value(int index) const;

        int keyToValue(const char* key) const;
        lib::string_view valueToKey(int value) const;

    public:
        template<typename T>
        struct Holder;

        using Table = detail::EnumerationTable;
        // using HashTable = detail::EnumerationHashTable;
    private:
        template<typename T>
        friend struct Holder;
        friend class Api;

        Enumeration(const Table* table);

    private:
        const Table* _table;
    };
}}  // namespace umof
