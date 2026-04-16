#pragma once
#include <share/mplcshare.h>
#include <string>
#include <vector>

#ifndef MPLC_LDAP_API
#    ifdef _WIN32
#        ifdef MPLC_LDAP_EXPORTS
#            define MPLC_LDAP_API __declspec(dllexport)
#        else
#            define MPLC_LDAP_API __declspec(dllimport)
#        endif
#    else
#        define MPLC_LDAP_API
#    endif
#endif

namespace mplc { namespace ldap {

#if defined(USE_LDAP) || defined(_WIN32)
    MPLC_LDAP_API OpcUa_StatusCode GetADGroups(const std::string& login,
                                                const std::string& password,
                                                const std::string& server,
                                                std::string& full_name,
                                                std::vector<std::string>& groups,
                                                std::string& error);
#else
    inline OpcUa_StatusCode GetADGroups(const std::string& login,
                                        const std::string& password,
                                        const std::string& server,
                                        std::string& full_name,
                                        std::vector<std::string>& groups,
                                        std::string& error) {
        return OpcUa_Good;
    }

#endif

}}  // namespace mplc::ldap
