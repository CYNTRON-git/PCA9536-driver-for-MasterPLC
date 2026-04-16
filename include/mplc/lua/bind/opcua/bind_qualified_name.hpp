#pragma once
#include "bind_opcua_base.hpp"
#include <mplc/libs/smart_ptr.hpp>

#include "mplc/macros/not_implemented.h"

namespace mplc { namespace lua {
    template<>
    struct Bind<detail::type_wrap<OpcUa_QualifiedName, OpcUaType_QualifiedName>> {
        template<class Tag>
        static int push(lua_State* L, const OpcUa_QualifiedName& v, bool* ok = nullptr) {
            size_t len = OpcUa_String_StrSize(&v.Name);
            const char* str = OpcUa_String_GetRawString(&v.Name);
            bool is_ok = true;
            if (len == 0 || str == nullptr) {
                lua_pushfstring(L, "%d|%s", v.NamespaceIndex, "");
            } else {
                lib::string_view name(OpcUa_String_GetRawString(&v.Name), OpcUa_String_StrSize(&v.Name));
                lib::unique_ptr<char[]> tmp;
                if (lua::detail::need_decode_to_cp1251(name)) {
                    tmp = lib::make_unique<char[]>(name.size());
                    size_t size = ::utf8_to_cp1251(name.data(), tmp.get(), name.size());
                    tmp.get()[size] = 0;
                    name = lib::string_view(tmp.get(), size);
                }
                lua_pushfstring(L, "%d|%s", v.NamespaceIndex, name.data());
            }

            if (ok)
                *ok = is_ok;
            return 1;
        }
        template<class Tag>
        static OpcUa_StatusCode get(lua_State* L, OpcUa_QualifiedName& v, int idx = -1) {
            return OpcUa_BadTypeDefinitionInvalid;
           // MPLC_NOT_IMPLEMENTED(false);
        }
    };
}}  // namespace mplc::lua
