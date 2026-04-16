#pragma once
#include <mplc/libs/bind.hpp>
#include <mplc/libs/threads.hpp>
#include <mplc/libs/smart_ptr.hpp>
#include <mplc/libs/optional.hpp>
#include <share/mplcshare.h>
#include <mplc/aggregation.hpp>

#include "config.h"
#include "sampling.h"
#include "item.h"

#define MIN_SAMPLING_INTERVAL (100) * FT_MILLISECOND
#ifndef INT64_MAX
#    define INT64_MAX 9223372036854775807ll
#endif
namespace mplc { namespace archive {
    template<class _Ptr>
    class interval_comapare {
        bool operator()(const _Ptr& lhs, const _Ptr& rhs) {
            if (!lhs)
                return false;
            if (!rhs)
                return true;
            return lhs->start < rhs->start;
        }
    };
    class Request final : public lib::enable_shared_from_this<Request> {
        lib::mutex m_mtx;
        void finalizeItem(const int64_t cache_id);
        lib::atomic<size_t> m_finalized_items;

    public:
        typedef lib::shared_ptr<Request> ptr;
        typedef lib::weak_ptr<Request> weak_ptr;

        struct Item final : lib::enable_shared_from_this<Item> {
            typedef lib::shared_ptr<Item> ptr;
            typedef lib::weak_ptr<Item> weak_ptr;

            struct Interval final : lib::enable_shared_from_this<Interval> {
                typedef lib::shared_ptr<Interval> ptr;
                typedef lib::weak_ptr<Interval> weak_ptr;
                typedef lib::function<void(ptr)> Listener;

                Interval(int id, int layer, int64_t start, int64_t end, lib::mutex& mtx)
                    : read_count(0), start(start), end(end), cache_id(0), mtx(mtx), item(nullptr), sampl(nullptr),
                      id(id), layer(layer), sort(asc), status(wait) {}
                virtual ~Interval();
                static ptr make(int id, int layer, int64_t start, int64_t end, lib::mutex& mtx) {
                    return lib::make_shared<Interval>(id, layer, start, end, boost::ref(mtx));
                }

                MPLC_DATABASE_API void finalize();
                MPLC_DATABASE_API void push(const PinValue::ptr& value);
                bool* addListener(const Listener& listener);
                MPLC_DATABASE_API int64_t availableSpace() const;

                std::vector<std::pair<Listener, bool> > listeners;
                PinVector data;
                lib::function<void(const PinValue::ptr&)> to_item;
                Item::weak_ptr p_item;
                int64_t read_count;
                FileTime start, end;
                int64_t cache_id;
                lib::mutex& mtx;
                lib::optional<int64_t> limit;
                const archive::Item* item;
                Sampling* sampl;
                int id;
                int layer;
                // ASC sorts from the lowest value to highest value.
                // DESC sorts from highest value to lowest value.
                enum { asc, desc } sort;
                enum { wait, finish, skip, fail } status;
            };
            aggregation::proc aggregation;
            Request::weak_ptr from_req;
            std::vector<Interval::ptr> intervals;
            lib::unique_ptr<FileTime> continue_point;
            int64_t cache_id;
            lib::atomic<OpcUa_StatusCode> status_code;
            int64_t limit;
            int64_t interval;
            FileTime t_start, t_end;
            int layer;
            PinList data;
            lib::function<void(const PinValue::ptr&)> proxy;
            lib::mutex mtx;
            Item(int64_t const cache_id, int layer = 0)
                : cache_id(cache_id), status_code(OpcUa_BadWaitingForInitialData), limit(0), interval(0), t_start(0),
                  t_end(0), layer(layer) {}

            virtual ~Item() {
                lib::lock_guard<lib::mutex> lock(mtx);
                data.clear();
            }
            static ptr make(int64_t cache_id, int layer = 0) {
                return lib::make_shared<Item>(cache_id, layer);
            }
            void setAggregation(aggregation::Type type, bool total = false, uint32_t filterMask = 0);
            template<class It>
            void insert(It& begin, It& end) {
                // if (count) {
                //     DateTime right;
                //     if (!data.empty()) {
                //         /*auto it = std::find_if(data.begin(), data.end(), [](const PinValue::ptr& v) {
                //             return !!v;
                //         });
                //         if (it != data.end()) {
                //             left = DateTime{(*it)->time.dt(), DateTime::tick};
                //         }*/
                //         auto itr = std::find_if(data.rbegin(), data.rend(), [](const PinValue::ptr& v) {
                //             return !!v;
                //         });
                //         if (itr != data.rend()) {
                //             right = DateTime{(*itr)->time.dt(), DateTime::tick};
                //         }
                //     }
                //     auto iv_left = DateTime{(*from)->time.dt(), DateTime::tick};
                //     auto iv_right = DateTime{(*(from + count - 1))->time.dt(), DateTime::tick};
                //     PRINTLN("Insert data after: %s > (%s .. %s ) : %zu",
                //             right.to_string().c_str(),
                //             iv_left.to_string().c_str(),
                //             iv_right.to_string().c_str(),
                //             count);
                //     if ((right && right > iv_left)) {
                //         int wtf = 1;
                //     }
                // }

                if (interval > MIN_SAMPLING_INTERVAL) {
                    Sampling::insert(data, interval, begin, end);
                } else {
                    data.insert(data.end(), begin, end);
                }
            }
            bool isWaiting() const {
                return status_code.load(lib::memory_order::acquire) != OpcUa_BadOperationAbandoned;
            }
            void finalize(bool no_archive_data = false);
            bool hasSpace();
            size_t availableSpace();
            Interval::ptr& addBotInterval(int layer = 0, int64_t start = 0, int64_t end = INT64_MAX);
            Interval::ptr& addTopInterval(int layer = 0, int64_t start = 0, int64_t end = INT64_MAX);
            Interval::ptr& addInterval(int layer = 0, int64_t start = 0, int64_t end = INT64_MAX);

        private:
            void intervalComplete(Interval::ptr interval);
            Interval::ptr& createInterval(int layer = 0, int64_t start = 0, int64_t end = INT64_MAX);
            void push_back(const PinValue::ptr& pin);
            void push_front(const PinValue::ptr& pin);
        };

        struct Options {
            Options()
                : get_bounds(false), only_first_last(false), only_item_list(false), use_other_layers(false),
                  use_cache(true), layer_id(0), reversed(false) {}

            bool get_bounds;
            bool only_first_last;
            bool only_item_list;
            bool use_other_layers;
            bool use_cache;
            int layer_id;
            bool reversed;
            FileTime t_start, t_end, interval;

            void fromJson(const Value& parameters);
            void toJson(ResponseWriter& writer) const;
        };
        friend struct Item;
        typedef std::vector<Item::ptr> Items;
        // ActiveTime waiting response in ms 0 = INFINITY
        int64_t m_timeout;
        OpcUa_StatusCode status;
        Options opts;
        Items m_items;
        lib::function<void(const ptr&)> callback;
        MPLC_DATABASE_API Item::ptr& addItem(int64_t cache_id);
        MPLC_DATABASE_API bool isLoaded() const;
        void timeout();
        Items& items() {
            return m_items;
        }
        static ptr make(const int64_t _timeout = 0) {
            return lib::make_shared<Request>(_timeout);
        }
        MPLC_DATABASE_API static ptr make(const Value& request);
        void finalize() {
            status = OpcUa_Good;
            if (callback)
                callback(shared_from_this());
        }
        void repeat(std::vector<Request::Item::ptr>& items) {
            m_items.swap(items);
            status = OpcUa_BadWaitingForInitialData;
            m_finalized_items = 0;
        }
        MPLC_DATABASE_API Request(const int64_t _timeout)
            : m_finalized_items(0), m_timeout(_timeout), status(OpcUa_BadWaitingForInitialData) {}
        ~Request() {}
    };

    typedef Request::Item::Interval Interval;
}}  // namespace mplc::archive
