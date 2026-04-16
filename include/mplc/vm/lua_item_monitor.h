#pragma once
#include "task_item_monitor.h"
namespace mplc { namespace vm {
    struct VMLuaTask;
    lib::shared_ptr<TaskItemMonitor> CreateLuaItemMonitor(const VMLuaTask* task, on_blocked_signal_t* on_blocked = nullptr);

    /*class GlobalItemMonitor : public LuaItemMonitor {
        static constexpr const char* params = "PARAMS";

    public:
        static lib::shared_ptr<GlobalItemMonitor> Create(const VMTask* task);
    };*/
}}  // namespace mplc::vm
