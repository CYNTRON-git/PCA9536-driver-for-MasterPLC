#pragma once

namespace mplc { namespace archive {
    struct Item {
        ADD_MAKE_PTR(Item)
        Item(): vm_item_id(0), f_time(0), l_time(0), count(0), type(OpcUaType_Null), connected(false) {}

        Item(const Item& item)
            : vm_item_id(item.vm_item_id), f_time(item.f_time), l_time(item.l_time), count(item.count), path(item.path),
              type(item.type), connected(item.connected) {}

        Item& operator=(const Item& item) {
            vm_item_id = item.vm_item_id;
            f_time = item.f_time;
            l_time = item.l_time;
            count = item.count;
            path = item.path;
            type = item.type;
            connected = item.connected;
            return *this;
        }
        virtual ~Item() {}
        int64_t vm_item_id;
        int64_t f_time;
        int64_t l_time;
        int64_t count;
        lib::mutex mtx;
        std::string path;
        OpcUa_BuiltInType type;
        bool connected;
        MPLC_DATABASE_API void up(const PinValue& val);

        /*
        inline void lock() const { _mtx.lock(); }
        inline void unlock() const { _mtx.unlock(); }

        virtual void write(const PinValue::ptr& val, int layer);
        virtual void swapData(Item& item);
        virtual void rollback(Item& item);
        void addLayer(int layer_id, int64_t max_time);

        std::map<int, int64_t> layers_size;
        std::map<int, std::vector<PinValue::ptr> > data;
        mutable mplc::lib::mutex _mtx;
        */
    };

    /// Общий тип Item-а для soci и sqlite
    struct DbItem : Item {
        ADD_MAKE_PTR(DbItem)
        int32_t id;
        int32_t project_id;
        int64_t db_count;
        std::string name;

        DbItem(): id(0), project_id(0), db_count(0) {}

        DbItem(const DbItem& item)
            : Item(item), id(item.id), project_id(item.project_id), db_count(item.count), name(item.name) {}

        DbItem& operator=(const DbItem& item) {
            if (this == &item)
                return *this;
            Item::operator=(item);

            id = item.id;
            name = item.name;
            db_count = item.db_count;
            return *this;
        }
    };

}}  // namespace mplc::archive
