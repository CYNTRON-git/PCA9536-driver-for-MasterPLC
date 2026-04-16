#pragma once
#include <core/addincmn.h>
#include <share/addin_base.h>
#include <mplc/signal.h>
#include <mplc/item_monitor.h>
#include <mplc/net/controllers_monitor.h>
#include <mplc/vm/vm_objects.h>

EXTERN_C MPLCSHARE_API void InitMplcShare();
namespace mplc {
    namespace vm {
        struct ItemMonitor;
    }

    namespace ws {
        struct context_t;
    }
    namespace kvb {
        struct Storage;
    }
    struct ShareAddin : AddinBase {
        ShareAddin();
        ~ShareAddin() override;
        MPLCSHARE_API static ShareAddin& GetInstance();
        MPLCSHARE_API static ws::context_t& GwsCtx();
        MPLCSHARE_API static vm::ItemMonitor& ItemMonitor();
        MPLCSHARE_API static kvb::Storage& SharedStorage();
        MPLCSHARE_API static const vm::VMInfo& VMInfo();
        MPLCSHARE_API static vm::VmObjects& VmObects();
        MPLCSHARE_API static net::ControllersMonitor& ControllersMonitor();

        MPLCSHARE_API void SubscribeConfigEvent(signal::slot& slot, lib::function<void(ConfigProcessorMode)>&& cb);

    protected:
        OpcUa_StatusCode OnConfigEvent(ConfigProcessorMode mode, ControllerConfig* config) override;

    private:
        struct impl* pimpl;
    };

}  // namespace mplc
