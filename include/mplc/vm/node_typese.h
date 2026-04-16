#pragma once

#include <mplc/libs/optional.hpp>
#include <mplc/libs/json.hpp>
#include <mplc/libs/threads.hpp>
#include <share/addins_share.h>
#include "pt_node.h"
#include "vm_type.h"
namespace mplc { namespace vm {
    struct ItemID;

    // --------------------

    struct Folder : PtNode {
        std::vector<const PtNode*> m_childs;
        Folder(Name name, PtNode* parent = nullptr, Type type = tFolder): PtNode(std::move(name), type, parent) {}
        Folder(const json::Value& json, PtNode* parent = nullptr, Type type = tFolder): PtNode(json, parent, type) {
            AccessOPCUA = GetSafeBoolValue(json, "AccessOPCUA");
        }
        ~Folder() override {
            for (auto child: m_childs) {
                delete child;
            }
        }
        const childs_t& all_childs() const override {
            return m_childs;
        }
        void insert(const PtNode* child) override {
            m_childs.push_back(child);
        }

        bool AccessOPCUA{};
    };
    struct Controller final : Folder {
        Controller(const json::Value& json, PtNode* parent = nullptr): Folder(json, parent, tController) {
            Comment = GetSafeBoolValue(json, "Comment");
        }
        std::string Comment;
    };

    struct Object : Folder {
        Object(const json::Value& json, PtNode* parent = nullptr, Type type = tFolder);

        // Key Layer::layer_id, json::Value: <first: archive_id, second: max_storage_time (default : 0 (unlimit))>
        typedef std::map<int, std::pair<int, int64_t>> UsedLayers;
        static constexpr int DEFAULT_ARCHIVE_IDX = 0;

        lib::shared_ptr<UsedLayers> layers;
        // TODO! это id или idx?
        int64_t events_archive_id;
        int ArchiveIdx;
        int64_t WindowId;
    };

    struct FreeObject : Object {
        const VmTypeObject* object_type;
        FreeObject(const json::Value& json, const VmTypeObject* parent);
    };
    struct ScaleAI {
        ScaleAI(const json::Value& json);

        std::string getFormat() {
            lib::lock_guard<lib::mutex> lock(mtx);
            return Format;
        }
        void setFormat(const std::string& f) {
            lib::lock_guard<lib::mutex> lock(mtx);
            Format = f;
        }
        std::string getUnit() {
            lib::lock_guard<lib::mutex> lock(mtx);
            return Unit;
        }
        void setUnit(const std::string& u) {
            lib::lock_guard<lib::mutex> lock(mtx);
            Unit = u;
        }
        int64_t id{};
        lib::optional<double> Min;
        lib::optional<double> Max;
        lib::optional<double> Hi;
        lib::optional<double> Lo;
        lib::optional<double> HiHi;
        lib::optional<double> LoLo;

    protected:
        friend struct Variable;
        lib::mutex mtx;
        std::string Unit;  // Unsafe
        std::string Format;
    };

    struct Variable : PtNode {
        MPLCSHARE_API Variable(const json::Value& json, PtNode* parent = nullptr, Type type = tVariable);
        MPLCSHARE_API ~Variable() override;
        MPLCSHARE_API OpcUa_StatusCode GetType(OpcUa_BuiltInTypeHlp& type, lib::string_view path = {}) const;

        bool isStruct() const {
            return vm_type->typeKind == VmType::kStruct || vm_type->typeKind == VmType::kSystemParam;
        }  // Пока Input и InOut кодируются одинаково.

        bool isArray() const {
            return vm_type->typeKind == VmType::kArray;
        }

        enum AccessMode {
            None = 0,
            Input = 3,
            Output = 1,
            InOut = 3,
        };
        lib::string_view scaleName() const {
            return m_scaleName ? m_scaleName : "";
        }
        lib::string_view comment() const {
            return m_comment ? m_comment : "";
        }
        lib::shared_ptr<Object::UsedLayers> layers;
        lib::unique_ptr<ItemID> archiveItem;
        // std::string archiveSourcePath;
        // Init after create
        const VmType* vm_type{};
        ScaleAI* scaleAI{};
        // int64_t archiveSourceId{};
        int ArchiveIdx{};
        uint8_t access{};
        bool isArchived{};
        bool isGlobal{};
        bool isExternal{};
        bool AccessOPCUA{};
        MPLCSHARE_API ItemID getScaleRef() const;

    private:
        char* m_comment{};
        char* m_scaleName{};
    };

    struct IOModule : Object {
        IOModule(const json::Value& json, PtNode* parent = nullptr, Type type = tIOModule);
        OpcUa_VariantHlp get(const std::string& name) const {
            auto it = settings.find(name);
            if (it == settings.end())
                return OpcUa_VariantHlp();
            return it->second;
        }
        std::map<std::string, OpcUa_VariantHlp> settings;
        std::string SubType;
        int64_t ConnectItemId, ExecuteItemId, WriteItemId, ErrorTextItemId, FaultItemId;
        int WriteCondition;
    };

    struct Channel : Object {
        enum Direction { Input, Output, InOut };
        Channel(const json::Value& json, PtNode* parent = nullptr, Type type = tChannel);
        ~Channel() override;
        OpcUa_VariantHlp get(const std::string& name) const {
            auto it = settings.find(name);
            if (it == settings.end())
                return OpcUa_VariantHlp();
            return it->second;
        }
        MPLCSHARE_API double scale(double val) const;
        MPLCSHARE_API double unscale(double val) const;
        bool needScale() const {
            return scaling != nullptr;
        }
        Direction getDirection() const {
            return WriteId ? ReadId ? InOut : Output : Input;
        }
        std::map<std::string, OpcUa_VariantHlp> settings;

        std::string ReadPath, WritePath;
        int64_t ReadId{}, WriteId{};
        int ChannelId{};

    private:
        struct Scaling {
            double source_min{}, source_max{};
            double target_min, target_max;
            Scaling(const json::Value& json);
            MPLCSHARE_API double scale(double val) const;
            MPLCSHARE_API double unscale(double val) const;
        };
        Scaling* scaling{};
    };

    struct ChannelGroup : Object {
        int ChannelGroupId;
        std::map<std::string, OpcUa_VariantHlp> settings;

        ChannelGroup(const json::Value& json, PtNode* parent = nullptr, Type type = tChannelGroup);
    };

    // struct VariableMap;

    struct ObjectInstance : Object {
        explicit ObjectInstance(const json::Value& json, PtNode* parent = nullptr, Type type = tObjectInstance);
        ~ObjectInstance() override;

        // const Variable* findVar(const std::string& lua_name) const;
        // const Variable* findVar(splitter<lib::string_view>::iterator& path_it) const;
        const PtNode* find(lib::string_view child, Name::Type find_by = Name::tUtf8) const override;
        // void OnLoaded(const json::Value& json) override;

        // FreeObject* object{};
        const VmTypeObject* vm_type{};
        // VariableMap* vars{};
    };
    
    struct FBInstance : Object {
        const VmTypeProgram* vm_type{};
        FBInstance(const json::Value& json, PtNode* parent);
    };
    struct Report : PtNode {
        std::string m_template;
        int64_t objectId;

        //  protected:
        friend struct NodeFactory;
        friend class VMInfo;
        Report(const json::Value& json, PtNode* parent = nullptr);
    };
    struct Layer : PtNode {
        // int aggregarion_type;
        // int aggregarion_sub_type;
        // uint32_t filter;
        int64_t interval;
        bool use_empty_intervals;
        bool use_cache;
        int layer_id;

        // protected:
        friend struct NodeFactory;
        friend class VMInfo;
        Layer(/*const json::Value  &json, Node* parent = nullptr*/);
    };
    struct NodeFactory {
        static PtNode* create(const json::Value& json, PtNode* parent = nullptr);
        static PtNode::Type fromString(lib::string_view type);
    };
    struct DirTable;
    struct DirField {
        const std::string name;
        // const std::string st_type;
        // OpcUa_BuiltInType type;
        const VmType* vm_type;
        OpcUa_BuiltInType OpcType() const {
            return vm_type->OpcType();
        }
        lib::string_view getStType() const {
            return vm_type->name.utf8();
        }
        OpcUa_VariantHlp default_value;
        int64_t m_ref;
        const DirTable* table;
        DirField(const json::Value& json, const DirTable* tbl, const VMInfo& vminfo);
        MPLCSHARE_API const DirTable* ref() const;
        MPLCSHARE_API std::string fullName() const;
    };
    struct DirTable {
        const std::string name;
        const DirField* displayField;
        const int64_t id;
        const DirField* pk;
        int archive_id;
        lib::unordered_map<lib::string_view, const DirField*> fields;
        DirTable(const json::Value& json, const VMInfo& vminfo);
        MPLCSHARE_API const DirField* field(lib::string_view name) const;
        ~DirTable();
        // void NewChild(const Node* field);
    };

}}  // namespace mplc::vm
