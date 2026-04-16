#pragma once
#include <ctime>
#include <mplc/database.h>
#include <string>
#include <share/time_device_share.h>
#include <mplc/api/scada_channel.h>
#include "mplc/driver/config.h"

namespace mplc { namespace drv_common { namespace archive_data {

    enum StateRequest { StateRequestOk = 0, StateRequestNoConnect, StateRequestNoData, StateNoPinCache };
    MPLCCOMMON_API PinCache::ptr GetPinCache(const vm::Channel* Channel);
    MPLCCOMMON_API StateRequest RequestLastTime(TimeDevice& Tim, PinCache::ptr PinCache);

}}}  // namespace mplc::drv_common::archive_data
