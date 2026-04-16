#pragma once
#include "data_block.h"
#include "request.h"
#include "cache.h"

namespace mplc { namespace cache {

    using archive::Interval;
    using archive::Request;
    class Layer {
        /*struct AsyncSampling : AsyncTask {
            archive::Sampling sampl;
            AsyncSampling(int64_t interval_len, PinList& data, const PinList::iterator to);
            void flush() { sampl.flush(); }
            void push(const PinValue::ptr& pin) { sampl.push(pin); }
        };*/
        enum UseStorage { use_cache = 0x01, use_arch = 0x02 };
        uint8_t used_storages;
        static DataBlockStorage::ptr storage;
        typedef std::map<FileTime, DataBlock::weak_ptr> Intervals;
        DataBlock::ptr last_block;
        int layer_id;
        Intervals m_map;
        archive::ILayerWriter* archive_layer;
        // bool use_cache;
        PinList tmp_store;
        archive::Sampling* sampl;
        FileTime last_time;
        void baseInit();
        PinCache* pin;

    public:
        Layer();
        static void Stats();
        void init(archive::ILayerWriter* _write_to_item, PinCache* pin);
        typedef boost::shared_ptr<Layer> ptr;
        // Layer& operator<<(DataBlock::ptr &block);
        Layer& operator<<(const PinValue::ptr& rec);
        void write(const PinValue::ptr& rec, bool cacheOnly);
        void getLostTopInterval(const Request::Item::ptr& item,
                                FileTime t_end,
                                FileTime first,
                                DataBlock::ptr data) const;
        bool load(Request::Item::ptr& item, const Request::Options& opts) const;
        PinValue::ptr directWrite(const PinVector& data, bool cache_only);
        void assign(FileTime start, FileTime end, const PinVector& data);
        void insertSkippedInterval(FileTime t_start, FileTime t_end, const PinVector& data);
        void flush();
        int getId() const {
            return layer_id;
        }
        int getArchiveId() const {
            return archive_layer->archive_id;
        }
        bool isSync() const {
            return archive_layer->is_sync;
        }
        static void clearGlobalCache() {
            if (storage)
                storage->clear();
        }
        void clear() {
            m_map.clear();
            if (used_storages != use_arch) {
                baseInit();
            }
        }
        /*template<class T>
        JsonWriter<T>& operator<<(JsonWriter<T>& writer) const {
            writer.StartObject();
            writer.Key("layer").Int(layer_id);
            writer.Key("blocks").StartArray();
            for(BOOST_AUTO(it, m_map.begin()); it != m_map.end(); ++it) {
                writer.StartObject();
                writer.Key("time").Int64(it->first);
                writer.Key("data").StartArray();
                writer << it->second.lock();
                writer.EndArray();
                writer.EndObject();
            }
            writer.EndArray();
            writer.EndObject();
            return writer;
        }*/
    private:
        // void getFirtsLast(Request::Item::ptr &item, int64_t t_start, int64_t t_end);
        void getLeftBound(Intervals::const_iterator it, Request::Item::ptr& item, int64_t time) const;
        void getRightBound(Intervals::const_iterator it, Request::Item::ptr& item, int64_t time) const;
        void set_last(FileTime time, DataBlock::ptr const& val);
    };
}}  // namespace mplc::cache
