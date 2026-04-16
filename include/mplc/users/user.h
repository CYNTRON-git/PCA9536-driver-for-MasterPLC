#pragma once
#include <share/mplcshare.h>
#include <rapidjson/filewritestream.h>
#include <mplc/users/right.h>
#include <mplc/users/role.h>

#include "share/scada_types.h"

#ifdef WIN32
#    ifdef MPLC_USERS_EXPORTS
#        define MPLC_USERS_API __declspec(dllexport)
#    else
#        define MPLC_USERS_API __declspec(dllimport)
#    endif
#else
#    define MPLC_USERS_API
#endif

typedef JsonTextStream<rapidjson::FileWriteStream>::type JsonFileWriter;

namespace mplc {
    struct TODInterval : ScadaStruct {
        SCADA_API::TOD Min;
        SCADA_API::TOD Max;
        SCADA_API::BYTE WeekDays;

    public:
        template<class This>
        static void BindFields() {
            SCADA_API::bind_field<This, SCADA_API::TOD, SCADA_API::__TOD>("Min", &This::Min, false);
            SCADA_API::bind_field<This, SCADA_API::TOD, SCADA_API::__TOD>("Max", &This::Max, false);
            SCADA_API::bind_field<This, SCADA_API::BYTE, SCADA_API::__BYTE>("WeekDays", &This::WeekDays, false);
        }
        static void RegAsLuaType();
        static const char* _FullName();
        static const char* _ShortName();
        TODInterval(int64_t min, int64_t max, uint8_t week_days = 0xff): Min(min), Max(max), WeekDays(week_days) {}
        TODInterval(): WeekDays(0) {}
    };
};  // namespace mplc

class User {
public:
    ADD_PTR_TYPEDEF(User)
    std::string login;
    std::string password;
    bool blocked;
    int64_t unblock_time;
    bool need_reset_password;
    int64_t login_count;
    mplc::IpWhiteList allowed_address;
    std::vector<std::pair<int64_t, int64_t> > allowed_dates;
    std::vector<mplc::TODInterval> allowed_times;
    std::vector<bool> disable_days;
    OpcUa_StringToVariantMap settings;

    int64_t password_time;

    std::map<Right::Type, Right> rights;
    std::vector<Role::ptr> roles;

    int64_t session_id;
    std::string start_mnemoscheme;
    std::string full_name;
    std::string salt;
    std::string hash_name;

    enum mask_t {
        changed_password = 0x1,
        changed_blocked = 0x2,
        changed_need_reset_password = 0x4,
        changed_allowed_address = 0x8,
        changed_allowed_date_time = 0x10,
        changed_settings = 0x20,
        changed_roles = 0x40,
        changed_all = 0x7F
    };

    int changed_mask = 0;

    User()
        : blocked(false), unblock_time(0), need_reset_password(false), login_count(0), password_time(0), session_id(0) {
    }

    User(const std::string& _name, const std::string& _password, OpcUa_StringToVariantMap _settings);

    User(const Value& node);
    static ptr make() {
        return boost::make_shared<User>();
    }
    static ptr make(const User& user) {
        return boost::make_shared<User>(user);
    }
    static ptr make(const std::string& name, const std::string& password, OpcUa_StringToVariantMap settings) {
        return boost::make_shared<User>(name, password, settings);
    }
    static ptr make(const Value& node) {
        return boost::make_shared<User>(node);
    }

    int GetAccess(Right::Type type) {
        auto it = rights.find(type);
        int access = it != rights.end() ? it->second.access : 0;
        for (auto& role: roles) {
            auto role_right_it = role->rights.find(type);
            if (role_right_it != role->rights.end()) {
                access |= role_right_it->second.access;
            }
        }
        return access;
    }

    template<class T>
    void to_json(T& writer, bool is_dt, bool full, bool for_http_api = false) const {
        writer.StartObject();
        writer.Key("login");
        writer.String(login);

        if (full) {
            writer.Key("password");
            writer.String(password);
        }
        writer.Key("blocked");
        writer.Bool(blocked);
        writer.Key("need_reset_password");
        writer.Bool(need_reset_password);
        if (!for_http_api) {
            writer.Key("login_count");
            writer.Int64(login_count);
        }
        writer.Key("unblock_time");
        writer.Int64(for_http_api ? filetime_to_timetms(unblock_time) : unblock_time);
        writer.Key("allowed_address");
        writer.String(allowed_address.to_string());
        if (!for_http_api) {
            writer.Key("is_dt");
            writer.Bool(is_dt);
        }
        writer.Key("allowed_times");
        writer.StartArray();
        for (const auto& allowed_time: allowed_times) {
            writer.StartObject();
            writer.Key("Min");
            writer.Int64(for_http_api ? filetime_to_timetms(allowed_time.Min.dt()) : allowed_time.Min.dt());
            writer.Key("Max");
            writer.Int64(for_http_api ? filetime_to_timetms(allowed_time.Max.dt()) : allowed_time.Max.dt());
            writer.Key("WeekDays");
            writer.Int(allowed_time.WeekDays);
            writer.EndObject();
        }
        writer.EndArray();

        writer.Key("allowed_dates");
        writer.StartArray();
        for (const auto& [from, to]: allowed_dates) {
            writer.StartArray();
            writer.Int64(for_http_api ? filetime_to_timetms(from) : from);
            writer.Int64(for_http_api ? filetime_to_timetms(to) : to);
            writer.EndArray();
        }
        writer.EndArray();
        writer.Key("disable_days_of_week");
        writer.StartArray();
        for (bool disable_day: disable_days) {
            writer.Bool(disable_day);
        }
        writer.EndArray();
        if (!for_http_api) {
            writer.Key("password_time");
            writer.Int64(for_http_api ? filetime_to_timetms(password_time) : password_time);
        }
        WriteRolesToJSON(writer);
        writer.Key("settings");
        writer.StartObject();
        auto s_it = settings.begin();
        std::string tmp;
        while (s_it != settings.end()) {
            s_it->second.GetString(tmp);
            writer.Key(s_it->first);
            writer.String(tmp);
            ++s_it;
        }
        writer.EndObject();
        if (!for_http_api) {
            writer.Key("changed_mask");
            writer.Int(changed_mask);
            writer.Key("salt");
            writer.String(salt);
            writer.Key("hash_name");
            writer.String(hash_name);
        }
        writer.EndObject();
    }

    void LoadUser(const Value& node);
    template<class T>
    void WriteRolesToJSON(T& writer) const {
        writer.Key("groups");
        writer.StartArray();
        for (const auto& role: roles) {
            const std::string& name = role->name;
            if (name == "_All")
                continue;
            writer.String(name);
        }
        writer.EndArray();
    }
    OpcUa_StatusCode CheckLoginError(const std::string& login_password,
                                     const std::string& source,
                                     std::string& out_msg);
    OpcUa_StatusCode CheckOperatorAllowedTime(bool already_loggined);
    OpcUa_StatusCode CheckOperatorAllowedAddress(const std::string& source, std::string& out_msg) const;
    int GetPasswordNonRepeatingCount() const;
    bool CheckPasswordComplexity(const std::string& login_password,
                                 int& non_repeating_passwords,
                                 std::string& error_msg) const;
    bool CheckIfNeedToUnblock();
    bool IsDisableMultiplyLogin();
    bool CheckPassword(const std::string& password_to_check, std::string& error_msg) const;
    bool SetRTUserLoginCount(OpcUa_StatusCode code);
    int64_t GetPasswordMinTime();
    int64_t GetSessionIdleTime();
    void WriteUserInfo(ResponseWriter& writer) const;
    OpcUa_StatusCode SetSessionSettings();
    std::string GetRolesList() const;
    bool IsRolesListEmpty() const;
    std::string GetSettingsList() const;
    int64_t GetSessionDuration() const;
};

typedef std::map<std::string, User::ptr> UsersMap;

class Users {
public:
    UsersMap users_map;
    void to_json(ResponseWriter& writer, bool without_dt, bool full) const;
    void from_json(const Value& request, Roles& roles);
    void delete_role(const std::string& name, std::string& error_msg);
    void clear(bool without_dt = false);
    bool empty() const {
        return users_map.empty();
    }
};

struct PreviousPassword {
    std::string password;
    std::string salt;
    std::string hash_name;

    PreviousPassword() = default;
    PreviousPassword(std::string password_, std::string salt_, std::string hash_name_)
        : password(std::move(password_)), salt(std::move(salt_)), hash_name(std::move(hash_name_)) {}
    PreviousPassword(const Value& json) {
        if (json.IsObject()) {
            password = GetSafeStringValue(json, "password");
            salt = GetSafeStringValue(json, "salt");
            hash_name = GetSafeStringValue(json, "hash_name");
        } else if (json.IsString()) {  // Загрузка старой версии
            password = json.GetString();
        }
    }

    template<class T>
    void to_json(T& writer) const {
        writer.StartObject();
        writer.Key("password");
        writer.String(password);
        writer.Key("salt");
        writer.String(salt);
        writer.Key("hash_name");
        writer.String(hash_name);
        writer.EndObject();
    }
};
