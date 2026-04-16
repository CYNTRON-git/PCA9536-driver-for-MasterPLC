#pragma once
#include <core/addincmn.h>
#include <mplc/libs/threads.hpp>
#include <mplc/libs/smart_ptr.hpp>
#include "project_tree.h"
#include "item_id.h"
#include "object_id.h"
#include "vm_type.h"
#include "node_typese.h"
#include "vm_task.h"
#include "vm_tree.h"

namespace mplc { namespace vm {

    class VMInfo {
        friend struct Report;
        friend struct Layer;
        friend struct ObjectInstance;
        lib::mutex _sec;

        lib::unordered_map<lib::string_view, const VmType*> types;
        lib::unordered_map<int64_t, VMTask*> tasks;
        lib::unordered_map<lib::string_view, VMAlarmType*> alarms;
        lib::unordered_map<uint64_t, DirTable*> directories;
        std::map<int64_t, const RemotePLC*> remote_plc;
        std::map<lib::string_view, const Report*> reports;
        std::map<int, Layer*> layers;
        std::vector<VMTask*> tasks_idx;
        VmTree vm_tree;
        ProjectTree project_tree;

        void initBaseLayers();
        OpcUa_StatusCode load(const json::Value& json);
        OpcUa_StatusCode validateProjectInfo(const Value& info);

        void clean();
        void loadTypes(const json::Value& json);
        void loadAlarmTypes(const json::Value& json);
        void loadTasks(const json::Value& json);
        void loadDirectories(const json::Value& json);
        void loadControllers(const json::Value& json);
        void addLayer(Layer* layer);
        void addLayer(Layer* layer, int id);
        /* Пока нет поддержки в скаде */
        Layer base, minute, hour, no_cache;
        lib::shared_ptr<Object::UsedLayers> base_layers;
        /*---------------------------*/
        friend class DataAddin;  //::OnConfigEvent;
        int64_t controller_id{-1};
        static VMInfo& instance();

    public:
        VMInfo(const VMInfo&) = delete;
        VMInfo& operator=(const VMInfo&) = delete;
        static OpcUa_StatusCode OnConfigEvent(ConfigProcessorMode mode, ControllerConfig* config);
        typedef std::pair<const PtNode*, std::string> Elem;
        typedef std::pair<const Variable*, std::string> VarElem;
        void addReport(const Report* report);
        const ProjectTree& GetProjectTree() const {
            return project_tree;
        }

        const VmTree& GetVmTree() const {
            return vm_tree;
        }

        auto GetTypes() const -> const decltype(types)& {
            return types;
        }

        Elem Find(const ItemID& item) const {
            return Find(item.id, item.path);
        }
        VarElem FindArchiveVar(const ItemID& item) const {
            return FindArchiveVar(item.id, item.path);
        }
        VarElem FindVar(const ItemID& item) const {
            return FindVar(item.id, item.path);
        }
        ItemID FindArchiveItem(const ItemID& item) const {
            return FindArchiveItem(item.id, item.path);
        }

        lib::shared_ptr<Object::UsedLayers> getBaseLayers() const;
        MPLCSHARE_API VMInfo();
        MPLCSHARE_API virtual ~VMInfo();
        MPLCSHARE_API static const DirTable* GetDirectory(int64_t int64_t);
        MPLCSHARE_API const Layer* GetLayer(int64_t layer_id) const;
        MPLCSHARE_API const Report* GetReport(const std::string& report_name) const;
        MPLCSHARE_API bool Check(const ObjectID& childId, const ObjectID& parentId) const;
        MPLCSHARE_API bool Check(const ItemID& childId, const ItemID& parentId) const;
        MPLCSHARE_API bool Check(int64_t childId, int64_t parentId) const;
        MPLCSHARE_API bool Check(const OpcUa_VariantHlp& childId, const OpcUa_VariantHlp& parentId) const;
        MPLCSHARE_API VarElem FindVar(int64_t item_id, const lib::string_view path = {}) const;
        MPLCSHARE_API VarElem FindArchiveVar(int64_t item_id, lib::string_view path = {}) const;
        MPLCSHARE_API ItemID FindArchiveItem(int64_t item_id, lib::string_view path = {}) const;
        MPLCSHARE_API Elem Find(int64_t item_id, lib::string_view path) const;
        MPLCSHARE_API const PtNode* Find(int64_t item_id) const;
        bool Exists(const ItemID& item_id) const {
            return Exists(item_id.id, item_id.path);
        }
        MPLCSHARE_API bool Exists(int64_t id, lib::string_view vm_path) const;
        MPLCSHARE_API OpcUa_StatusCode GetItemType(OpcUa_BuiltInTypeHlp& type,
                                                   int64_t item_id,
                                                   lib::string_view path = {}) const;
        MPLCSHARE_API ItemID GetItemIdByPath(lib::string_view path) const;
        MPLCSHARE_API ObjectID CreateObjectId(lib::string_view path) const;
        MPLCSHARE_API ObjectID GetObjectId(const ItemID& item_id) const;
        MPLCSHARE_API const VmType* GetVmType(lib::string_view name) const;
        MPLCSHARE_API auto GetAllTypes() const -> const decltype(types)&;
        MPLCSHARE_API const std::string& GetAlarmTypeEventNameByName(const std::string& name) const;
        MPLCSHARE_API const struct VMAlarmType* GetAlarmTypeByName(const std::string& name) const;
        MPLCSHARE_API OpcUa_StatusCode ParseVMInfoFile(const std::string& utf8_file_path);
        MPLCSHARE_API static const VMInfo& GetInstance();
        MPLCSHARE_API const VMTask* GetTaskById(int64_t id) const;
        MPLCSHARE_API const VMTask* GetTaskByIdx(int idx) const;
        MPLCSHARE_API std::vector<const VMLuaTask*> GetLuaTasks() const;
        MPLCSHARE_API std::vector<const VMTask*> GetAllTasks() const;
        MPLCSHARE_API std::vector<const VMRemoteTask*> GetRemoteTasks() const;
        MPLCSHARE_API std::map<int64_t, const RemotePLC*> GetRemoteControllers() const {
            return remote_plc;
        }
        MPLCSHARE_API const RemotePLC* FindController(int64_t id) const {
            auto it = remote_plc.find(id);
            return it == remote_plc.end() ? nullptr : it->second;
        }
        MPLCSHARE_API const VmType* CreateLuaRef(const ItemID& item_id, VmPath& lua_path) const;
        int64_t GetControllerId() const {
            return controller_id;
        }
        /// offset   - смещение вверх по дереву относительно node. 0 - путь включает имя этого узла.
        /// use_mask - использовать offset как маску для типов
        ///     false: смещение вверх на offset елементов.
        ///     true:  Ищет первого родителя любого типа из offset
        /// level:     если >= 0 - задает максимальную уровень в дереве относительно node + offset.
        ///                0:    только имя node + offset
        ///            если < 0  путь начиная от корня.
        ///               -1:    Root
        ///               -2:    Объекты/Система
        ///               -3:    АРМ/Объект  и т.д
        MPLCSHARE_API std::string GetProjectPath(int64_t id,
                                                 lib::string_view vm_path,
                                                 int level = 0,
                                                 int offset = 0,
                                                 bool use_mask = false) const;
        MPLCSHARE_API std::string GetProjectPath(int64_t id,
                                                 int level = 0,
                                                 int offset = 0,
                                                 bool use_mask = false) const {
            return GetProjectPath(id, "", level, offset, use_mask);
        }
        // MPLCSHARE_API void AddType(const std::string& name, const VMType& type);
    };

    inline VMInfo::Elem FindNode(int64_t item_id, const std::string& path = "") {
        return VMInfo::GetInstance().Find(item_id, path);
    }
    inline VMInfo::Elem FindNode(const ItemID& item) {
        return FindNode(item.id, item.path);
    }
    inline VMInfo::VarElem FindVar(int64_t item_id, const std::string& path = "") {
        return VMInfo::GetInstance().FindVar(item_id, path);
    }
    inline VMInfo::VarElem FindVar(const ItemID& item) {
        return FindVar(item.id, item.path);
    }
    inline VMInfo::VarElem FindArchiveVar(int64_t item_id, const std::string& path = "") {
        return VMInfo::GetInstance().FindArchiveVar(item_id, path);
    }
    inline ItemID FindArchiveItem(int64_t item_id, const std::string& path = "") {
        return VMInfo::GetInstance().FindArchiveItem(item_id, path);
    }
    inline VMInfo::VarElem FindArchiveVar(const ItemID& item) {
        return FindArchiveVar(item.id, item.path);
    }
    inline ItemID FindArchiveItem(const ItemID& item) {
        return FindArchiveItem(item.id, item.path);
    }

    /// offset   - смещение вверх по дереву относительно node. 0 - путь включает имя этого узла.
    /// use_mask - использовать offset как маску для типов
    ///     false: смещение вверх на offset елементов.
    ///     true:  Ищет первого родителя любого типа из offset
    /// level:     если >= 0 - задает максимальную уровень в дереве относительно node + offset.
    ///                0:    только имя node + offset
    ///            если < 0  путь начиная от корня.
    ///               -1:    Root
    ///               -2:    Объекты/Система
    ///               -3:    АРМ/Объект  и т.д
    MPLCSHARE_API std::string GetProjectPath(int64_t id, int level = 0, int offset = 0, bool use_mask = false);
    MPLCSHARE_API std::string GetProjectPath(int64_t id,
                                             lib::string_view vm_path,
                                             int level = 0,
                                             int offset = 0,
                                             bool use_mask = false);
    /**
     * \brief Возвращает тип элемента в дереве по Id, Path
     * \param type Возвращаемое значение для описания типа параметра
     * \param item_id ID параметра или ObjectInstance
     * \param path Путь в ObjectInstance
     * \return OpcUa_Good если нашёл и OpcUa_BadNodeIdUnknown если нет
     */
    inline OpcUa_StatusCode GetItemType(OpcUa_BuiltInTypeHlp& type, int64_t item_id, lib::string_view path = {}) {
        return VMInfo::GetInstance().GetItemType(type, item_id, path);
    }

    /**
     * \brief Возвращает тип элемента в дереве по Id, Path
     * \param type Возвращаемое значение для описания типа параметра
     * \param item_id Пара ID,Path для параметра в дереве
     * \return OpcUa_Good если нашёл и OpcUa_BadNodeIdUnknown если нет
     */
    inline OpcUa_StatusCode GetItemType(OpcUa_BuiltInTypeHlp& type, const ItemID& item_id) {
        return VMInfo::GetInstance().GetItemType(type, item_id.id, item_id.path);
    }
    MPLCSHARE_API void to_graphviz(const vm::VMInfo& info, std::ostream& os);
}}  // namespace mplc::vm
