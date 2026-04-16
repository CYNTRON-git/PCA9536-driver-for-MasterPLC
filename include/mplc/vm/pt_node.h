#pragma once
#include <string>
#include <vector>

#include <mplc/libs/string_view.hpp>
#include <mplc/libs/json.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <array>
#include "vm_path.h"
#include "share/config.h"

#pragma warning(disable : 4251)

namespace mplc::vm {
    struct VmType;

    struct MPLCSHARE_API PtNode {
        struct Permissions {
            enum PermissionMask : uint8_t {
                tUndefined = 0,
                tAllow = 1,
                tDeny = 1 << 1,
                tLog = 1 << 2,
                tUndefinedLog = tUndefined | tLog,
                tAllowLog = tAllow | tLog,
                tDenayLog = tDeny | tLog,
            };
            enum MPLCSHARE_API Operation : uint8_t { tRead, tWrite, tExecute };
            using ptr = lib::intrusive_ptr<Permissions>;

            MPLCSHARE_API uint8_t get_permission(const std::string& name, Operation op);

        private:
            boost::unordered_flat_map<std::string, std::array<uint8_t, 3>> permissions_;

            mutable uint32_t refcount_{};
            friend void intrusive_ptr_add_ref(const Permissions* p);
            friend void intrusive_ptr_release(const Permissions* p);

            friend struct PtNode;
            static ptr make(const json::Value& json, const ptr& parent_permissions);
            Permissions(const json::Value& json, const ptr& parent_permissions);
        };

        using childs_t = std::vector<const PtNode*>;

        enum Type : uint32_t {
            tNone = 0,
            tVariable = 0x1,
            tFolder = 0x1 << 1,
            tReport = 0x1 << 2,
            tObjectInstance = 0x1 << 3,
            tLayer = 0x1 << 4,
            tIOModule = 0x1 << 5,
            tChannel = 0x1 << 6,
            tTag = 0x1 << 7,
            tObject = 0x1 << 8,
            tProtocol = 0x1 << 9,
            tController = 0x1 << 10,
            tChannelGroup = 0x1 << 11,
            tStructField = 0x1 << 12,
            tArrayField = 0x1 << 13,
            tProgram = 0x1 << 14,
            tFreeObject = 0x1 << 15,
            tFBInstance = 0x1 << 16,
        };
        // Оптимизированное хранение имён
        // Все имена заканчиваются терминирующим \0
        // Порядок расположения в памяти
        // [desc]|<utf>|<>
        class MPLCSHARE_API Name {
            char* buf{};
            uint16_t m_desc_end{};
            uint16_t m_utf_end{};
            uint16_t m_translited_end{};
            uint16_t m_lua_end{};

        public:
            enum Type { tUtf8, tTranslited, tLua, tDesc };
            Name(Name&& rv) noexcept;
            Name(const Name& lv);
            Name(const json::Value& json);
            Name(lib::string_view utf,
                 lib::string_view translited = {},
                 lib::string_view lua = {},
                 lib::string_view desc = {});
            Name(const char* utf): Name(lib::string_view(utf)) {}
            ~Name();

            Name& operator=(Name&& rv) noexcept;
            Name& operator=(const Name& lv);

            void swap(Name& lv) noexcept;
            void init(lib::string_view utf,
                      lib::string_view translited = {},
                      lib::string_view lua = {},
                      lib::string_view desc = {});

            bool has_lua_name() const;
            lib::string_view utf8() const;
            lib::string_view translited() const;
            lib::string_view lua() const;
            lib::string_view desc() const;

            lib::string_view get(Type type) const;
        };
        template<class T = PtNode>
        class iterator {
            friend struct PtNode;
            iterator(const childs_t& nodes, uint32_t mask = 0, size_t pos = 0);

            void next();
            bool test() const;

            const PtNode* curent() const {
                if (data)
                    return (*data)[pos];
                return nullptr;
            }

        public:
            typedef childs_t::value_type value_type;
            typedef childs_t::pointer pointer;
            typedef childs_t::reference reference;
            typedef childs_t::difference_type difference_type;
            typedef std::forward_iterator_tag iterator_category;

            bool operator!=(const iterator& it) const;

            bool operator==(const iterator& it) const;

            bool operator!=(const childs_t::const_iterator& it) const;

            bool operator==(const childs_t::const_iterator& it) const;

            const T* operator*() const;

            const T* operator->() const;

            iterator& operator++();
            iterator(const iterator<T>& it);
            iterator(iterator<T>&& it) noexcept;
            iterator& operator=(const iterator<T>& it);
            iterator& operator=(iterator<T>&& it) noexcept;
            ~iterator() = default;

        private:
            // const T* last;  // cache for dynamic_cast
            // const Childs& nodes_;
            const childs_t* data;
            size_t pos;
            uint32_t mask_;
        };
        template<class T>
        struct filter {
            const childs_t& m_childs;
            uint32_t mask;
            filter(const childs_t& childs, uint32_t mask): m_childs(childs), mask(mask) {}
            iterator<T> begin() const {
                return iterator<T>(m_childs, mask);
            }
            iterator<T> end() const {
                return iterator<T>(m_childs, mask, m_childs.size());
            }
            /*childs_t::const_iterator end() const {
                return m_childs.end();
                iterator<T>(m_childs, mask, m_childs.size());
            }*/
        };

        PtNode(const PtNode&) = delete;
        PtNode& operator=(const PtNode&) = delete;
        PtNode(Name name = "Root", Type type = tNone, PtNode* parent = nullptr, int64_t id = 0, int task_idx = 0);
        PtNode(const json::Value& json, PtNode* parent, Type type);
        virtual ~PtNode();

        virtual bool hasName(lib::string_view name, Name::Type find_by = Name::tUtf8) const;
        void initPermissions(const json::Value& json);
        Permissions::ptr get_user_permissions() const;
        Permissions::ptr get_group_permissions() const;

        const PtNode* ChildByName(lib::string_view name) const;
        const PtNode* ChildByTranslitedName(lib::string_view translitedName) const;
        lib::string_view getTranslitedName() const {
            return name.translited();
        }
        const PtNode* find_parent(int64_t parentId) const;
        const PtNode* find(int64_t id) const;
        virtual const PtNode* find(lib::string_view child, Name::Type find_by = Name::tUtf8) const;

        virtual const PtNode* find(const char* child, Name::Type find_by = Name::tUtf8) const {
            return find(lib::string_view(child), find_by);
        }
        template<class T>
        const T* find(int64_t id) const {
            return dynamic_cast<const T*>(find(id));
        }
        template<class Pred>
        const PtNode* find_by(Pred pred) const {
            const PtNode* res{};
            for (auto child: childs()) {
                if (pred(child)) {
                    res = child;
                    break;
                }
            }
            if (!res) {
                return nullptr;
            }
            return res;
        }

        template<class HPred, class... TPred>
        const PtNode* find_by(HPred h_pred, TPred... t_tpred) const {
            auto res = find_by(std::forward<HPred>(h_pred));
            return res == nullptr ? nullptr : res->find_by(std::forward<TPred>(t_tpred)...);
        }
        virtual const childs_t& all_childs() const {
            static const childs_t NONE;
            return NONE;
        }
        const childs_t& childs() const {
            return all_childs();
        }
        filter<PtNode> childs(uint32_t type_mask) const {
            return filter<PtNode>(childs(), type_mask);
        }
        template<class T>
        filter<T> childs(uint32_t type_mask = 0) const {
            return filter<T>(childs(), type_mask);
        }

        virtual void insert(const PtNode* child) {}
        childs_t GetList(int level = 0, int offset = 0, bool use_mask = false) const;

        const PtNode* parentLike(uint32_t mask) const;
        template<class T>
        const T* parentLike() const {
            const PtNode* tmp = parent;
            while (tmp) {
                if (dynamic_cast<const T*>(tmp)) {
                    return static_cast<const T*>(tmp);
                }
                tmp = tmp->parent;
            }
            return nullptr;
        }
        std::string project_path(Name::Type type = Name::tUtf8,
                                 int level = 0,
                                 int offset = 0,
                                 bool use_mask = false) const;
        Name name;
        PtNode* parent{};
        int64_t id{};
        Type type{};
        int task_idx{};

    private:
        Permissions::ptr user_permissions;
        Permissions::ptr group_permissions;
    };

    // -------------------- Node::iterator

    template<class T>
    PtNode::iterator<T>::iterator(const std::vector<const PtNode*>& nodes, uint32_t mask, size_t pos)
        : data(&nodes), pos(pos), mask_(mask) {
        if (pos < data->size() && !test()) {
            next();
        }
    }
    template<>
    inline bool PtNode::iterator<PtNode>::test() const {
        return !mask_ || (curent()->type & mask_);
    }

    template<class T>
    bool PtNode::iterator<T>::test() const {
        if (mask_ && (curent()->type & mask_) == 0)
            return false;
        return dynamic_cast<const T*>(curent()) != nullptr;
    }

    template<class T>
    void PtNode::iterator<T>::next() {
        while (++pos < data->size() && !test()) {
        }
    }

    template<class T>
    bool PtNode::iterator<T>::operator!=(const iterator& it) const {
        return !operator==(it);
    }

    template<class T>
    bool PtNode::iterator<T>::operator==(const iterator& it) const {
        return data->begin() + pos == it.data->begin() + it.pos;
    }

    template<class T>
    bool PtNode::iterator<T>::operator!=(const childs_t::const_iterator& it) const {
        return !operator==(it);
    }

    template<class T>
    bool PtNode::iterator<T>::operator==(const childs_t::const_iterator& it) const {
        return data->begin() + pos == it;
    }

    template<class T>
    const T* PtNode::iterator<T>::operator*() const {
        return static_cast<const T*>(curent());
    }

    template<class T>
    const T* PtNode::iterator<T>::operator->() const {
        return static_cast<const T*>(curent());
    }

    template<class T>
    PtNode::iterator<T>& PtNode::iterator<T>::operator++() {
        next();
        return *this;
    }

    template<class T>
    PtNode::iterator<T>::iterator(const iterator<T>& it): data(it.data), pos(it.pos), mask_(it.mask_) {}

    template<class T>
    PtNode::iterator<T>::iterator(iterator<T>&& it) noexcept: data(it.data), pos(it.pos), mask_(it.mask_) {
        it.data = nullptr;
        it.pos = 0;
        it.mask_ = 0;
    }

    template<class T>
    PtNode::iterator<T>& PtNode::iterator<T>::operator=(const iterator<T>& it) {
        data = it.data;
        pos = it.pos;
        mask_ = it.mask_;
        return *this;
    }

    template<class T>
    PtNode::iterator<T>& PtNode::iterator<T>::operator=(iterator<T>&& it) noexcept {
        data = it.data;
        pos = it.pos;
        mask_ = it.mask_;
        it.data = nullptr;
        it.pos = 0;
        it.mask_ = 0;
        return *this;
    }
}  // namespace mplc::vm

#pragma warning(default : 4251)
