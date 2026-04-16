#pragma once
#include <mplc/libs/unordered_map.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_abstract.hpp>
#include "scada_fb.h"
#include "scada_protocol.h"

namespace mplc { namespace api {
    struct ApiStorage {
        ApiStorage();
        ApiStorage(const ApiStorage&) = delete;
        ApiStorage& operator=(const ApiStorage&) = delete;
        ApiStorage(ApiStorage&&) = delete;
        ApiStorage& operator=(ApiStorage&&) = delete;
        template<class T>
        const meta::Object& CreateApi() {
            meta::Object api = meta::Object::from<T>();
            if (!boost::is_abstract<T>::value) {
                if (boost::is_base_of<DynamicFB, T>::value) {
                    import_dynamic_fb(api.name());
                } else if (boost::is_base_of<ScadaFB, T>::value) {
                    import_fb(api.name());
                } else if (boost::is_base_of<ScadaProtocol, T>::value) {
                    import_protocol(api.name());
                }
            }
            return m_types[api.name()] = std::move(api);
        }

        MPLCSHARE_API void import_fb(lib::string_view name);
        MPLCSHARE_API void import_dynamic_fb(lib::string_view name);
        MPLCSHARE_API void import_protocol(lib::string_view name);
        MPLCSHARE_API const meta::Object* GetApi(lib::string_view name);
        /*template<class T>
        const mplc::meta::Object* GetType<T>() {
            auto type = mplc::meta::Type::from<T>();
            auto it = m_fb.find(type.name());
            if (it != m_fb.end())
                return &it->second;
            return nullptr;
        }*/
        MPLCSHARE_API static ApiStorage& instance();

    private:
        typedef lib::unordered_map<lib::string_view, meta::Object> Storage;
        Storage m_types;
        template<class>
        struct Box;
        // struct BoxProtocolType;
    };
    namespace detail {
        template<class T>
        struct ApiHolder {
            static const meta::Object& type;
        };
    }  // namespace detail
}}     // namespace mplc::api
