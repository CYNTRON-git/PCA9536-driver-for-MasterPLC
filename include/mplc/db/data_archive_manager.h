#pragma once
#include <boost/core/noncopyable.hpp>
#include "cache.h"
#include "data_archive.h"
#include "mplc/db/table_size_info.h"
#include <mplc/libs/containers.hpp>
#include <mplc/libs/threads.hpp>

namespace mplc { namespace archive {
    struct PinInfo {
        vm::ItemID key;
        struct LayerInfo {
            bool is_local;
            bool sync;
        };
        // layer_id, is_local db
        std::map<int, LayerInfo> layers;
    };

    class DataArchiveManager final : boost::noncopyable {
        typedef std::list<std::pair<int, Request::weak_ptr>> RequestQueue;
        typedef std::map<std::string, IDataArchiveProcFactory*> FactoryMap;
        FactoryMap factories;
        std::map<int, IDataArchiveProc::ptr> archives;
        std::map<int64_t, int> archives_id;
        
        // std::map<int64_t, PinInfo> pin_kyes;
        // cache_id, layer_id : archive_id
        // std::map<std::pair<int64_t, int>, int> cache_to_archive;
        RequestQueue requests;
        Document cfg;
        cache::Cache cache;
        lib::mutex mtx_create_pin;
        lib::mutex req_mutex;
        bool stop;
        lib::thread* request_process;
        int _default;
        void loadFromCache(const Request::ptr& req, std::vector<Interval::ptr>&) const;
        void requestProc();
        DataArchiveManager();

    public:
        ~DataArchiveManager();
        void addArchive(int db_id, const Value& config);
        void Stop();
        void Clear();
        OpcUa_StatusCode Start();
        bool Reload(int64_t archvie_id);
        MPLC_DATABASE_API int getArchiveId(const vm::ItemID& id) const;
        MPLC_DATABASE_API int64_t lastTime(int64_t cache_id, bool* loaded = nullptr) const;
        MPLC_DATABASE_API int64_t firstTime(int64_t cache_id, bool* loaded = nullptr) const;
        MPLC_DATABASE_API bool updateBounds(const PinCache::ptr& pin) const;
        MPLC_DATABASE_API void syncRead(const Request::ptr& req, int64_t timeout = FT_MINUTE);
        MPLC_DATABASE_API void regFactory(const std::string& name, IDataArchiveProcFactory* factory);
        MPLC_DATABASE_API int64_t createPinCache(const vm::ItemID& pin_id);
        MPLC_DATABASE_API int64_t getCacheId(const vm::ItemID& pin_id) const;
        MPLC_DATABASE_API bool isLoaded(const vm::ItemID& pin_id) const;
        MPLC_DATABASE_API void clearCache();
        MPLC_DATABASE_API std::vector<PinCache::ptr> getPinsByArchive(int archive_id) const;
        int64_t getCacheId(int64_t vm_item_id, const std::string& path) const {
            return getCacheId(vm::ItemID(vm_item_id, path));
        }
        MPLC_DATABASE_API PinCache::ptr getPinCache(int64_t cache_id) const;
        std::vector<PinCache::ptr> getPinList() const {
            return cache.all();
        }

        // timeout - Max time waiting response in ms. <= 0 INFINITY
        MPLC_DATABASE_API void addRequest(const Request::ptr& req, int timeout = 0);
        MPLC_DATABASE_API static DataArchiveManager& instance();
        MPLC_DATABASE_API void GetTableSizeInfo(TableSizeInfo& table_info, int archive_id) const;
        MPLC_DATABASE_API void GetStatistics(CountStatistics& stats, int archive_id) const;
        MPLC_DATABASE_API void CheckConnected(ArchiveConnectionStatus& status, int archive_id) const;
        MPLC_DATABASE_API std::string GetHost(int archive_id) const;
        MPLC_DATABASE_API int64_t GetMaxTableSize(int archive_id) const;
        MPLC_DATABASE_API IDataArchiveProc::ptr GetArchive(int64_t archive_id) const;
        MPLC_DATABASE_API bool isSync() const;
        MPLC_DATABASE_API std::vector<int64_t> getSyncArchivesId() const;
    };
}}  // namespace mplc::archive
