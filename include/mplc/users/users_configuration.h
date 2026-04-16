#pragma once
#include <share/mplcshare.h>
#include <mplc/users/user.h>
#include <boost/scoped_ptr.hpp>
#define SecurityAllGroup "_All"

namespace mplc {
    typedef boost::unordered_map<std::string, Role::ptr> DTRolesMap;
    typedef boost::unordered_map<std::string, boost::scoped_ptr<User> > DTUsersMap;

    class UsersDTConfiguration {
    private:
        DTUsersMap dt_users;
        DTRolesMap dt_roles;
        Role::ptr all;
        std::string start_mnemoscheme;
        std::string default_user;

        bool need_local_network_auth;
        bool disable_delete_logged_users;
        bool log_invalid_login;
        int64_t security_item_id;
        std::string active_directory_host;
        bool request_password_on_logout;
        bool single_user_mode;

    public:
        UsersDTConfiguration()
            : need_local_network_auth(false), disable_delete_logged_users(false), log_invalid_login(false),
              security_item_id(0), request_password_on_logout(false), single_user_mode(false){};

        UsersDTConfiguration(const UsersDTConfiguration&)
            : need_local_network_auth(false), disable_delete_logged_users(false), log_invalid_login(false),
              security_item_id(0), request_password_on_logout(false), single_user_mode(false){};

        UsersDTConfiguration& operator=(const UsersDTConfiguration&) {
            return *this;
        };

        MPLC_USERS_API static UsersDTConfiguration& Instance();
        const DTUsersMap& GetDTUsers() const {
            return dt_users;
        }
        const DTRolesMap& GetDTRoles() const {
            return dt_roles;
        }
        MPLC_USERS_API bool IsDTRole(const std::string& name) const;
        MPLC_USERS_API bool IsDTUser(const std::string& name) const;

        void AddUserRole(User* user, const std::string& role);

        bool IsLogInvalidLogin() const {
            return log_invalid_login;
        }
        bool IsDisabledDeleteLogged() const {
            return disable_delete_logged_users;
        }
        int64_t SecurityItemId() const {
            return security_item_id;
        }
        bool RequestPasswordOnLogout() const {
            return request_password_on_logout;
        }
        bool SingleUserMode() const {
            return single_user_mode;
        }
        const std::string& ActiveDirectoryHost() const {
            return active_directory_host;
        }
        const std::string& DefaultUser() const {
            return default_user;
        }
        const std::string& StartMnemoscheme() const {
            return start_mnemoscheme;
        }
        bool NeedLocalNetworkAuth() const {
            return need_local_network_auth;
        }

        const Role::ptr& All() {
            return all;
        }

        int Load();
        void Clear();
    };
}  // namespace mplc
