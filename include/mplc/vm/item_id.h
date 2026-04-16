#pragma once
#include <cstdint>
#include <string>
#include <share/addins_share.h>
#include <mplc/libs/string_view.hpp>
// #include "pt_node.h"

namespace mplc::vm {
    struct ObjectID;
}

namespace mplc { namespace vm {
    struct PtNode;
    /**
     * \brief Хранит базовый id и путь lua
     */
    struct ItemID {
        int64_t id;
        std::string path;
        ItemID(): id(0) {}
        ItemID(const ItemID&) = default;
        ItemID(ItemID&&) = default;

        ItemID& operator=(const ItemID&) = default;
        ItemID& operator=(ItemID&&) = default;

        bool operator==(const ItemID& rvl) const {
            return id == rvl.id && path == rvl.path;
        }
        bool operator!=(const ItemID& rvl) const {
            return !(*this == rvl);
        }
        MPLCSHARE_API ItemID(int64_t id, lib::string_view path = {});

        MPLCSHARE_API ItemID(lib::string_view id_path);

        MPLCSHARE_API ItemID(const std::pair<const PtNode*, std::string>& elem);

        bool operator<(const ItemID& rhs) const {
            return id < rhs.id || (!(rhs.id < id) && path < rhs.path);
        }
        MPLCSHARE_API std::string to_string() const;
        MPLCSHARE_API ObjectID to_object_id() const;

        friend std::size_t hash_value(const ItemID& item_id) {
            using boost::hash_value;
            return hash_value(item_id.id) ^ hash_value(item_id.path);
        }
    };

}}  // namespace mplc::vm
