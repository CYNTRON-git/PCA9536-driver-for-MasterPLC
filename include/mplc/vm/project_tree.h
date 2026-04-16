#pragma once
#include <cstdint>
#include <mplc/libs/unordered_map.hpp>
#include <mplc/libs/smart_ptr.hpp>
#include <mplc/libs/json.hpp>
#include <mplc/libs/string_view.hpp>

#include <share/config.h>

#include "item_id.h"
#include "object_id.h"
#include "pt_node.h"
#include "vm_path.h"

namespace mplc { namespace vm {
    struct Folder;
    class VMInfo;
    struct VmType;
    struct PtNode;

    class ProjectTree {
        lib::unordered_map<int64_t, const PtNode*> all_nodes;
        lib::unique_ptr<Folder> m_root;

    public:
        enum Level {
            lName = 0,
            lProject = -1,  //(Объекты / Система)
        };
        void Load(const json::Value& projectTree, VMInfo& vminfo);
        const Folder* Root() const {
            return m_root.get();
        }
        auto AllNodes() const -> const decltype(all_nodes)& {
            return all_nodes;
        }

        MPLCSHARE_API const PtNode* find(int64_t node_id) const;

        /**
         * \brief
         * \param node_id Начальный id элемента в дереве
         * \param path Путь относительно начального item_id
         * \param name_type Тип пути Utf8/Translited
         * \return ItemId Содержащий ID и LuaPath для поиска в VmTree
         */
        MPLCSHARE_API ItemID CreateItemId(int64_t node_id,
                                          lib::string_view path,
                                          PtNode::Name::Type name_type = PtNode::Name::tUtf8) const;
        MPLCSHARE_API ObjectID CreateObjectId(int64_t node_id,
                                              lib::string_view path) const;

        void clear() {
            all_nodes.clear();
            m_root.reset();
        }
    };
}}  // namespace mplc::vm
