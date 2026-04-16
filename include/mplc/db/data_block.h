#pragma once
#include <share/mplcshare.h>
#include "storage.hpp"
#define MAX_BLOCK_RECS 1000
namespace mplc { namespace cache {
    using mplc::lib::mutex;
    using mplc::lib::lock_guard;

    class DataBlock;
    typedef Storage<DataBlock, Allocator2Q, storage_hook> DataBlockStorage;

    class DataBlock : public storage_hook<DataBlockStorage> {
        friend class Layer;
    public:
        typedef PinVector::iterator data_iterator;
        DataBlock();
        static const size_t MAX_SIZE;

        virtual ~DataBlock() {}
        // static ptr make();
        bool hasSpace() const;
        bool empty() const { return m_data.empty(); };
        void push(const PinValue::ptr&);
        void r_join(DataBlock& block);
        void l_join(DataBlock& block);
        void join(FileTime start, DataBlock& block);
        PinValue::ptr& last() { return m_data.back(); }
        PinValue::ptr& first() { return m_data.front(); }
        data_iterator left_it(FileTime time, bool include = false);
        data_iterator right_it(FileTime time, bool include = false);
        PinValue::ptr left_val(FileTime time, bool include = false);
        PinValue::ptr right_val(FileTime time, bool include = false);
        PinVector const& get() const { return m_data; }
        data_iterator begin() { return m_data.begin(); }
        data_iterator end() { return m_data.end(); }
        size_t size() const { return m_data.size(); }
        void erase(FileTime from, FileTime to) {
            m_data.erase(from ? right_it(from, true) : m_data.begin(), right_it(to, true));
        }
        FileTime t_start() const { return m_data.empty() ? FileTime() : m_data.front()->time; }
        FileTime t_end() const { return m_data.empty() ? FileTime() : m_data.back()->time; }

    protected:
        // size_t not_writed;
        PinVector m_data;
    };
    template<class T>
    JsonWriter<T>& operator<<(JsonWriter<T>& writer, DataBlock& block) {
        for(DataBlock::data_iterator it = block.begin(); it != block.end(); ++it) { writer << *it; }
        return writer;
    }
}}  // namespace mplc::cache
