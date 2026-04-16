#pragma once
#include "aggregation/aggregation_config.h"
#include "aggregation/enums.h"
#include "aggregation/filter.h"
#include "aggregation/quality.h"
#include "aggregation/pin.h"
#include "aggregation/base.h"
#include "aggregation/functions.h"
#include "aggregation/factory.h"

namespace mplc {
    namespace aggregation {
        struct Config {
            Type type;
            SubType sub_type;
            SampleInterval interval;  // in FT_MILLISECOND
            // int64_t delta;              // offset concerning start_time if start_time % resemple_interval != 0

            /*
             * Good = 0 bit, Stop = 1 bit, Bad = 2 bit, Uncertain = 3 bit
             * example : 00 00 11 11 | 1111 | 11 10 01 00
             * quality : 8; | convert quality options (11 10 01 00) order and type set in Quality
             * ignore    : 4; | ignore quality options, if set 0 then isPeek return true
             * return false
             * options : 8; | additional options for quality if set not 0 in quality
             * position where ignore opt set 1 then isIgnore return false;
             */
            uint32_t filter;  // binary flags
            // enum UsePrevIntervalValue
            int use_prev_interval_val;
            Config()
                : type(Type::LastValue), sub_type(0),  // delta(0),
                  filter(0x000fbcf4), use_prev_interval_val(0)
            /* (11 11 10 11 | 1100 | 11 11 01 00) (no convert quality peek all) */
            {}
        };
        typedef lib::shared_ptr<Interface> proc;
        proc MPLC_AGGREAGATION_API make_proc(const Config& conf);

    }  // namespace aggregation
    typedef aggregation::Pin PinValue;
    typedef std::list<PinValue::ptr> PinList;
    typedef std::vector<PinValue::ptr> PinVector;
}  // namespace mplc
