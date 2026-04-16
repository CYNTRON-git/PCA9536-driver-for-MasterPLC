#pragma once

#include <string>
#include <msgpack.hpp>

namespace mplc::data {

    struct SkippedIntervalUniqueKey {
        int64_t item_id = 0;
        std::string path;
        int layer = 0;
        int64_t t_start = 0;
    };

    inline bool operator==(const SkippedIntervalUniqueKey& f, const SkippedIntervalUniqueKey& s) noexcept {
        return f.item_id == s.item_id && f.path == s.path && f.layer == s.layer && f.t_start == s.t_start;
    }

    inline std::size_t hash_value(const SkippedIntervalUniqueKey& key) noexcept {
        std::size_t seed;
        boost::hash_combine(seed, key.item_id);
        boost::hash_combine(seed, key.path);
        boost::hash_combine(seed, key.layer);
        boost::hash_combine(seed, key.t_start);
        return seed;
    }

    // Часть запроса slave-а на перекачку интервала резервируемых данных для конкретного item-а и layer-а.
    class RequestItem {
    public:
        int64_t item_id = 0;
        std::string path;
        int layer = 0;
        int64_t t_start = 0;
        int64_t t_end = 0;

        RequestItem() = default;

        RequestItem(int64_t item_id, std::string path, int layer, int64_t t_start, int64_t t_end)
            : item_id(item_id), path(std::move(path)), layer(layer), t_start(t_start), t_end(t_end) {}

        RequestItem(const SkippedIntervalUniqueKey& key, int64_t t_end)
            : item_id(key.item_id), path(key.path), layer(key.layer), t_start(key.t_start), t_end(t_end) {}

        MSGPACK_DEFINE(item_id, path, layer, t_start, t_end)
    };
}  // namespace mplc::data
