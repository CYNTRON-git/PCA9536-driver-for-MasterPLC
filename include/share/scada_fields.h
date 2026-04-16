#pragma once
#include <share/mplcshare.h>
#include "lua/lua_addins.h"

// template<class ScadaType>
struct ScadaFields {
    struct IField {
        std::string name;
        const bool retain;
        IField(const char* name, bool retain): name(name), retain(retain) {}
        virtual void set(const void* ptr, lua_State* L) const = 0;
        virtual void get(void* ptr, /* const */ lua_State* L) const = 0;
        virtual void to_json(mplc::JsonWrapper& json, const void* ptr) const {}
        virtual const char* lua_type() const = 0;
        virtual ~IField() {}
    };
    // typedef boost::unordered_map<std::string, IField*> LuaMeta;
    typedef boost::unordered_map<boost::string_view, const IField*> LuaMeta;
    struct MPLCSHARE_API IDynamicFields {
        virtual ~IDynamicFields() = default;
        virtual int set(const void* ptr, const boost::string_view& key, lua_State* L) = 0;
        virtual int get(void* ptr, const boost::string_view& key, lua_State* L) = 0;
        virtual size_t size() const { return 0; }
    };

    IDynamicFields* dynamic_fields;
    LuaMeta meta;
    MPLCSHARE_API size_t size() { return meta.size() + (dynamic_fields ? dynamic_fields->size() : 0); }
    MPLCSHARE_API ScadaFields();
    MPLCSHARE_API ~ScadaFields();
    MPLCSHARE_API void add(const IField* field);
    MPLCSHARE_API int ReadFrom(void* obj, /*const*/ lua_State* L) const;
    MPLCSHARE_API int WriteTo(const void* obj, lua_State* L) const;

    MPLCSHARE_API int ReadAllFrom(void* obj, lua_State* L) const;
    MPLCSHARE_API int WriteAllTo(const void* obj, lua_State* L) const;
};
