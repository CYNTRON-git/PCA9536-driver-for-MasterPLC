#pragma once
#include <share/mplcshare.h>
#include <rapidjson/filewritestream.h>
#include <mplc/users/right.h>
#include "mplc/ip_filter.h"

typedef JsonTextStream<rapidjson::FileWriteStream>::type JsonFileWriter;

class Role {
public:
    ADD_PTR_TYPEDEF(Role)
    std::string name;
    std::string start_mnemoscheme;
    int64_t session_duration;
    int password_min_size;
    int password_non_repeating_count;
    bool password_is_complex;
    int64_t password_expiration_time;
    bool disable_os_access;
    bool log_action_try;
    int max_login_count;
    int64_t pause_after_invalid_logins;
    int64_t password_min_time;
    bool end_session_by_time;
    int64_t session_end_delay;
    int64_t session_idle_time;
    bool disable_multiply_login;
    std::map<Right::Type, Right> rights;
    std::vector<ControlRight> control_rights;
    bool changed;
    mplc::IpWhiteList allowed_ip_addresses{};
    bool password_requires_spec_chars;

    Role()
        : session_duration(0), password_min_size(0), password_non_repeating_count(0), password_is_complex(false),
          password_expiration_time(0), disable_os_access(false), log_action_try(false), max_login_count(0),
          pause_after_invalid_logins(0), password_min_time(0), end_session_by_time(false), session_end_delay(0),
          session_idle_time(0), disable_multiply_login(false), changed(false), password_requires_spec_chars(false) {}

    Role(const Value& node);

    static ptr make() {
        return boost::make_shared<Role>();
    }
    static ptr make(const Role& role) {
        return boost::make_shared<Role>(role);
    }
    static ptr make(const Value& node) {
        return boost::make_shared<Role>(node);
    }

    void LoadRole(const Value& node);

    template<class T>
    void to_json(T& writer, bool is_dt, bool for_http_api = false) const {
        writer.StartObject();
        writer.String("name");
        writer.String(name);
        writer.String("sessionDuration");
        writer.Int64(for_http_api ? filetime_to_timetms(session_duration) : session_duration);
        writer.String("passwordMinSize");
        writer.Int(password_min_size);
        writer.String("passwordNonRepeatingCount");
        writer.Int(password_non_repeating_count);
        writer.String("passwordExpirationTime");
        writer.Int64(for_http_api ? filetime_to_timetms(password_expiration_time) : password_expiration_time);
        writer.String("passwordIsComplex");
        writer.Bool(password_is_complex);
        if (!for_http_api) {
            writer.String("isDt");
            writer.Bool(is_dt);
        }
        writer.String("logActionTry");
        writer.Bool(log_action_try);
        writer.String("disableOSAccess");
        writer.Bool(disable_os_access);
        writer.String("maxLoginCount");
        writer.Int64(max_login_count);
        writer.String("pauseAfterInvalidLogins");
        writer.Int64(for_http_api ? filetime_to_timetms(pause_after_invalid_logins) : pause_after_invalid_logins);
        writer.String("passwordMinTime");
        writer.Int64(for_http_api ? filetime_to_timetms(password_min_time) : password_min_time);
        writer.String("endSessionByTime");
        writer.Bool(end_session_by_time);
        writer.String("sessionEndDelay");
        writer.Int64(for_http_api ? filetime_to_timetms(session_end_delay) : session_end_delay);
        writer.String("sessionIdleTime");
        writer.Int64(for_http_api ? filetime_to_timetms(session_idle_time) : session_idle_time);
        writer.String("disableMultiplyLogin");
        writer.Bool(disable_multiply_login);
        writer.String("allowedIpAddresses");
        writer.String(allowed_ip_addresses.to_string());
        writer.String("passwordRequiresSpecChars");
        writer.Bool(password_requires_spec_chars);
        if (!for_http_api) {
            writer.String("сontrolRights");
            writer.StartArray();
            for (const auto& control_right: control_rights) {
                control_right.to_json(writer);
            }
            writer.EndArray();
        }
        writer.EndObject();
    }
};

typedef std::map<std::string, Role::ptr> RolesMap;

class Roles {
public:
    RolesMap roles_map;
    void to_json(ResponseWriter& writer, bool without_dt = false) const;
    void from_json(const Value& request);
    void clear(bool without_dt = false);
    bool empty() const {
        return roles_map.empty();
    }
    bool delete_role(const std::string& name, Role& deleted_role, std::string& error_msg);
    void load_from_dt();
};
