#pragma once
#include <share/mplcshare.h>
#include "mplc/time_span.h"
#include "share/filetime.h"

namespace mplc { namespace archive {

    struct Type {
        enum name { Data, Events, Directory, Other };
        std::string defaultDbName() const {
            switch (m_enum) {
            case Data:
                return std::string("data.db");
            case Events:
                return std::string("EventsData.db");  // EventsData.db
            case Other:
            default:
                return std::string("other.db");
            }
        }
        AddBaseEnumFunctions(Type, name, Other)
    };
    // TODO: Убрать конвертацию дней в часы после исправления ошибки на стороне AVADS SA
    struct InsatConfig {
        std::string FsType;
        int Cycle;
        bool AutoSave;
        std::string Interval;
        std::string Duration;
        std::string DbSize;
        std::string Lt;
        bool AutoAddS;
        bool NoAck;
        bool NoLayers;

        std::string convertTime(std::string time) {
            std::vector<std::string> tmp;
            mplc::split(tmp, time, ".:");
            // d.h:m:s.ms
            if (tmp.size() == 5) {
                int d = to_int64(tmp[0].c_str(), tmp[0].size());
                int h = to_int64(tmp[1].c_str(), tmp[1].size());
                h += d * 24;
                return to_string(h) + "h" + tmp[2] + "m" + tmp[3] + "s" + tmp[4].substr(0, 3) + "ms";
            }
            // h:m:s
            if (tmp.size() == 3) {
                return tmp[0] + "h" + tmp[1] + "m" + tmp[2] + "s";
            }
            //  d.h:m:s or h:m:s.ms
            if (tmp.size() == 4) {
                auto pointPos = time.find(".", 0);
                auto dpointPos = time.find(":", 0);
                if (pointPos > dpointPos) {
                    return tmp[0] + "h" + tmp[1] + "m" + tmp[2] + "s" + tmp[3].substr(0,3) + "ms";
                } else {
                    int d = to_int64(tmp[0].c_str(), tmp[0].size());
                    int h = to_int64(tmp[1].c_str(), tmp[1].size());
                    h += d * 24;
                    return to_string(h) + "h" + tmp[2] + "m" + tmp[3] + "s";
                }
            }
            return "1h30m";
        }
        std::string convertTime(int64_t time) {
            //1000 - секунда
            TimeSpan tmp = time;
            return tmp.human("Hhm'm's's'S'ms'");
        }

        InsatConfig(): Cycle(0), AutoSave(false) {}

        InsatConfig(const Value& config) {
            if (!config.IsObject())
                return;
            FsType = GetSafeStringValue(config, "FsType");
            AutoSave = GetSafeBoolValue(config, "AutoSave");
            DbSize = to_string(GetSafeIntValue(config["limits"], "maxSize")) + "mb";
            Cycle = GetSafeIntValue(config, "Cycle");
            Lt = convertTime(GetSafeInt64Value(config["limits"], "maxTime"));
            Interval = convertTime(GetSafeStringValue(config, "AutoSaveInterval"));
            Duration = convertTime(GetSafeStringValue(config, "IdleDuration"));
            NoAck = GetSafeBoolValue(config, "NoAck");
            NoLayers = GetSafeBoolValue(config, "NoLayers");
        }
    };
    struct ArchiveCfg {
        int64_t id;            // Id архива (Id элемента проекта)
        int64_t max_size;      // Максимальный объём в хранения в байтах
        size_t max_recs;       // Максимальное количество записей
        size_t max_arch_size;  // Максимальное кол-во записей в арзиве
        size_t write_per_op;  // Максимальное кол-во записей записываемых за одну операцию
        size_t delete_per_op;  // Максимальное кол-во записей удаляемых за одну операцию
        TimeSpan max_time;      // Максимальное время хранения в filetime интервалах
        TimeSpan write_period;  // Период записи в filetime интервалах
        TimeSpan clear_period;  // Период очистки в filetime интервалах, default = 500ms
        TimeSpan vacuum_time;  // Период вызова функций освобождения данных на диске
        TimeSpan stat_check_period;  // Период подсчета статистики (сейчас совпадает с vacuum_time)
        TimeSpan stop_write_timeout;  // Таймаут ожидания записи значений при остановке
        int write_buf_size;  // Максимальный размер очереди на запись
        bool sync;           // Резервировать данные

        lib::optional<InsatConfig> insat;

        ArchiveCfg()
            : id(0), max_size(1024 * 1024 * 1024), max_recs(1000), max_arch_size(0), write_per_op(10000),
              delete_per_op(10000), max_time(TimeSpan::Day), write_period(TimeSpan::Milliseconds(500)), clear_period(0),
              vacuum_time(0), stat_check_period(0), write_buf_size(10000), sync(false),
              stop_write_timeout(TimeSpan::Minutes(1)) {}

        explicit ArchiveCfg(const Value& config): id(0), max_arch_size(0), sync(false) {
            write_buf_size = 10000;
            delete_per_op = 10000;
            write_per_op = 10000;
            write_period = 500 * FT_MILLISECOND;
            vacuum_time = FT_HOUR;
            if (!config.IsObject())
                return;
            max_recs = GetSafeInt64Value(config, "maxValuesCount");
            max_size = GetSafeInt64Value(config, "maxSize") * 1024 * 1024;
            max_time = GetSafeInt64Value(config, "maxTime");
            clear_period = GetSafeInt64Value(config, "cleanupPeriod");
            write_period = GetSafeInt64Value(config, "writePeriod", write_period.ticks());
            delete_per_op = GetSafeUIntValue(config, "maxDelValues", delete_per_op);
            write_per_op = GetSafeUIntValue(config, "maxWriteValues", write_per_op);
            write_buf_size = GetSafeUIntValue(config, "writeBufferSize", write_buf_size);
            stat_check_period = vacuum_time = GetSafeInt64Value(config, "vacuumTime", vacuum_time.ticks());
            stop_write_timeout = GetSafeInt64Value(config, "stopWriteTimeout", stop_write_timeout.ticks());
        }
    };
    struct ConnectionCfg {
        int64_t id;  // Id архива (Id элемента проекта)
        std::string db;
        std::string user;
        std::string pass;
        std::string host;
        int port;
        Type type;

        std::string provider;
        std::string parameters;

        FileTime reconnect_timeout;
        int connections_count;
        int write_connections_count;

        lib::optional<InsatConfig> insat;
        ConnectionCfg()
            : id(0), port(0), type(Type::Other), reconnect_timeout(FT_SECOND), connections_count(0),
              write_connections_count(0) {}

        ConnectionCfg(const Value& config, Type type = Type::Other): id(0), type(type) {
            if (!config.IsObject())
                return;
            db = GetSafeStringValue(config, "location");
            user = GetSafeStringValue(config, "user");
            pass = GetSafeStringValue(config, "pass");
            host = GetSafeStringValue(config, "host");
            port = GetSafeIntValue(config, "port");

            provider = GetSafeStringValue(config, "providerType");
            parameters = GetSafeStringValue(config, "parameters");

            reconnect_timeout = GetSafeInt64Value(config, "reconnectTimeout", FT_SECOND);
            connections_count = GetSafeIntValue(config, "connectionsCount", 0);
            write_connections_count = GetSafeIntValue(config, "writeThreadsCount", 0);
            connections_count += write_connections_count;
            if (write_connections_count == 0)
                write_connections_count = 1;
        }
    };


}}  // namespace mplc::archive
