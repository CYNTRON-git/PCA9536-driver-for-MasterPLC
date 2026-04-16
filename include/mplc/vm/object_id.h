#pragma once
#include <cstdint>
#include <string>
#include <share/addins_share.h>
#include <mplc/libs/string_view.hpp>

namespace mplc { namespace vm {
    struct ItemID;
    struct PtNode;
    /**
     * \brief Хранит базовый id и путь в проекте
     */
    struct ObjectID {
        int64_t id;
        std::string path;
        ObjectID(): id(0) {}
        ObjectID(const ObjectID&) = default;
        ObjectID(ObjectID&&) = default;

        ObjectID& operator=(const ObjectID&) = default;
        ObjectID& operator=(ObjectID&&) = default;

        bool operator==(const ObjectID& rvl) const {
            return id == rvl.id && path == rvl.path;
        }
        bool operator!=(const ObjectID& rvl) const {
            return !(*this == rvl);
        }
        MPLCSHARE_API ObjectID(int64_t id, lib::string_view path = {});
        MPLCSHARE_API bool contains(const ObjectID& obj) const;
        MPLCSHARE_API bool contains(const ItemID& obj) const;


        MPLCSHARE_API ObjectID(lib::string_view id_path);

        MPLCSHARE_API ObjectID(const std::pair<const PtNode*, std::string>& elem);

        bool operator<(const ObjectID& rhs) const {
            return id < rhs.id || (!(rhs.id < id) && path < rhs.path);
        }
        MPLCSHARE_API std::string to_string() const;

        friend std::size_t hash_value(const ObjectID& object_id) {
            using boost::hash_value;
            return hash_value(object_id.id) ^ hash_value(object_id.path);
        }
    };
}}
