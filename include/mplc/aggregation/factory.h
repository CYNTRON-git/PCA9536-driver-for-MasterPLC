#pragma once

#include "functions.h"

namespace mplc { namespace aggregation {

    class Factory {
        Factory() = delete;
        Factory(const Factory&) = delete;
        Factory operator=(const Factory&) = delete;
        Factory(Factory&&) noexcept = delete;
        Factory operator=(Factory&&) = delete;

    public:
        MPLC_AGGREAGATION_API static Interface* get(const Value& params);
        MPLC_AGGREAGATION_API static Interface* get(Type type, SubType sub_type);
    };

}}  // namespace mplc::aggregation
