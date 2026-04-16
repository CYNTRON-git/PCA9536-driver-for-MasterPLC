#pragma once
#include <share/config.h>
#include <mplc/libs/string_view.hpp>
#include <mplc/libs/json.hpp>
#include <mplc/libs/threads.hpp>
#include <share/opcua_variant_hlp.h>
#include "vm_path.h"
#include "pt_node.h"

class OpcUa_BuiltInTypeHlp;

namespace mplc { namespace vm {
    // namespace test {

    struct MPLCSHARE_API VmType {
        class MPLCSHARE_API Name {
            std::string* m_utf_name{};
            std::string* m_translited_name{};
            std::string* m_display_name{};
            std::string* m_full_name{};
            /*
             *  "Name": "REAL",
                "TranslitedName": "REAL",
                "DisplayShort": "REAL",
                "FullTypeName": "REAL",
             */
        public:
            enum Type { tUtf8, tTranslited, tDisplay, tFullName };
            Name(const json::Value& json);

            Name(lib::string_view utf,
                 lib::string_view translited = {},
                 lib::string_view full = {},
                 lib::string_view dispaly = {});

            ~Name();

            lib::string_view dispaly() const;

            lib::string_view full() const;

            lib::string_view translited() const;

            lib::string_view utf8() const;

            lib::string_view get(Type type) const;
        };
        enum Kind : uint8_t {
            None = 0,
            kElementary,
            kString,
            kRange,
            kEnumerated,
            kStruct,
            kArray,
            kProgram,
            kClassBlock,
            kAlarmBlock,
            kSystemParam,
            kDirectory,
        };
        template<int K>
        struct GetVmType {
            using type = VmType;
        };
        template<class T>
        struct GetVmTypeKind {
            enum { type = VmType::None };
        };
        Name name;
        uint8_t typeHash{};
        Kind typeKind{None};
        virtual ~VmType() {}
        VmType(lib::string_view name): name(name) {}
        VmType(const json::Value& json, Kind type);

        virtual OpcUa_BuiltInType OpcType() const {
            return OpcUaType_Null;
        }
        virtual OpcUa_BuiltInTypeHlp OpcTypeHlp() const;
        virtual const VmType* find(lib::string_view path) const;
        virtual const VmType* get_ref(lib::string_view& path, VmPath& ref) const;
        virtual void init(const json::Value& json, const VMInfo& ctx) {}
        virtual size_t size() const noexcept {
            return 0;
        }
        template<class T>
        const T* as() const noexcept;
        static Kind getTypeKind(lib::string_view name);
        static VmType* create(const json::Value& json);
    };

    struct MPLCSHARE_API VmTypeElementary : VmType {
        enum STType : uint8_t {
            NONE,
            BOOL,

            SINT,
            INT,
            DINT,
            LINT,

            REAL,
            LREAL,

            BYTE,
            USINT,

            WORD,
            UINT,

            DWORD,
            UDINT,

            LWORD,
            ULINT,

            DATE,
            DATE_AND_TIME,
            DT = DATE_AND_TIME,

            TIME,
            TIME_OF_DAY,
            TOD = TIME_OF_DAY,

            ANY,
            REF_TO,
            // ANY_INT,
            // ANY_REAL,
            //  ANY_DATE,
            // STRING,
            // ANY_STRING,
        };
        int16_t size{};
        STType st_type{};
        VmTypeElementary();
        VmTypeElementary(const json::Value& json);
        OpcUa_BuiltInType OpcType() const override;
        static STType getType(lib::string_view name);
    };
    struct MPLCSHARE_API VmTypeString : VmType {
        int32_t lenght{};
        VmTypeString(const json::Value& json);

        bool isDynamic() const {
            return lenght < 0;
        }
        OpcUa_BuiltInType OpcType() const override {
            return OpcUaType_String;
        }
    };
    struct MPLCSHARE_API VmTypeRange : VmType {
        int32_t from{};
        int32_t to{};
        int32_t step{1};
        VmTypeRange(const json::Value& json);
    };
    struct VmTypeEnum : VmType {
        lib::unordered_map<lib::string_view, int32_t> values;
        lib::unordered_map<int32_t, std::string> names;
        const VmType* parent_type;
        MPLCSHARE_API VmTypeEnum(const json::Value& json);
        MPLCSHARE_API void init(const json::Value& json, const VMInfo& ctx) override;

        OpcUa_BuiltInType OpcType() const override {
            if (parent_type != nullptr)
                return parent_type->OpcType();
            return OpcUaType_Int32;
        }
    };
    struct VmTypeStruct : VmType { /*
         std::vector<char*> lua_names;
         std::vector<char*> utf_names;
         std::vector<const VmType*> types;*/
        struct Field {
            std::string lua;
            std::string utf8;
            const VmType* type;
        };
        std::vector<Field*> fields;
        lib::unordered_map<lib::string_view, int> fields_map;

    private: /*
         lib::unordered_map<lib::string_view, int> lua_fields;
         lib::unordered_map<lib::string_view, int> utf_fields;*/
    public:
        MPLCSHARE_API VmTypeStruct(lib::string_view name): VmType(name) {}
        MPLCSHARE_API VmTypeStruct(const json::Value& json);
        MPLCSHARE_API ~VmTypeStruct() override;
        MPLCSHARE_API OpcUa_BuiltInType OpcType() const override {
            return OpcUaType_JsonElement;
        }
        MPLCSHARE_API OpcUa_BuiltInTypeHlp OpcTypeHlp() const override;

        const VmType* field_type(lib::string_view name) const {
            int field_idx = field_pos(name);
            if (field_idx != -1) {
                return fields[field_idx]->type;
            }
            return nullptr;
        }
        int field_pos(lib::string_view name) const {
            auto it = fields_map.find(name);
            if (it != fields_map.end()) {
                return it->second;
            }
            return -1;
        }
        size_t count() const {
            return fields.size();
        }
        MPLCSHARE_API const VmType* get_ref(lib::string_view& path, VmPath& ref) const override;
        MPLCSHARE_API const VmType* find(lib::string_view path) const override;
        MPLCSHARE_API bool add_field(lib::string_view name, const VmType* type, lib::string_view utf_name = {});
        MPLCSHARE_API void init(const json::Value& json, const VMInfo& ctx) override;
    };
    struct VmTypeDirectory : VmTypeStruct {
    private:
        mutable std::vector<char*> strings;
        mutable lib::unordered_map<lib::string_view, const VmType*> storage;
        mutable lib::shared_mutex mtx;

    public:
        MPLCSHARE_API VmTypeDirectory(const json::Value& json): VmTypeStruct(json) {
            typeKind = kDirectory;
        }
        MPLCSHARE_API ~VmTypeDirectory() override;
        MPLCSHARE_API const VmType* get_ref(lib::string_view& path, VmPath& ref) const override;
        MPLCSHARE_API const VmType* find(lib::string_view path) const override;
    };
    struct MPLCSHARE_API VmTypeSystemParam final : VmType {
        const VmTypeElementary* time{};
        const VmTypeEnum* quality{};
        const VmType* value{};
        VmTypeSystemParam(lib::string_view name): VmType(name) {}
        VmTypeSystemParam(const json::Value& json);
        ~VmTypeSystemParam() override {}

        OpcUa_BuiltInType OpcType() const override {
            return OpcUaType_DataValue;
        }
        OpcUa_BuiltInTypeHlp OpcTypeHlp() const override;
        const VmType* field_type(lib::string_view name) const;
        const VmType* get_ref(lib::string_view& path, VmPath& ref) const override;
        const VmType* find(lib::string_view path) const override;
        void init(const json::Value& json, const VMInfo& ctx) override;
    };
    struct VmTypeObject : VmType {
        struct FreeObject* object{};
        lib::unordered_map<int64_t, const struct PtNode*> all_nodes;
        MPLCSHARE_API VmTypeObject(const json::Value& json);
        MPLCSHARE_API ~VmTypeObject() override;
        // MPLCSHARE_API OpcUa_BuiltInType OpcType() const override {
        //     return OpcUaType_Variant;
        // }
        // MPLCSHARE_API OpcUa_BuiltInTypeHlp OpcTypeHlp() const override;

        MPLCSHARE_API const std::vector<const PtNode*>& childs() const;
        void init(const json::Value& json, const VMInfo& ctx) override;
    };

    struct MPLCSHARE_API VmTypeArray : VmType {
        // отдельные измерения не могут быть динамическими, иначе не возможно вычислить линейную позицию
        struct Dimention {
            uint32_t begin{};
            int32_t size{};
        };
        Dimention dimentions[3]{};
        const VmType* elementType{};
        uint8_t dimentions_count{1};
        VmTypeArray(const json::Value& json);

        bool is_dynamic(uint8_t dimention) const {
            if (dimention < dimentions_count) {
                return dimentions[dimention].size <= 0;
            }
            return false;
        }
        bool is_dynamic() const {
            for (int i = 0; i < dimentions_count; i++) {
                if (dimentions[i].size <= 0) {
                    return true;
                }
            }
            return false;
        }
        int32_t lenght() const;
        int32_t position(int dimention, int32_t pos) const;
        int32_t index_from_str(lib::string_view path) const;
        OpcUa_BuiltInType OpcType() const override {
            if (elementType == nullptr)
                return OpcUaType_Null;
            return elementType->OpcType();
        }
        OpcUa_BuiltInTypeHlp OpcTypeHlp() const override;
        const VmType* find(lib::string_view path) const override;
        const VmType* get_ref(lib::string_view& path, VmPath& ref) const override;
        void init(const json::Value& json, const VMInfo& ctx) override;
    };
    struct VmTypeProgram : VmType {
        lib::unordered_map<int64_t, const PtNode*> all_nodes;
        VmTypeProgram(const json::Value& json);
        void init(const json::Value& json, const VMInfo& ctx) override;
        MPLCSHARE_API const std::vector<const PtNode*>& childs() const {
            return m_childs;
        }
        const PtNode* find(int64_t id) const;
        virtual const PtNode* find(lib::string_view child, PtNode::Name::Type find_by = PtNode::Name::tUtf8) const;

        template<class T>
        const T* find(int64_t id) const {
            return dynamic_cast<const T*>(find(id));
        }

    private:
        std::vector<const PtNode*> m_childs;
    };
    struct VmTypeAlarm : VmType {
        struct Field {
            Field(lib::string_view lua_name, const VmType* type, lib::string_view utf_name)
                : lua_name(lua_name), utf_name(utf_name), type(type) {}
            Field(const Field&) = delete;
            Field& operator=(const Field&) = delete;
            std::string lua_name;
            std::string utf_name;
            const VmType* type;
        };

        std::vector<const Field*> fields;

        VmTypeAlarm(const json::Value& json);

        MPLCSHARE_API const VmType* field_type(lib::string_view name) const;
        MPLCSHARE_API int field_pos(lib::string_view name) const;

        MPLCSHARE_API size_t count() const;
        MPLCSHARE_API const VmType* get_ref(lib::string_view& path, VmPath& ref) const override;
        MPLCSHARE_API const VmType* find(lib::string_view path) const override;
        void init(const json::Value& json, const VMInfo& ctx) override;

    private:
        lib::unordered_map<lib::string_view, size_t> lua_fields;
        lib::unordered_map<lib::string_view, size_t> utf_fields;
        auto add_field(lib::string_view lua_name, const VmType* type, lib::string_view utf_name = {})
            -> decltype(lua_fields.begin());
    };

    /*struct VmTypeExternalFb : VmType {
        VmTypeExternalFb(const json::Value& json): VmType(json, kExternalFB) {}

        void init(const json::Value& json, const VMInfo& ctx) override;
    };*/
#define MPLC_VM_TYPE_MAP(Kind, Type)                                                                                   \
    template<>                                                                                                         \
    struct VmType::GetVmType<VmType::Kind> {                                                                           \
        using type = Type;                                                                                             \
    };                                                                                                                 \
    template<>                                                                                                         \
    struct VmType::GetVmTypeKind<Type> {                                                                               \
        enum { type = VmType::Kind };                                                                                  \
    };

    MPLC_VM_TYPE_MAP(kElementary, VmTypeElementary);
    MPLC_VM_TYPE_MAP(kString, VmTypeString);
    MPLC_VM_TYPE_MAP(kRange, VmTypeRange);
    MPLC_VM_TYPE_MAP(kEnumerated, VmTypeEnum);
    MPLC_VM_TYPE_MAP(kStruct, VmTypeStruct);
    MPLC_VM_TYPE_MAP(kArray, VmTypeArray);
    MPLC_VM_TYPE_MAP(kProgram, VmTypeProgram);
    MPLC_VM_TYPE_MAP(kClassBlock, VmTypeObject);
    MPLC_VM_TYPE_MAP(kAlarmBlock, VmTypeAlarm);
    MPLC_VM_TYPE_MAP(kSystemParam, VmTypeSystemParam);
    MPLC_VM_TYPE_MAP(kDirectory, VmTypeDirectory);
    // MPLC_VM_TYPE_MAP(kExternalFB, VmTypeExternalFb);
    MPLC_VM_TYPE_MAP(None, VmType);
#undef MPLC_VM_TYPE_MAP

    template<class T>
    const T* VmType::as() const noexcept {
        if (typeKind == GetVmTypeKind<T>::type) {
            return static_cast<const T*>(this);
        }
        return nullptr;
    }

    //}  // namespace test
    struct VMAlarmType {
        std::string name;
        int64_t id;
        std::string idString;
        int64_t event_archive_id;
    };
}}  // namespace mplc::vm
