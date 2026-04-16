#pragma once
#include "users_archive.h"
#include <mplc/users/session_manager.h>

namespace mplc::users {
    class UsersArchiveManager {
    private:
        typedef std::map<int, archive::IUsersArchiveProc::ptr> ArchiveProcessMap;
        typedef std::map<std::string, archive::IUsersArchiveProcFactory*> UsersArchiveFactories;
        ArchiveProcessMap archives;
        UsersArchiveFactories factories;

    public:
        OpcUa_StatusCode init();
        void addArchive(int db_id, const Value& config);
        MPLC_USERS_API void regFactory(const std::string& name, archive::IUsersArchiveProcFactory* factory);
        MPLC_USERS_API static UsersArchiveManager& instance();
        MPLC_USERS_API void addRequest(const User& item,
                                       archive::UserRequestType type,
                                       const std::string& update_name = "");
        MPLC_USERS_API void addRequest(const Role& item,
                                       archive::UserRequestType type,
                                       const std::string& update_name = "");
        MPLC_USERS_API void addRequest(const ControlRight& item,
                                       archive::UserRequestType type,
                                       const std::string& update_name = "");

        MPLC_USERS_API bool getDBUsers(std::vector<User>& items,
                                       std::map<std::string, std::vector<PreviousPassword>>& pass_items,
                                       std::vector<Role::ptr>& roles_list);

        MPLC_USERS_API void getDBSessions(std::map<int, OperatorSession::ptr>& operator_sessions,
                                          std::vector<int>& deleted_sessions);

        void stop();
    };

}  // namespace mplc::users
