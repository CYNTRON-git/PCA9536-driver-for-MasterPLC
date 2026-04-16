#pragma once

#include <string>
#include <mplc/msgpack_ext.hpp>
#include <mplc/aggregation.hpp>
#include <mplc/msgpack/pack_pin.hpp>

namespace mplc::data {
    /// Часть ответа master-а на запрос перекачки резервируемых данных для кокретного item-а и layer-а.
    class ResponseItem {
    public:
        int64_t item_id = 0;
        std::string path;
        OpcUa_StatusCode status_code{};
        PinVector data;
        int layer{};
        int64_t t_start{};

        ResponseItem() = default;

        ResponseItem(int64_t item_id,
                     std::string path,
                     OpcUa_StatusCode status_code,
                     PinVector data,
                     int layer,
                     int64_t t_start)
            : item_id(item_id), path(std::move(path)), status_code(status_code), data(std::move(data)), layer(layer),
              t_start(t_start) {}

        MSGPACK_DEFINE(item_id, path, status_code, data, layer, t_start)
    };
}  // namespace mplc::data
