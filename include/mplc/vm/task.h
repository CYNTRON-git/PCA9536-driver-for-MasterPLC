#pragma once
#include "task_item_monitor.h"

namespace mplc { namespace vm {

    class Task {
    public:
        virtual ~Task() = default;
        virtual TaskItemMonitor& monitor() = 0;
    };

}}  // namespace mplc::vm
