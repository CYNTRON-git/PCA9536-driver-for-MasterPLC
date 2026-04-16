#pragma once
#include <mplc/vm/item_id.h>
#include <mplc/item_monitor.h>
#include <msgpack/object_fwd.hpp>

namespace mplc { namespace vm {
    struct TaskItemMonitor {
        virtual ~TaskItemMonitor() = default;
        virtual item_connect_ptr Connect(const ItemID& item,
                                         bool locked,
                                         msgpack::object::with_zone& init_val,
                                         on_changed_fn&& on_change) = 0;
        virtual bool WriteSingleValue(const vm::ItemID& item_id,
                                      const msgpack::object& val,
                                      bool block,
                                      lib::string_view type_name = {}) {
            return false;
        }
        virtual on_changed_fn CreateSignal(const ItemID& item) = 0;
        virtual on_changed2_fn CreateSignal2(const ItemID& item, lib::string_view type_name = {}) = 0;
        virtual void Disconnect(item_connect_ptr hndl) = 0;
        virtual void GetBlocked(std::vector<ItemID>& items) const {}
    };
}}  // namespace mplc::vm
