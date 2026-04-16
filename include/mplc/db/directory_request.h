#pragma once
#include <share/mplcshare.h>
#include <mplc/database.h>

namespace mplc {

    struct DirectoryRequest {
        ADD_PTR_TYPEDEF(DirectoryRequest)
        enum Type { tNull, tSelect, tInsert, tUpdate, tDelete };
        struct Field {
            Field(const vm::DirField* field = nullptr): ref(nullptr), field(field) {}
            std::string path;
            const vm::DirField* ref;
            const vm::DirField* field;
            const std::string& getName() const {
                static const std::string none = "_";
                return field ? field->name : none;
            }
            const std::string& getPath() const { return path.empty() ? field->name : path; }
            lib::string_view getStType() const { return field->vm_type->name.utf8(); }
        };

        struct SQL {
            enum Type { tSelect, tInsert, tUpdate, tDelete } type;
            const vm::DirTable* table;
            std::vector<const vm::DirField*> fields;
            std::map<const vm::DirTable*, const vm::DirField*> join;
            std::vector<std::pair<const vm::DirField*, std::string> > where_;
            MPLC_DATABASE_API std::string create() const;
            MPLC_DATABASE_API std::string read() const;
            MPLC_DATABASE_API std::string update() const;
            MPLC_DATABASE_API std::string delete_() const;
        };

        struct Table {
            typedef std::pair<const vm::DirTable*, const vm::DirField*> Join;

            std::vector<const Field*> fields;
            std::vector<Join> join;
            std::string where_;
            const vm::DirTable* tbl;
            int64_t id;
            Table(const vm::DirTable* table);
            MPLC_DATABASE_API virtual ~Table();
            MPLC_DATABASE_API std::string select_sql() const;
            MPLC_DATABASE_API std::string insert_sql() const;
            void addField(const Field* field);
            std::vector<OpcUa_VariantHlp> data;
            template<class T>
            void insert(const T& from);
            OpcUa_BuiltInType getType(const std::string& field) const;
            bool getFieldName(const std::string& name, std::ostream& out);
        };
        struct Select {
            const vm::DirTable* main;
            std::map<const vm::DirTable*, Table*> tables;
            std::vector<const Field*> refs;
            Select(): main(nullptr) {}
            MPLC_DATABASE_API int getAarchiveId() const;
            MPLC_DATABASE_API std::string mainName() const;
            MPLC_DATABASE_API OpcUa_StatusCode load(int64_t from, const std::vector<std::string>& fields, int64_t id);
            MPLC_DATABASE_API OpcUa_StatusCode load(const Value& request);
            void addField(const vm::DirField* field);
            std::vector<OpcUa_VariantHlp> data;
            MPLC_DATABASE_API virtual ~Select();
            template<class T>
            OpcUa_StatusCode Exec(T& sql);
        };
        struct Insert {
            const vm::DirTable* table;
            std::vector<const vm::DirField*> fields;
            std::vector<OpcUa_VariantHlp> data;
            // Table table;
            int64_t inserted_id;
            template<class T>
            OpcUa_StatusCode Exec(T& sql);
            MPLC_DATABASE_API std::string sql() const;
            Insert(): table(nullptr), inserted_id(0) {}
            MPLC_DATABASE_API OpcUa_StatusCode load(const Value& request);
            int getAarchiveId() const;
        };
        struct Update {
            const vm::DirTable* table;
            std::vector<const vm::DirField*> fields;
            std::map<int32_t, std::vector<OpcUa_VariantHlp> > data;
            // Table table;
            int32_t pk;
            template<class T>
            OpcUa_StatusCode Exec(T& sql);
            MPLC_DATABASE_API std::string sql() const;
            Update(): table(nullptr), pk(0) {}
            MPLC_DATABASE_API OpcUa_StatusCode load(const Value& request);
            int getAarchiveId() const;
        };
        struct Delete {
            const vm::DirTable* table;
            std::vector<int32_t> rows;
            template<class T>
            OpcUa_StatusCode Exec(T& sql);
            MPLC_DATABASE_API std::string sql() const;
            Delete(): table(nullptr) {}
            MPLC_DATABASE_API OpcUa_StatusCode load(const Value& request);
            int getAarchiveId() const;
        };
        template<class T>
        OpcUa_StatusCode newQuery(const Value& request);
        MPLC_DATABASE_API OpcUa_StatusCode newSelect(int64_t from, const std::vector<std::string>& fields, int64_t id);
        template<class T>
        operator const T*();
        MPLC_DATABASE_API int archive();

        template<class T>
        OpcUa_StatusCode Exec(T& sql);
        std::string error;
        MPLC_DATABASE_API void clear();
        DirectoryRequest(): type(tNull), req(nullptr) {}
        MPLC_DATABASE_API ~DirectoryRequest();
        static ptr make() { return boost::make_shared<DirectoryRequest>(); }

    private:
        Type type;
        void* req;
        template<class T>
        T* get_req();
        template<class T>
        static Type get_type();
    };

    template<>
    inline DirectoryRequest::Type DirectoryRequest::get_type<DirectoryRequest::Select>() {
        return tSelect;
    }
    template<>
    inline DirectoryRequest::Type DirectoryRequest::get_type<DirectoryRequest::Update>() {
        return tUpdate;
    }
    template<>
    inline DirectoryRequest::Type DirectoryRequest::get_type<DirectoryRequest::Delete>() {
        return tDelete;
    }
    template<>
    inline DirectoryRequest::Type DirectoryRequest::get_type<DirectoryRequest::Insert>() {
        return tInsert;
    }
    template<class T>
    T* DirectoryRequest::get_req() {
        return type == get_type<T>() ? static_cast<T*>(req) : nullptr;
    }
    template<class T>
    DirectoryRequest::operator const T*() {
        return get_req<T>();
    }

    template<class T>
    OpcUa_StatusCode DirectoryRequest::Exec(T& sql) {
        switch (type) {
        case tSelect:
            return get_req<Select>()->Exec(sql);
        case tInsert:
            return get_req<Insert>()->Exec(sql);
        case tUpdate:
            return get_req<Update>()->Exec(sql);
        case tDelete:
            return get_req<Delete>()->Exec(sql);
        default:
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Unexpected case: %d", type);
            return OpcUa_Bad;
        }
    }

    template<class T>
    OpcUa_StatusCode DirectoryRequest::newQuery(const Value& request) {
        clear();
        type = get_type<T>();
        req = new T();
        return static_cast<T*>(req)->load(request);
    }
}  // namespace mplc
