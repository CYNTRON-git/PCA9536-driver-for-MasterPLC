#pragma once
#include <queue>
#include "request.h"
#include "filter.h"
#include "stats.h"
#include "archive_proc.hpp"
#include "item.h"
#include "table_size_info.h"

#include "custom_request.h"
#include "data_backup_request_item.h"
#include "skipped_interval_request.h"

namespace mplc { namespace archive {
    struct SkippedIntervalRequest;

    struct ILayerWriter {
        virtual ~ILayerWriter() {}
        virtual PinValue::ptr insert(const PinVector& data) = 0;
        virtual void push(const PinValue::ptr& val) = 0;
        virtual int vm_layer_id() const {
            return _vm_layer_id;
        }
        virtual int64_t item_id() const = 0;
        bool is_local;
        bool is_sync;
        bool cache_only;
        int archive_id;
        int _vm_layer_id;
        int64_t max_size;
        TimeSpan max_time_storage;
        // int64_t lose_data;
        virtual const vm::Layer* archive_layer_info() {
            if (!layer_meta) {
                const vm::VMInfo& vm = vm::VMInfo::GetInstance();
                layer_meta = vm.GetLayer(vm_layer_id());
            }
            return layer_meta;
        }

    protected:
        ILayerWriter()
            : is_local(false), is_sync(false), cache_only(false), archive_id(0), _vm_layer_id(0), max_size(0),
              layer_meta(nullptr) {}

        const vm::Layer* layer_meta;
    };

    struct IDataArchive {
        virtual ~IDataArchive() {}
        typedef lib::shared_ptr<IDataArchive> ptr;

        virtual void Reload() {}
        virtual void addRequest(const Interval::ptr&) = 0;
        virtual void addRequest(db::CustomRequest::ptr) = 0;
        virtual void addRequest(SkippedIntervalRequest::ptr) = 0;
        virtual std::vector<data::RequestItem> getSkippedIntervals() = 0;
        virtual Item* getItem(const vm::ItemID& pin_id) const = 0;
        virtual const Filter& getFilter() const = 0;

        virtual std::vector<vm::ItemID> getItems() const = 0;
        virtual ILayerWriter* regItem(const vm::ItemID& pin_id, int layer_id, int64_t max_storage_time) = 0;
        virtual void GetTableSizeInfo(TableSizeInfo& table_info) const = 0;
        virtual void GetStatistics(CountStatistics& stats) = 0;
        virtual void CheckConnected(ArchiveConnectionStatus& status) const = 0;
        virtual int64_t GetMaxRawDataSize() const = 0;

    protected:
        friend class DataArchiveManager;
        virtual void setFilter(const rapidjson::Value& config) = 0;
    };

#define MAX_DATA_IN_QUEUE 10000
    /// Общий класс слоя для Soci и Sqlite
    struct StoreLayer final : ILayerWriter {
        DbItem::ptr item;
        lib::mutex mtx;
        std::vector<PinValue::ptr> data;
        Stats& stats;
        StoreLayer(Stats& stats): stats(stats) {
            max_size = MAX_DATA_IN_QUEUE;
        }
        PinValue::ptr insert(const PinVector& new_data) override {
            lib::lock_guard<lib::mutex> lock(mtx);
            data.insert(data.begin(), new_data.begin(), new_data.end());
            std::sort(data.begin(), data.end(), PinValue::sort_by_time);
            item->count += new_data.size() - 1;
            item->up(*data.back());
            return data.back();
        }
        void push(const PinValue::ptr& val) override {
            item->up(*val);
            lib::lock_guard<lib::mutex> lock(mtx);
            if (data.empty() || data.back()->time < val->time) {
                data.push_back(val);
            } else if (data.back()->time == val->time) {
                return;
            } else if (data.back()->time > val->time) {
                auto it = std::upper_bound(data.begin(), data.end(), val, PinValue::sort_by_time);
                if (it != data.begin()) {
                    if ((*--it)->time == val->time) {
                        return;
                    }
                    ++it;
                }
                data.insert(it, val);
            }
            if (data.size() > max_size) {
                size_t cur_size = data.size();
                int64_t last_interval = (data.back()->time - data.front()->time).dt() / (max_size * 2);
                Sampling::sampl(data, last_interval);
                size_t diff = cur_size - data.size();
                item->count -= diff;
                stats.add_lose_data(diff);
            }
        }
        int64_t item_id() const override {
            return item ? item->id : 0;
        }
    };

    template<class _ConnectionPool>
    class DataArchive : public IArchive<_ConnectionPool, ArchiveCfg>, public IDataArchive {
    protected:
        mutable lib::mutex req_mtx;
        Filter filter;
        std::queue<Interval::ptr> requests;
        std::queue<db::CustomRequest::weak_ptr> custom_requests;

        /// Для определения нужно добавления пропуещнные интервалы. (При запуске и при переходе SLAVE ->MASTER)
        bool was_master = false;
        // Пропущенные интервалы полученные из базы
        lib::unordered_map<data::SkippedIntervalUniqueKey, int64_t> skipped_intervals;
        mutable lib::mutex intervals_mtx;

        // Запрос на запись полученных данных по пропущенными интервалам в базу(не более 1 следующий запрос после
        // завершение текущего)
        lib::shared_ptr<SkippedIntervalRequest> skipped_interval_request;

        DataArchive() {}
        void setFilter(const rapidjson::Value& config) override {
            filter.load(config);
        }

    public:
        virtual ~DataArchive() {}
        void addRequest(const Interval::ptr& interval) override {
            lib::lock_guard<lib::mutex> lock(req_mtx);
            requests.push(interval);
        }
        void addRequest(db::CustomRequest::ptr request) override {
            lib::lock_guard<lib::mutex> lock(req_mtx);
            custom_requests.push(request);
        }
        // virtual void          syncRead(const Interval::ptr&) = 0;
        const Filter& getFilter() const override {
            return filter;
        }

        void addRequest(SkippedIntervalRequest::ptr request) override {
            lib::lock_guard intervals_lock(intervals_mtx);
            skipped_interval_request = request;
        }

        std::vector<data::RequestItem> getSkippedIntervals() override {
            lib::lock_guard lock(intervals_mtx);
            // После получения запроса от клиента резервирования интервалы будут храниться в нём.
            std::vector<data::RequestItem> temp;
            for (const auto& [key, t_end]: skipped_intervals) {
                temp.emplace_back(key, t_end);
            }
            skipped_intervals.clear();
            return temp;
        }

    };  // namespace archive

    struct IDataArchiveProc {
        virtual IDataArchive& getArchive() = 0;
        virtual void run() = 0;
        virtual void stop() = 0;
        virtual bool isLocal() const = 0;
        virtual bool isSync() const = 0;
        virtual void getStatus(ArchiveConnectionStatus& status) = 0;
        virtual std::string getHost() = 0;
        virtual ~IDataArchiveProc() {}
        virtual void Reload() {}
        typedef boost::shared_ptr<IDataArchiveProc> ptr;
    };

    struct IDataArchiveProcFactory {
        virtual IDataArchiveProc::ptr make(const ArchiveCfg& arch_cfg, const ConnectionCfg& pool_cfg) = 0;
    };

}}  // namespace mplc::archive
