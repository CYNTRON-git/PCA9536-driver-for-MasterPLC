#pragma once
//#include "layer.h"
#include "request.h"
#include "filter.h"
#include "data_archive.h"

namespace mplc {
    namespace archive {
        class DataArchiveManager;
    }
    namespace cache {
        using archive::Interval;
        using archive::Request;
        class Layer;
        class Cache {
        public:
            class Pin {
                friend class archive::DataArchiveManager;
                PinValue::ptr last_rec, first_rec;
                std::map<int, Layer> layers;
                archive::Filter filter;
                lib::spinlock filter_spin;
                mutable lib::mutex _mtx;
                FileTime f_time, l_time;

                int64_t cache_id;
                void updateFirst(Interval::ptr interval);
                void updateLast(Interval::ptr interval);
                void updateLastValue(const PinValue::ptr& rec);
                friend class Cache;

            public:
                typedef lib::shared_ptr<Pin> ptr;
                const vm::ItemID key;
                int64_t getCacheId() const {
                    return cache_id;
                }
                Pin(int64_t item_id, const std::string& path = "");
                int64_t lastTime() const {
                    return l_time;
                }
                int64_t firstTime() const {
                    return f_time;
                }
                PinValue::ptr last() const;
                PinValue::ptr first() const;
                void load(Request::Item::ptr&, const Request::Options&);
                void getFirtsLast(Request::Item::ptr& item);
                void addLayer(archive::ILayerWriter* arch_layer);
                void stop();
                MPLC_DATABASE_API void insert(Request::Item::Interval::ptr interval);
                MPLC_DATABASE_API void directWrite(const PinVector& data, int layer);
                MPLC_DATABASE_API void insertSkippedInterval(const PinVector& data, int layer_id);
                MPLC_DATABASE_API void writeCacheOnly(const PinValue::ptr& rec, int layer_id);
                MPLC_DATABASE_API Layer* getLayer(int id);
                MPLC_DATABASE_API static ptr make(int64_t item_id, const std::string& path = "");
                MPLC_DATABASE_API void setFilter(const archive::Filter& new_filter);
                MPLC_DATABASE_API void operator<<(const PinValue::ptr& rec);
                MPLC_DATABASE_API void write(const PinValue::ptr& rec, bool ignore_filter, bool allow_insert);
                MPLC_DATABASE_API void write(OpcUa_VariantHlp&& val,
                                             FileTime time,
                                             OpcUa_StatusCode sc,
                                             bool use_filter = true);
                MPLC_DATABASE_API void writeStopValue();
                MPLC_DATABASE_API const archive::Filter& getFilter() const {
                    return filter;
                }
                MPLC_DATABASE_API std::map<int, Layer>::const_iterator layers_begin();
                MPLC_DATABASE_API std::map<int, Layer>::const_iterator layers_end();
                MPLC_DATABASE_API std::map<int, const Layer*> getLayers() const;
                MPLC_DATABASE_API int getArchiveId(int layer_id);
                MPLC_DATABASE_API void getArchives(std::vector<int>& all_archive_id) const;
                MPLC_DATABASE_API bool inArchive(int archive_id) const;
                MPLC_DATABASE_API bool checkFilter(const PinValue::ptr& custom_last_rec,
                                                   const PinValue::ptr& new_rec) const;
            };

        private:
            std::vector<Pin::ptr> pins;
            std::map<vm::ItemID, int64_t> pin_ids;
            mutable lib::mutex m_mtx;

        public:
            std::vector<Pin::ptr> all() const {
                lib::lock_guard<lib::mutex> lock(m_mtx);
                return pins;
            }
            void clear();
            void stop();
            MPLC_DATABASE_API const static int64_t ID_NOT_FOUND;
            Cache();
            MPLC_DATABASE_API std::vector<Cache::Pin::ptr> getPinsByArchive(int archive_id) const;
            MPLC_DATABASE_API int64_t getCacheId(const vm::ItemID& pin_id) const;
            MPLC_DATABASE_API int64_t insert(const Pin::ptr& pin);
            MPLC_DATABASE_API Pin::ptr find(const vm::ItemID& item) const;
            MPLC_DATABASE_API Pin::ptr find(int64_t id) const;
            MPLC_DATABASE_API void clearData();
            void clearArchiveCache(int archive_id);
            // MPLC_DATABASE_API Pin::ptr& operator[](int64_t id);
        };
    }  // namespace cache
    typedef cache::Cache::Pin PinCache;
}  // namespace mplc
