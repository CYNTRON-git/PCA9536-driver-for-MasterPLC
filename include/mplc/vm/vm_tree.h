#pragma once
#include <share/config.h>
#include <set>
#include <mplc/libs/unordered_map.hpp>

namespace mplc::vm {
    struct PtNode;
    class ProjectTree;
    class VmTree {
    public:
        struct Tree;
        enum Type { none, tLeaf, tTree };
        struct MPLCSHARE_API Leaf {
            Leaf(const Tree* parent = nullptr, const PtNode* node = nullptr): node(node), parent(parent) {}
            struct Parents;
            class parents_iterator {
                friend struct Parents;
                parents_iterator(const Leaf* leaf): leaf(leaf), node(leaf->node->parent) {}
                parents_iterator(): leaf(nullptr), node(nullptr) {}

            public:
                parents_iterator(const parents_iterator& it) = default;
                parents_iterator(parents_iterator&& it) noexcept = default;
                parents_iterator& operator=(const parents_iterator& it) = default;
                parents_iterator& operator=(parents_iterator&& it) noexcept = default;
                ~parents_iterator() = default;
                typedef PtNode value_type;
                typedef PtNode* pointer;
                typedef PtNode& reference;
                typedef std::forward_iterator_tag iterator_category;

                bool operator!=(const parents_iterator& it) const {
                    return it.node != node;
                }

                bool operator==(const parents_iterator& it) const {
                    return it.node == node;
                }

                const PtNode& operator*() const {
                    return *node;
                }

                const PtNode& operator->() const {
                    return *node;
                }

                parents_iterator& operator++() {
                    if (node == leaf->parent_node() || node->type == PtNode::tFreeObject) {
                        leaf = leaf->parent;
                        node = leaf->node;
                    }
                    node = node->parent;
                    if (node && node->type == PtNode::tFreeObject) {
                        node = leaf->parent_node();
                    }
                    return *this;
                }

            private:
                const Leaf* leaf{};
                const PtNode* node{};
            };
            struct Parents {
                Parents(const Leaf& leaf): m_begin(&leaf) {}
                parents_iterator begin() const {
                    return m_begin;
                }
                parents_iterator end() const {
                    return m_end;
                }
                /*childs_t::const_iterator end() const {
                    return m_childs.end();
                    iterator<T>(m_childs, mask, m_childs.size());
                }*/
            private:
                parents_iterator m_begin;
                parents_iterator m_end;
            };

            lib::string_view lua_name() const;

            virtual const Tree* tree() const {
                return nullptr;
            }
            const Leaf* leaf() const {
                return this;
            }
            const PtNode* pt_node() const {
                return node;
            }
            Type type() const {
                return m_type;
            }
            const Leaf* get_child(lib::string_view vm_path) const;
            const PtNode* parent_node() const;
            // std::string projectPath(PtNode::Name::Type type = PtNode::Name::tUtf8) const;
            std::string project_path(PtNode::Name::Type type = PtNode::Name::tUtf8,
                                     int level = 0,
                                     int offset = 0,
                                     bool use_mask = false) const;
            Parents parents() const {
                return Parents(*this);
            }

            virtual ~Leaf() {}
            const PtNode* node{};
            const Tree* parent{};

            uint8_t get_user_permissions(const std::string& name, PtNode::Permissions::Operation op) const;
            uint8_t get_group_permissions(const std::string& name, PtNode::Permissions::Operation op) const;

        protected:
            Type m_type{tLeaf};
        };

        struct Tree : Leaf {
            std::vector<const Leaf*> childs;
            // using iterator = std::vector<const Leaf*>::iterator;
            Tree(Tree* parent = nullptr, const PtNode* node = nullptr): Leaf(parent, node) {
                m_type = tTree;
            }
            ~Tree() override {
                for (auto& child: childs) {
                    delete child;
                }
            }
            auto lower_bound(lib::string_view name) -> decltype(childs.begin()) {
                return std::lower_bound(childs.begin(),
                                        childs.end(),
                                        name,
                                        [](const Leaf* lv, const lib::string_view& rv) { return lv->lua_name() < rv; });
            }
            auto lower_bound(lib::string_view name) const -> decltype(childs.begin()) {
                return std::lower_bound(childs.begin(),
                                        childs.end(),
                                        name,
                                        [](const Leaf* lv, const lib::string_view& rv) { return lv->lua_name() < rv; });
            }
            auto insert(Leaf* leaf) -> std::pair<decltype(lower_bound("")), bool> {
                auto it = lower_bound(leaf->lua_name());
                if (it == childs.cend() || (*it)->lua_name() != leaf->lua_name()) {
                    return std::make_pair(childs.insert(it, leaf), true);
                }
                return std::make_pair(it, false);
            }
            const Leaf* child(lib::string_view name) const {
                auto it = lower_bound(name);
                if (it != childs.cend() && (*it)->lua_name() == name) {
                    return *it;
                }
                return nullptr;
            }
            const Tree* tree() const override {
                return this;
            }
        };

        MPLCSHARE_API VmTree() {}
        MPLCSHARE_API ~VmTree() {}
        MPLCSHARE_API void Load(const ProjectTree& vm_info);
        MPLCSHARE_API const Leaf* find(int64_t id) const;
        MPLCSHARE_API const Leaf* find(int64_t id, lib::string_view vm_path) const;
        MPLCSHARE_API const Leaf* find(const ItemID& item_id) const;
        MPLCSHARE_API const Leaf* find_leaf(int64_t id, lib::string_view& vm_path) const;
        MPLCSHARE_API const Variable* find_var(int64_t id, lib::string_view& vm_path) const;

        void clear() {
            all_nodes.clear();
            m_root.reset();
        }
        const Tree* root() const {
            return m_root.get();
        }

    private:
        lib::unordered_flat_map<int64_t, Leaf*> all_nodes;
        lib::unordered_flat_map<std::set<int64_t>, Leaf*> obj_inst_nodes;
        lib::unique_ptr<Tree> m_root;
    };
    // lib::unordered_map<int64_t, child_t> vm_nodes;
}  // namespace mplc::vm
