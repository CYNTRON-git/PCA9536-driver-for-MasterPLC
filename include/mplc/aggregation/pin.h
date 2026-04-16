#pragma once
#include "aggregation_config.h"
#include <mplc/libs/smart_ptr.hpp>
#include <mplc/libs/threads.hpp>
#include "quality.h"

struct ArchiveRec;
namespace mplc { namespace aggregation {
    struct Pin {
        Pin() = default;
        MPLC_AGGREAGATION_API Pin(const ArchiveRec& rec);
        Pin(const Pin& val) {
            quality = val.quality;
            value.CopyFrom(val.value);
            time = val.time;
        }
        Pin& operator=(const Pin& val) {
            quality = val.quality;
            value.CopyFrom(val.value);
            time = val.time;
            return *this;
        }
        Pin(Pin&& val) noexcept {
            quality = val.quality;
            time = val.time;
            value.Swap(val.value);
        }
        Pin& operator=(Pin&& val) noexcept {
            quality = val.quality;
            time = val.time;
            value.Swap(val.value);
            return *this;
        }
        typedef lib::intrusive_ptr<Pin> ptr;
        static ptr make() {
            return lib::intrusive_ptr<Pin>(new Pin());
        }

        static ptr make(const ArchiveRec& rec) {
            return lib::intrusive_ptr<Pin>(new Pin(rec));
        }
        static ptr make(const Pin& rec) {
            return lib::intrusive_ptr<Pin>(new Pin(rec));
        }
        static bool sort_by_time(const ptr& l, const ptr& r) {
            if (!l)
                return true;
            if (!r)
                return false;
            return l->time < r->time;
        }
        static bool sort_by_time_desc(const ptr& l, const ptr& r) {
            if (!l)
                return false;
            if (!r)
                return true;
            return l->time > r->time;
        }
        static bool time_equal(const ptr& l, const ptr& r) {
            if (l == r)
                return true;
            if (!r || !l)
                return false;
            return l->time == r->time;
        }
        OpcUa_VariantHlp value;
        Quality quality;
        FileTime time;

    private:
        mutable lib::atomic<int> refcount_{};
        friend void intrusive_ptr_add_ref(const Pin* x) {
            x->refcount_.fetch_add(1, lib::memory_order_relaxed);
        }
        friend void intrusive_ptr_release(const Pin* x) {
            if (x->refcount_.fetch_sub(1, lib::memory_order_release) == 1) {
                boost::atomic_thread_fence(lib::memory_order_acquire);
                delete x;
            }
        }
    };

    inline const Value& operator>>(const Value& json, Pin::ptr& val) {
        if (json.IsNull() || !json.IsArray() || json.Size() != 3) {
            val.reset();
        } else {
            if (!val)
                val = Pin::make();
            val->time = json[0].GetInt64();
            json[1] >> val->value;
            json[2] >> val->quality;
        }
        return json;
    }
    template<class T>
    JsonWriter<T>& operator<<(JsonWriter<T>& os, const Pin& v) {
        os.StartArray();
        os << v.time.dt() << v.value << (uint32_t)v.quality.sc();
        os.EndArray();
        return os;
    }
    template<class WT, class T>
    JsonWriter<WT>& operator<<(JsonWriter<WT>& os, const boost::intrusive_ptr<T>& v) {
        if (v) {
            os << *v;
        } else {
            os.Null();
        }
        return os;
    }

    /*template<class T>
    JsonTextWriter<T>& operator<<(JsonTextWriter<T>& os, const Pin::ptr& v) {
        if(!v) {
            os.Null();
        } else {
            os.StartArray();
            os << v->time ; os << v->value ; os << v->quality;
            os.EndArray();
        }
        return os;
    }*/
}}  // namespace mplc::aggregation
