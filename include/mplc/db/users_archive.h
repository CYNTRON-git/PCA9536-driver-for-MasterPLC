#pragma once
#include <share/mplcshare.h>
#include <mplc/users/user.h>
#include <mplc/users/session_manager.h>

namespace mplc::archive {
    enum UserRequestType {
        InitUser,
        AddUser,
        UpdateUser,
        DeleteUser,
        AddUserSettings,
        UpdateUserSettings,
        DeleteUserSettings,
        GetUser,
        GetUserSettings,
        GetUserPasswords,
        GetAllUsers,
        AddPassword,
        UpdatePassword,
        DeletePassword,
        InitRole,
        AddRole,
        UpdateRole,
        DeleteRole,
        AddRoleRight,
        UpdateRoleRight,
        DeleteRoleRight,
        AddRoleControlRight,
        UpdateRoleControlRight,
        DeleteRoleControlRight,
        GetRoleControlRights,
        AddSession,
        UpdateSession,
        UpdateSessionLogout,
        DeleteSession
    };

    struct UserRequest {
        User user;
        Role role;
        ControlRight control_right;
        OperatorSession operator_session;
        std::string update_name;
        UserRequestType request_type;

        UserRequest(const User& _user, UserRequestType type, const std::string& _update_name = "") {
            user = _user;
            request_type = type;
            update_name = _update_name;
        }

        UserRequest(const Role& _role, UserRequestType type, const std::string& _update_name = "") {
            role = _role;
            // role.is_dt = _role.is_dt;
            request_type = type;
            update_name = _update_name;
        }

        UserRequest(const ControlRight& _right, UserRequestType type, const std::string& _update_name = "") {
            control_right = _right;
            update_name = _update_name;
            request_type = type;
        }

        UserRequest(const OperatorSession& operator_session, UserRequestType type)
            : operator_session(operator_session), request_type(type) {}
    };

    struct IUsersArchive {
        typedef lib::shared_ptr<IUsersArchive> ptr;
        virtual void addRequest(const User& item, UserRequestType type, const std::string& update_name = "") = 0;
        virtual void addRequest(const Role& item, UserRequestType type, const std::string& update_name = "") = 0;
        virtual void addRequest(const ControlRight& right,
                                UserRequestType type,
                                const std::string& update_name = "") = 0;
        virtual void addRequest(const OperatorSession& session, archive::UserRequestType type) = 0;

        virtual bool getCurrentDB(std::vector<User>& users_list,
                                  std::map<std::string, std::vector<PreviousPassword>>& pass_list,
                                  std::vector<boost::shared_ptr<Role>>& roles_list) = 0;

        virtual void updateCurrentSessionDB(std::map<int, OperatorSession::ptr>& operator_sessions,
                                            std::vector<int>& deleted_sessions) = 0;

        virtual bool IsNeedUpdate() = 0;

    protected:
        friend class UsersArchiveManager;
    };

    template<class _ConnectionPool>
    class UsersArchive : public IArchive<_ConnectionPool, ArchiveCfg>, public IUsersArchive {
    protected:
        mutable lib::mutex req_mtx;
        mutable lib::mutex session_mtx;

    public:
        std::queue<UserRequest> requests;

        void addRequest(const User& item, UserRequestType type, const std::string& update_name) override {
            lib::lock_guard<lib::mutex> lock(req_mtx);
            requests.push(UserRequest(item, type, update_name));
        }

        void addRequest(const Role& role, UserRequestType type, const std::string& update_name) override {
            lib::lock_guard<lib::mutex> lock(req_mtx);
            requests.push(UserRequest(role, type, update_name));
        }

        void addRequest(const ControlRight& right, UserRequestType type, const std::string& update_name) override {
            lib::lock_guard<lib::mutex> lock(req_mtx);
            requests.push(UserRequest(right, type, update_name));
        }

        void addRequest(const OperatorSession& session, archive::UserRequestType type) override {
            lib::lock_guard<lib::mutex> lock(req_mtx);
            requests.push(UserRequest(session, type));
        }
    };

    struct IUsersArchiveProc {
        virtual IUsersArchive& getArchive() = 0;
        virtual void run() = 0;
        virtual void stop() = 0;
        virtual ~IUsersArchiveProc(){};
        typedef boost::shared_ptr<IUsersArchiveProc> ptr;
    };

    struct IUsersArchiveProcFactory {
        virtual IUsersArchiveProc::ptr make(const ArchiveCfg& arch_cfg, const ConnectionCfg& pool_cfg) = 0;
    };
}  // namespace mplc::archive
