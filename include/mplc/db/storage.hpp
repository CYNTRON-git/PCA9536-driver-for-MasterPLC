#pragma once
#include <boost/smart_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/container/list.hpp>
#include <mplc_stdint.h>
#include <map>
#include <list>

#include "mplc/log.h"

namespace mplc {
    template<class _Storage>
    class storage_hook {
    protected:
        friend _Storage;
        friend typename _Storage::_Mybase;
        typename _Storage::type_id id;
        typename _Storage::link storage_link;

    public:
        typedef typename _Storage::data_ptr ptr;
        typedef typename _Storage::weak_ptr weak_ptr;

        static ptr makeFrom(const typename _Storage::ptr& storage) {
            return storage->make_new();
        }

        void used() {
            typename _Storage::ptr storage = storage_link.lock();
            if (storage)
                storage->up(id);
        }
        void drop() {
            typename _Storage::ptr storage = storage_link.lock();
            if (storage)
                storage->drop(id);
        }
        virtual ~storage_hook() {}
    };
    template<class _Type>
    struct BaseAllocator {
        typedef uint32_t limit_type;
        typedef uint32_t type_id;
        typedef lib::shared_ptr<_Type> data_ptr;
        typedef lib::weak_ptr<_Type> data_weak_ptr;
        virtual data_ptr make_new() = 0;
        virtual void up(const type_id& id) = 0;
        virtual void drop(const type_id& id) = 0;
        virtual void clear() = 0;
    };

    template<class _Type, template<class> class _Allocator, template<class> class _Hook>
    class Storage : protected _Allocator<_Type>,
                    public lib::enable_shared_from_this<Storage<_Type, _Allocator, _Hook> > {
        typedef Storage<_Type, _Allocator, _Hook> StorageType;

        friend _Hook<StorageType>;

        typedef _Allocator<_Type> _Mybase;
        typedef _Type value_type;

        typedef typename _Mybase::type_id type_id;
        typedef typename _Mybase::limit_type limit_type;
        typedef typename _Mybase::data_ptr data_ptr;
        typedef typename _Mybase::data_weak_ptr weak_ptr;

        typedef lib::weak_ptr<StorageType> link;
        Storage(limit_type limit): _Mybase(limit) {}

    public:
        void Stats() const {
            _Mybase::Stats();
        }
        typedef lib::shared_ptr<StorageType> ptr;
        ptr static make(limit_type limit) {
            return ptr(new StorageType(limit));
        }
        virtual void up(const type_id& id) {
            _Mybase::up(id);
        }
        virtual void drop(const type_id& id) {
            _Mybase::drop(id);
        }
        virtual void clear() {
            _Mybase::clear();
        }
        virtual data_ptr make_new() {
            data_ptr tmp = _Mybase::make_new();
            tmp->storage_link = this->weak_from_this();
            return tmp;
        }
    };

    template<class _Type>
    class Allocator2Q {
    protected:
        typedef uint32_t limit_type;
        typedef uint32_t type_id;
        typedef lib::shared_ptr<_Type> data_ptr;
        typedef lib::weak_ptr<_Type> data_weak_ptr;
        typedef boost::container::list<data_ptr> data_list;

        typedef lib::unordered_flat_map<type_id, typename data_list::iterator> StorageMap;

        mutable lib::mutex mtx;
        limit_type max_new, max_out, max_hot;
        StorageMap out_map, hot_map;
        data_list new_block, outed, hot;
        const type_id no_ID;
        type_id free_id;
        // async::AsyncTask stats;
        Allocator2Q(const limit_type limit)
            : max_new(limit / 4), max_out(max_new * 2), max_hot(limit - max_new - max_out), no_ID(0), free_id(1) {
            // stats = async::CreateRepeatedTask("CacheDataStats", TimeSpan::Seconds(30), [this] { Stats(); });
        }

    public:
        void Stats() const {
            size_t new_blocks, outed_blocks, hot_blocks;
            {
                lib::lock_guard<lib::mutex> lock(mtx);
                new_blocks = new_block.size();
                outed_blocks = outed.size();
                hot_blocks = hot.size();
            }
            size_t all = new_blocks + outed_blocks + hot_blocks;
            double all_max = max_new + max_out + max_hot;
            PRINT_SYS("CacheDataStats a(%zu, %.1f) n(%zu, %.1f) o(%zu, %.1f) h(%zu, %.1f)",
                      all,
                      all / all_max * 100,

                      new_blocks,
                      new_blocks / (double)max_new * 100,

                      outed_blocks,
                      outed_blocks / (double)max_out * 100,

                      hot_blocks,
                      hot_blocks / (double)max_hot * 100);
        }
        virtual ~Allocator2Q() {}
        virtual void drop(const type_id& id) {
            if (id == no_ID)
                return;
            lib::lock_guard<lib::mutex> lock(mtx);
            auto it = hot_map.find(id);
            if (it != hot_map.end()) {
                hot.erase(it->second);
                hot_map.erase(it);
                return;
            }
            it = out_map.find(id);
            if (it != out_map.end()) {
                outed.erase(it->second);
                out_map.erase(it);
            }
        }

        virtual void clear() {
            lib::lock_guard<lib::mutex> lock(mtx);
            new_block.clear();
            outed.clear();
            hot.clear();
            out_map.clear();
            hot_map.clear();
        }
        virtual data_ptr make_new() {
            lib::lock_guard<lib::mutex> lock(mtx);
            data_ptr data = lib::make_shared<_Type>();
            data->id = no_ID;
            new_block.push_front(data);
            if (new_block.size() > max_new) {
                outed.splice(outed.begin(), new_block, --new_block.end());
                auto it = outed.begin();
                data_ptr& tmp = *it;
                tmp->id = free_id++;
                if (outed.size() > max_out) {
                    out_map.erase(outed.back()->id);
                    outed.pop_back();
                }
                out_map[tmp->id] = it;
            }
            return data;
        }

        virtual void up(const type_id& id) {
            if (id == no_ID)
                return;
            lib::lock_guard<lib::mutex> lock(mtx);
            auto it = hot_map.find(id);
            if (it != hot_map.end()) {                     // Если данные уже в списке Hot
                hot.splice(hot.begin(), hot, it->second);  // Поднять вверх
                return;
            }
            it = out_map.find(id);
            if (it == out_map.end())
                return;                                  // Если данные ещё в списке Out
            hot.splice(hot.begin(), outed, it->second);  // Перенести в начало списка Hot
            type_id& hot_id = hot.front()->id;
            out_map.erase(hot_id);
            if (hot.size() > max_hot) {         // Если Hot переполнен
                hot_map.erase(hot.back()->id);  // Удалить последнюю запись
                hot.pop_back();
            }
            hot_map[hot_id] = hot.begin();
        }
    };

}  // namespace mplc
