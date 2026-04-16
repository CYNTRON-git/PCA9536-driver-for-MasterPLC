#pragma once
#include <string>
#include <mplc/vm/item_id.h>

namespace mplc { namespace api {
    struct RefTo : vm::ItemID {
        RefTo() {}
        RefTo(const std::string& id_path): ItemID(id_path), raw(id_path) {}
        RefTo(const ItemID& item): ItemID(item) {}
        std::string raw;
    };
}}  // namespace mplc::api
