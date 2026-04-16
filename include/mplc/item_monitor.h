#pragma once
#include <vector>
#include <msgpack/object_fwd.hpp>
#include <share/config.h>
#include <mplc/signal.h>
#include <mplc/vm/item_id.h>
// #include <mplc/outcome.hpp>

namespace mplc {
    struct ShareAddin;
}

namespace mplc { namespace vm {
    struct TaskItemMonitor;
    using on_changed_t = void(const msgpack::object&);
    using on_changed_fn = lib::function<on_changed_t>;
    using on_changed2_fn = lib::function<void(const msgpack::object&, bool)>;
    using on_changed_signal_t = signal::signal<on_changed_t>;
    using on_blocked_signal_t = signal::signal<void(const ItemID&, bool)>;
    using on_blocked_fn = lib::function<void(const ItemID&, bool)>;
    struct on_blocked_con {
        signal::connection con;
    };
    enum class WriteOperation { oNone = 0, oConcat = 1, oAdd = 2, oMul = 3, oDiv = 4, oSub = 5 };
    using item_slot_fn = lib::function<void(const msgpack::object&, WriteOperation)>;
    using item_connect_ptr = signal::connection;
    // using con_res_t = outcome_v2::result<on_changed_connect_t, OpcUa_StatusCode>;

    class item_hndl {
    public:
        MPLCSHARE_API item_hndl(item_connect_ptr hndl, lib::shared_ptr<TaskItemMonitor> monitor);
        MPLCSHARE_API ~item_hndl();
        MPLCSHARE_API void set_lock(bool lock);
        MPLCSHARE_API bool is_lock();
        MPLCSHARE_API void disconnect();

    private:
        item_connect_ptr hndl;
        lib::weak_ptr<TaskItemMonitor> monitor;
    };
    using item_hndl_ptr = lib::shared_ptr<item_hndl>;
    MPLCSHARE_API item_hndl_ptr connect_item(const vm::ItemID& item,
                                             msgpack::object::with_zone& init_val,
                                             on_changed_fn&& on_changed);
    MPLCSHARE_API item_hndl_ptr connect_item(const vm::ItemID& item,
                                             bool locked,
                                             msgpack::object::with_zone& init_val,
                                             on_changed_fn&& slot);
    MPLCSHARE_API item_hndl_ptr connect_item(const vm::ItemID& item,
                                             bool locked,
                                             int64_t task_idx,
                                             msgpack::object::with_zone& init_val,
                                             on_changed_fn&& slot);
    struct ItemMonitor {
        MPLCSHARE_API item_hndl_ptr Connect(const vm::ItemID& item_id,
                                            bool locked,
                                            int64_t task_idx,
                                            msgpack::object::with_zone& init_val,
                                            on_changed_fn&& on_changed);
        MPLCSHARE_API on_changed_fn CreateSignal(const vm::ItemID& item_id, int64_t task_idx);
        MPLCSHARE_API on_changed2_fn CreateSignal2(const vm::ItemID& item_id, int64_t task_idx = 0, lib::string_view type_name = {});

        MPLCSHARE_API bool WriteSingleValue(const vm::ItemID& item_id,
                                            const msgpack::object& val,
                                            bool block,
                                            int task_idx = -1,
                                            lib::string_view type_name = {});
        MPLCSHARE_API signal::connection GetBlocked(std::vector<ItemID>& items, on_blocked_fn&& on_blocked = {});
        MPLCSHARE_API void Disconnect(signal::connection&&);

    private:
        friend ShareAddin;
        mutable lib::recursive_mutex mtx;
        std::vector<lib::shared_ptr<TaskItemMonitor>> task_monitors;
        lib::shared_ptr<TaskItemMonitor> global_vars_monitor;
        lib::shared_ptr<TaskItemMonitor> GetMonitor(const vm::ItemID& item_id);
        on_blocked_signal_t on_blocked;
        // lib::function<void(const msgpack::object&)> CreateSignal(const vm::ItemID& item_id);
        void init();
        void clear();
    };
}}  // namespace mplc::vm
