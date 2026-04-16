#pragma once
#include "user.h"
#include "users_configuration.h"
#include <mplc/database.h>
#include "session_manager.h"

namespace mplc {
    typedef std::map<std::string, Role::ptr> RolesMap;
    typedef std::map<std::string, User::ptr> UsersMap;

    struct UserPermissionSettings {
        std::string name;
        bool enable{};
        bool disable{};
        bool confirmation{};
        bool write_log{};
    };

    class AuthorizationModel {
        friend class AccountService;

        AuthorizationModel();
        ~AuthorizationModel();

    public:
        void LoadUsers();
        void LoadChangedUser(User::ptr changed_user);
        void Update(const User& item, mplc::archive::UserRequestType type, const std::string& update_name = "");
        void Update(Role& item, archive::UserRequestType type, const std::string& update_name = "");
        void SaveDTRole(const Role& item, archive::UserRequestType type);
        void Update(const ControlRight& item, mplc::archive::UserRequestType type, const std::string& update_name = "");

        UsersMap& GetUsers() {
            TryLoadUsers();
            return users.users_map;
        }
        RolesMap& GetRoles() {
            TryLoadUsers();
            return roles.roles_map;
        }
        const UsersMap& GetConstUsers() const {
            return users.users_map;
        }
        const RolesMap& GetConstRoles() const {
            return roles.roles_map;
        }
        UsersMap& GetADUsers() {
            return active_directory_users;
        }
        const UsersMap& GetConstADUsers() const {
            return active_directory_users;
        }

        bool DeleteRole(const std::string& name, std::string& error_msg);
        bool CheckPassword(const User::ptr& user, const std::string& password, std::string& error_msg) const;
        void UpdateUsersJSONFile();

        void TryLoadUsers();
        std::string GetUsersFile();
        OpcUa_StatusCode LoadDocument(const std::string& file_name, Document& document);
        OpcUa_StatusCode ParseDocument(const Document& document);
        void Clear();

        void SetRoles(User::ptr& user, std::set<std::string>& roles_list);

        bool RestoreUser(const std::string& name, std::string& err);
        bool RestoreRoleSettings(const std::string& name, std::string& err);
        void ToJson(const std::string& source, ResponseWriter& writer);

        int Load();

        int64_t GetUpdateTime() const noexcept {
            return update_time;
        }
        OpcUa_StatusCode Import(const Value& request);
        OpcUa_StatusCode Import(const std::string& file_name);
        OpcUa_StatusCode Export(ResponseWriter& writer) const;

        void ImportADUsers(const Value& request);
        void ExportADUsers(ResponseWriter& writer) const;

    private:
        bool need_update_file, loaded, ok_loaded;
        RDateTime try_load_time;
        Users users;
        Roles roles;
        std::map<std::string, User::ptr> active_directory_users;
        users::UsersArchiveManager* archive_mngr;
        std::map<std::string, std::vector<PreviousPassword>> user_passwords;
        UsersDTConfiguration& dt_configuration;
        int64_t update_time;

        std::map<std::string, std::vector<PreviousPassword>>& GetPasswordHistory() {
            return user_passwords;
        }

        void UpdateUsersWithRole(const std::string& new_name);
    };
}  // namespace mplc
