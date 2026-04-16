#pragma once
#include <mplc/libs/containers.hpp>
#include <share/config.h>
#include "lua_context.hpp"

namespace mplc { namespace lua {

    /* struct IFieldMeta {
     private:
         struct _Base {
             virtual int set(lua_State* L, const void* ptr, bool* ok) = 0;
             virtual void get(lua_State* L, void* ptr, int i) = 0;
         };
         template<class Tag, class OType, class FieldType, FieldType OType::*field>
         struct _Type : _Base {
             int set(lua_State* L, const void* ptr, bool* ok) override {
                 const OType* obj = static_cast<const OType*>(ptr);
                 return lua::set<Tag>(L, obj->*field, ok);
             }
             void get(lua_State* L, void* ptr, int i) override {
                 OType* obj = static_cast<OType*>(ptr);
                 lua::get<Tag>(L, obj->*field, i);
             }
         };

     public:
         lib::string_view name;
         lib::unique_ptr<_Base> type;
         template<class Tag, class Type, class FieldType, FieldType Type::*param>
         IFieldMeta(const char* name): name(name) {
             type = lib::make_unique<_Type<Tag, Type, FieldType, param>>();
         }
         IFieldMeta(const IFieldMeta& rv) = delete;
         IFieldMeta& operator=(const IFieldMeta& rv) = delete;
         IFieldMeta(IFieldMeta&& rv) noexcept: name(rv.name) {
             type.swap(rv.type);
         }
         IFieldMeta& operator=(IFieldMeta&& rv) noexcept {
             name = rv.name;
             type.swap(rv.type);
             return *this;
         }
         int set(lua_State* L, const void* ptr, bool* ok) {
             return type->set(L, ptr, ok);
         }
         void get(lua_State* L, void* ptr, int i) {
             type->get(L, ptr, i);
         }
     };*/
    struct IFieldMeta {
        IFieldMeta(const IFieldMeta& rv) = delete;
        IFieldMeta& operator=(const IFieldMeta& rv) = delete;
        virtual int set(lua_State* L, const void* ptr, bool* ok) {
            return 0;
        }
        virtual void get(lua_State* L, void* ptr, int i) {}
    };
    template<class Tag, class OType, class FieldType, FieldType OType::*field>
    struct FieldInfo final : IFieldMeta {
        lib::string_view name;
        FieldInfo(lib::string_view f_name): name(f_name) {}

        int set(lua_State* L, const void* ptr, bool* ok) override {
            const OType* obj = static_cast<const OType*>(ptr);
            return lua::set<Tag>(L, obj->*field, ok);
        }
        void get(lua_State* L, void* ptr, int i) override {
            OType* obj = static_cast<OType*>(ptr);
            lua::get<Tag>(L, obj->*field, i);
        }
    };

    struct ScadaFields {
        // typedef boost::unordered_map<std::string, IField*> LuaMeta;
        typedef lib::unordered_map<lib::string_view, IFieldMeta> LuaMeta;
        struct MPLCSHARE_API IDynamicFields {
            virtual ~IDynamicFields() = default;
            virtual int set(const void* ptr, const lib::string_view& key, lua_State* L) = 0;
            virtual int get(void* ptr, const lib::string_view& key, lua_State* L) = 0;
            virtual size_t size() const {
                return 0;
            }
        };

        IDynamicFields* dynamic_fields;
        LuaMeta meta;
        MPLCSHARE_API size_t size() {
            return meta.size() + (dynamic_fields ? dynamic_fields->size() : 0);
        }
        MPLCSHARE_API ScadaFields();
        MPLCSHARE_API ~ScadaFields();
        template<class Tag, class Type, class FieldType, FieldType Type::*param, size_t N>
        void add(const char (&field)[N]) {
            auto f_name = lib::string_view(field, N - 1);
            meta[f_name] = FieldInfo<Tag, Type, FieldType, param>(f_name);
        }

        MPLCSHARE_API int ReadFrom(void* obj, /*const*/ lua_State* L) const;
        MPLCSHARE_API int WriteTo(const void* obj, lua_State* L) const;

        MPLCSHARE_API int ReadAllFrom(void* obj, lua_State* L) const;
        MPLCSHARE_API int WriteAllTo(const void* obj, lua_State* L) const;
    };

}}  // namespace mplc::lua
