#pragma once
#include "share/config.h"

namespace mplc::api {

    class task_interrupted {};

    MPLCSHARE_API void interruption_point() noexcept(false);
    MPLCSHARE_API bool task_stop_requested() noexcept;

}  // namespace mplc::api
