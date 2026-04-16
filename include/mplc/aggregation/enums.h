#pragma once
#include <share/base_macros.h>
//#include "quality.h"

namespace mplc { namespace aggregation {

    struct DataAttribute {
        enum Names { Value = 0, Time = 1, Quality = 2 };
        AddBaseEnumFunctions(DataAttribute, Names, Value)
    };

    struct Type {
        enum Name {
            LastValue = 0,
            FirstValue = 1,
            Integral = 2,
            Average = 3,
            IntegralAverage = 4,
            Max = 5,
            Min = 6,
            Interpolation = 7,
            ExactValue = 8,
            CountSwitchingOn = 9,
            Count = 10,
            Sum = 11,
            Delta = 12,
            LastBoundValue = 13,
            Sampling = 14,
            FirstBoundValue = 15
        };
        AddBaseEnumFunctions(Type, Name, LastValue)
    };

    class SubType {
    public:
        enum Name { LeftRectangle = 0, Tropecia = 1 };
        AddBaseEnumFunctions(SubType, Name, LeftRectangle)
    };

}}  // namespace mplc::aggregation
