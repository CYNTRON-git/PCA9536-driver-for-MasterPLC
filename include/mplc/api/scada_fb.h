#pragma once
#include <share/config.h>
#include "scada_object.h"
#include "scada_module.h"

namespace mplc { namespace api {

    struct MPLCSHARE_API ScadaFB : ScadaObject {
        MPLC_OBJECT(ScadaFB);

        ScadaFB() = default;
        virtual ~ScadaFB() = default;
        void SetEnO(bool v) {
            __EnO = v;
        }
        bool GetEnO() const {
            return __EnO;
        }
        virtual void Init();
        virtual void Execute() = 0;
        int TaskIdx() const {
            return __TaskIdx;
        }
        // Return count of milliseconds
        uint32_t TaskPeriod() const {
            return __TaskPeriod;
        }
        virtual void lock() {}
        virtual void unlock() {}

    protected:
        friend struct ApiStorage;
        int __TaskIdx{-1};
        uint32_t __TaskPeriod{100};
        BOOL __Internal;
        BOOL __EnO;
    };

    struct DynamicFB : ScadaFB {
        // MPLC_OBJECT(DynamicFieldFB);

        /*virtual boost::span<lib::string_view> GetFields() {
            return {};
        }*/
        virtual void SetField(lib::string_view field, msgpack::object_handle&&) = 0;
        virtual msgpack::object_handle GetField(lib::string_view field) const = 0;
        // virtual size_t FieldsCount() = 0;
    };
    struct AsyncFB : ScadaFB {
        using lock_guard = lib::lock_guard<AsyncFB>;
        void lock() override {
            mtx.lock();
        }
        void unlock() override {
            mtx.unlock();
        }
        bool try_lock() noexcept {
            return mtx.try_lock();
        }
    private:
        lib::mutex mtx;
    };

}}  // namespace mplc::api
MPLC_EXPORT_PUBLIC_API(MPLCSHARE_API, mplc::api::ScadaFB);
// MPLC_DEFINE_PUBLIC_API(MPLCSHARE_API, mplc::api::DynamicFieldFB);
