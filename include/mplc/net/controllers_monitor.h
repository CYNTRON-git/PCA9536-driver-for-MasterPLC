#pragma once
#include <share/config.h>
#include <mplc/signal.h>
#include <mplc/vm/vm_task.h>

namespace mplc {
    struct ShareAddin;
}

namespace mplc { namespace net {

    enum class State { sNoResponce, sMaster, sReserv };
    using change_master_cb = lib::function<void(const vm::RemoteMplc&, State)>;
    struct ControllersMonitor final {
        void connect(int64_t id, const change_master_cb& fn, signal::slot* slot);
        vm::RemoteMplc get_master(int64_t controller_id);
        const vm::RemoteMplc* get_controller_info(int64_t id);

        ~ControllersMonitor();
        ControllersMonitor();

    private:
        friend ShareAddin;
        ControllersMonitor(const ControllersMonitor&) = delete;
        ControllersMonitor& operator=(const ControllersMonitor&) = delete;
        ControllersMonitor(ControllersMonitor&&) = delete;
        ControllersMonitor& operator=(ControllersMonitor&&) = delete;

        void start();
        void stop();
        void init();
        struct impl;
        impl* pimpl;
    };
    MPLCSHARE_API void master_changed_event(int64_t controller_id,
                                            const change_master_cb& on_changed,
                                            signal::slot* slot);
    MPLCSHARE_API vm::RemoteMplc get_master_ip(int64_t controller_id);

}}  // namespace mplc::net
