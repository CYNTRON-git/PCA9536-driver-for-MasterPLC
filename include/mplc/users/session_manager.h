#pragma once
#include "user.h"
#include <events/audit_event_type.h>

#include "mplc/ip_filter.h"
#include "mplc/async/paralel_task_pool.h"

namespace mplc {
    struct OperatorSession {
        ADD_PTR_TYPEDEF(OperatorSession);
        std::string login;
        std::string full_name;
        int session_id;
        int64_t expire;
        int64_t last_active;
        int64_t login_time;
        int64_t session_idle_time;
        std::string client_address;
        std::string host_name;
        std::string address_and_host;
        bool is_anonymous_operator;
        bool need_log;
        int time_inactive_count;
        bool db_loaded = false;

        OperatorSession()
            : session_id(0), expire(0), last_active(0), login_time(0), session_idle_time(0),
              is_anonymous_operator(false), need_log(true), time_inactive_count(0) {}

        template<typename... Args>
        static ptr make(Args&&... args) {
            return boost::make_shared<OperatorSession>(std::forward<Args>(args)...);
        }
        MPLC_USERS_API int64_t GetExpireAsUnixTime() const;
        MPLC_USERS_API int64_t GetSessionIdleTimeInMs() const {
            return session_idle_time / FT_MILLISECOND;
        }
        // Передаем копию для инициализации
        MPLC_USERS_API int64_t InitSession(User::ptr user,
                                           const std::string& remote_address,
                                           const std::string& host,
                                           bool is_anonymous);
    };

    class OperatorSessionManager {
    private:
        std::map<int, OperatorSession::ptr> operator_sessions;
        std::vector<int> deleted_sessions;
        mutable lib::mutex sessions_mutex;
        int total_count, max_try_count;
        int sessions_limit;
        int max_sessions_per_ip{};
        bool config_started;
        events::AuditEventType* user_events;
        async::AsyncTask sync_with_db;
        async::AsyncTask close_old_sessions;
        std::vector<IpFilter> ip_priorities;

        OperatorSessionManager();
        ~OperatorSessionManager();
        BOOST_DELETED_FUNCTION(OperatorSessionManager(const OperatorSessionManager&))
        BOOST_DELETED_FUNCTION(OperatorSessionManager& operator=(const OperatorSessionManager&))
        void SyncWithDb();
        void CloseOldSessions();
        void MarkSessionAsDeleted(const OperatorSession::ptr& session);

        OpcUa_StatusCode CheckSessionLimit(const std::string& source);
        OpcUa_StatusCode CheckPerIpLimit(const std::string& source);

    public:
        void Stop();
        MPLC_USERS_API void Init(int64_t securityItemId);
        MPLC_USERS_API bool IsConfigStarted() const;
        MPLC_USERS_API void SetConfigStarted(bool _config_started);
        int GetPriority(const std::string& source);

        MPLC_USERS_API OpcUa_StatusCode CreateSession(const std::string& source, OperatorSession::ptr& session);
        MPLC_USERS_API OperatorSession::ptr CheckSessionId(int sessionId);
        MPLC_USERS_API std::vector<OperatorSession::ptr> GetActiveSessions() const;
        MPLC_USERS_API bool CheckActiveSession(const std::string& login) const;
        MPLC_USERS_API OpcUa_StatusCode DisableMultiplyLogin(const std::string& login, const std::string& address);

        MPLC_USERS_API void ClearSessionsMap();
        MPLC_USERS_API bool DeleteSession(int64_t session_id, std::string& error, const std::string& message);
        MPLC_USERS_API bool DeleteSessionByUserName(const std::string& login);

        MPLC_USERS_API OperatorSession::ptr LoadSessionFromCookie(const std::string& query);
        MPLC_USERS_API static OperatorSessionManager& Instance();
        MPLC_USERS_API void ImportSessions(std::map<int, OperatorSession::ptr>& sessions);
        MPLC_USERS_API void UserAction(const std::string& source,
                                       const std::string& user,
                                       const std::string& error_msg) const;

        friend class AccountService;
    };

    uint32_t GetCrc32(int original);
    uint32_t GetCrc32(lib::string_view original);
}  // namespace mplc
