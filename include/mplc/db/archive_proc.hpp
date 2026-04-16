#pragma once
#include <boost/ref.hpp>
#include <mplc/libs/bind.hpp>
#include <mplc/libs/smart_ptr.hpp>
#include <mplc/libs/threads.hpp>

#include "main.h"

#ifdef WIN32
#    ifdef MPLC_DATABASE_EXPORTS
#        define MPLC_DATABASE_API __declspec(dllexport)
#    else
#        define MPLC_DATABASE_API __declspec(dllimport)
#    endif
#else
#    define MPLC_DATABASE_API
#endif

namespace mplc {
    struct ArchiveConnectionStatus {
        ArchiveConnectionStatus(): connected(false), error(true), code(-1), msg("Unknown Error") {}
        ArchiveConnectionStatus(bool isConnected, bool isError, int statusCode, const std::string& message)
            : connected(isConnected), error(isError), code(statusCode), msg(message) {}
        bool connected;
        bool error;
        int code;
        std::string msg;
    };  // namespace ArchiveConnectionStatus
    namespace events {
        class AlarmCondition;
    }
}  // namespace mplc::events

namespace mplc { namespace archive {
    enum ConnectionPoolStatus { Good, ConnectionError, ReconnectPause };
    namespace __detail {
        MPLC_DATABASE_API events::AlarmCondition* create_alarm(int64_t);
        MPLC_DATABASE_API bool update_alarm(events::AlarmCondition* alarm,
                                            ConnectionPoolStatus code,
                                            const char* error);
        MPLC_DATABASE_API void send_alram(events::AlarmCondition* alarm, const char* msg);
        MPLC_DATABASE_API void clear_alarm(events::AlarmCondition* alarm);
        MPLC_DATABASE_API void exec_alarm(events::AlarmCondition* alarm);
    }  // namespace __detail

    class ConnectException : public std::runtime_error {
    public:
        ConnectException(const std::string& info): runtime_error(info) {}
    };

    struct IStrategy {
        template<class _IArchive, class _ConnectionPool>
        static void run(_IArchive arch, _ConnectionPool pool);
    };

    template<class _CfgType, class _ConnectionType>
    struct IConnectionPool {
        typedef const _CfgType config_type;
        typedef _ConnectionType connection_type;
        events::AlarmCondition* alarm;
        mplc::ArchiveConnectionStatus status;
        virtual bool isOpen() = 0;
        virtual void Stop() = 0;
        virtual bool NeedReconnect() = 0;
        virtual connection_type getConnect() = 0;
        virtual int count() = 0;
        virtual int writeCount() = 0;
        virtual std::pair<ConnectionPoolStatus, std::string> Connect() = 0;
        virtual bool Disconnect() = 0;
        virtual bool isLocal() const = 0;
        virtual int64_t GetId() const = 0;
        virtual std::string GetHost() = 0;

        virtual void InitAlarm() {
            alarm = __detail::create_alarm(GetId());
        }
        virtual void ExecAlarm() {
            __detail::exec_alarm(alarm);
        }
        IConnectionPool(): alarm(nullptr) {}
        virtual ~IConnectionPool() {
            __detail::clear_alarm(alarm);
        }
        bool DoConnect() {
            std::pair<ConnectionPoolStatus, std::string> state = Connect();

            if (state.first == ConnectionError) {
                status.code = state.first;
                status.connected = state.first == Good;
                status.error = state.first == ConnectionError;
                status.msg = mplc::cp1251_to_utf8(state.second);
            }

            return __detail::update_alarm(alarm, state.first, state.second.c_str());
        }
        void OnConnectionError(const std::string& msg) const {
            __detail::send_alram(alarm, msg.c_str());
        }
    };

    inline void EventDBWriteError(const std::string& msg, int64_t item_id) {
        FireSystemEvent(etgSystemEvent, seDatabaseWriteError, msg.c_str(), 500, item_id, NULL);
    }

    template<class _ConnectionPool, class _CfgType>
    struct IArchive {
        virtual ~IArchive() {}
        typedef _ConnectionPool connection_pool;
        typedef const _CfgType config_type;
        typedef typename _ConnectionPool::connection_type connect;

        virtual void RunRead(connect&) = 0;
        virtual void RunWrite(connect&) = 0;
        virtual void RunDelete(connect&) = 0;

        virtual void InitRead(connect&) = 0;
        virtual void InitWrite(connect&) = 0;
        virtual void InitDelete(connect&) = 0;

        virtual void wait() = 0;
        virtual void Stop() = 0;

        virtual bool isSync() const = 0;
        virtual bool Connected(_ConnectionPool&) = 0;
        virtual void Disconnected(_ConnectionPool&) = 0;
    };

    template<class _Archive, template<class, class> class _Strategy>
    struct ArchiveProc {
        typedef ArchiveProc<_Archive, _Strategy> _Mybase;
        typedef typename _Archive::connection_pool connection_pool;
        typedef typename connection_pool::config_type con_pool_cfg;
        typedef typename connection_pool::connection_type connection_type;
        typedef typename _Archive::config_type archive_cfg;
        _Archive archive;
        connection_pool pool;
        lib::thread* th;
        bool m_stop{false};
        // template run<_Archive, _ConnectionPool>
        static void Worker(_Archive& archive, connection_pool& pool) {
            while (pool.NeedReconnect()) {
                pool.ExecAlarm();
                try {
                    if (!pool.DoConnect()) {
                        archive.wait();
                        continue;
                    }
                    if (archive.Connected(pool)) {
                        _Strategy<_Archive, connection_pool>::run(archive, pool);
                        archive.Disconnected(pool);
                    } else
                        return;
                    pool.Disconnect();
                } catch (const std::exception& ex) {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Some error\n%s", ex.what());  //-V111
                } catch (ArchiveConnectionStatus& status) {
                    pool.status = status;
                } catch (...) {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Unexpected error");
                }
            }
        }

        typedef ArchiveProc<_Archive, _Strategy> base_proc;
        ArchiveProc(const archive_cfg& arch_cfg, const con_pool_cfg& con_cfg)
            : archive(arch_cfg), pool(con_cfg), th(nullptr) {
            pool.InitAlarm();
        }
        // typedef boost::shared_ptr<_Mybase> ptr;

        /*  static ptr make(const archive_cfg& arch_cfg, const con_pool_cfg& con_cfg) {
              return boost::make_shared<_Mybase>(arch_cfg, con_cfg);
          }*/

        void Start() {
            if (th && th->joinable()) {
                return;
            }
            m_stop = false;
            th = new lib::thread(&_Mybase::Worker, boost::ref(archive), boost::ref(pool));
        }
        void Stop() {
            try {
                m_stop = true;
                archive.Stop();
                pool.Stop();
            } catch (...) {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Unexpeted Error from: %s", BOOST_CURRENT_FUNCTION);
            }
        }
        void Clear() {
            if (!m_stop) {
                Stop();
            }
            if (th) {
                th->join();
                delete th;
                th = nullptr;
            }
        }

        bool isSync() const {
            return archive.isSync();
        }
        bool isLocal() const {
            return pool.isLocal();
        }
        virtual ~ArchiveProc() {
            Clear();
        }
        void GetStatus(ArchiveConnectionStatus& status) {
            status = pool.status;
        }
        std::string GetHost() {
            return pool.GetHost();
        }
    };

    template<class _IArchive, class _ConnectionPool>
    struct SinglThreadStrategy {
        static void run(_IArchive& arch, _ConnectionPool& pool) {
            if (!pool.isOpen())
                return;
            typedef typename _ConnectionPool::connection_type connection_type;

            connection_type connect = pool.getConnect();
            try {
                arch.InitRead(connect);
                arch.InitWrite(connect);
                arch.InitDelete(connect);
                while (pool.isOpen()) {
                    pool.ExecAlarm();
                    arch.RunDelete(connect);
                    arch.RunWrite(connect);
                    arch.RunRead(connect);
                    arch.wait();
                }
            } catch (const std::exception& ex) {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Some error\n%s", ex.what());  //-V111
            } catch (ArchiveConnectionStatus& status) {
                pool.status = status;
            } catch (...) {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Unexpected error");
            }
        }
    };
    template<class _Archive, class _ConnectionPool>
    struct RWThreadStrategy {
        static void run(_Archive& arch, _ConnectionPool& pool) {
            if (!pool.isOpen())
                return;
            typedef typename _ConnectionPool::connection_type connection_type;

            int readers_count = pool.count() - 1;
            if (readers_count < 1) {
                PRINTLN("WARNING: not enough connections for RWThreadStrategy");
                SinglThreadStrategy<_Archive, _ConnectionPool>::run(arch, pool);
                return;
            }
            std::vector<lib::thread*> threads(readers_count);
            try {
                for (int i = 0; i < readers_count; ++i) {
                    threads[i] = new lib::thread(&paralelRead, boost::ref(arch), boost::ref(pool));
                }
                connection_type writer = pool.getConnect();
                arch.InitWrite(writer);
                arch.InitDelete(writer);

                while (pool.isOpen()) {
                    pool.ExecAlarm();
                    arch.RunDelete(writer);
                    arch.RunWrite(writer);
                    arch.wait();
                }
            } catch (const std::exception& ex) {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Some error\n%s", ex.what());  //-V111
                pool.Disconnect();
            } catch (ArchiveConnectionStatus& status) {
                pool.status = status;
                pool.Disconnect();
            } catch (...) {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Unexpected error");
                pool.Disconnect();
            }

            for (int i = 0; i < readers_count; ++i) {
                if (!threads[i])
                    continue;
                threads[i]->join();
                delete threads[i];
            }
        }
        static void paralelRead(_Archive& arch, _ConnectionPool& pool) {
            typedef typename _ConnectionPool::connection_type connection_type;
            while (pool.isOpen()) {
                try {
                    connection_type con = pool.getConnect();
                    arch.InitRead(con);
                    while (pool.isOpen()) {
                        arch.RunRead(con);
                        arch.wait();
                    }
                } catch (const std::exception& ex) {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Some error\n%s", ex.what());  //-V111
                    pool.Disconnect();
                } catch (ArchiveConnectionStatus& status) {
                    pool.status = status;
                    pool.Disconnect();
                } catch (...) {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Unexpected error");
                    pool.Disconnect();
                }
            }
        }
    };
    template<class _Archive, class _ConnectionPool>
    struct AllParalelStrategy {
        static void run(_Archive& arch, _ConnectionPool& pool) {
            if (!pool.isOpen())
                return;
            typedef typename _ConnectionPool::connection_type connection_type;

            int connection_count = pool.count() - 1;
            if (connection_count < 2) {
                PRINTLN("WARNING: not enough connections for AllParalelStrategy");
                // connection_count = 2;
                RWThreadStrategy<_Archive, _ConnectionPool>::run(arch, pool);
                return;
            }
            std::vector<lib::thread*> threads(connection_count);

            for (int i = 0; i < pool.writeCount(); ++i) {
                threads[i] = new lib::thread(&paralelWrite, boost::ref(arch), boost::ref(pool));
            }
            for (int i = pool.writeCount(); i < connection_count; ++i) {
                threads[i] = new lib::thread(&paralelRead, boost::ref(arch), boost::ref(pool));
            }
            paralelDelete(arch, pool);

            for (int i = 0; i < connection_count; ++i) {
                if (!threads[i])
                    continue;
                threads[i]->join();
                delete threads[i];
            }
        }

        static void paralelRead(_Archive& arch, _ConnectionPool& pool) {
            typedef typename _ConnectionPool::connection_type connection_type;
            while (pool.isOpen()) {
                try {
                    connection_type con = pool.getConnect();
                    arch.InitRead(con);
                    while (pool.isOpen()) {
                        pool.ExecAlarm();
                        arch.RunRead(con);
                        arch.wait();
                    }
                } catch (const std::exception& ex) {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Some error\n%s", ex.what());  //-V111
                    pool.Disconnect();
                } catch (ArchiveConnectionStatus& status) {
                    pool.status = status;
                    pool.Disconnect();
                } catch (...) {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Unexpected error");
                    pool.Disconnect();
                }
            }
        }
        static void paralelWrite(_Archive& arch, _ConnectionPool& pool) {
            typedef typename _ConnectionPool::connection_type connection_type;
            while (pool.isOpen()) {
                try {
                    connection_type con = pool.getConnect();
                    arch.InitWrite(con);
                    while (pool.isOpen()) {
                        arch.RunWrite(con);
                        arch.wait();
                    }
                } catch (const std::exception& ex) {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Some error\n%s", ex.what());  //-V111
                    pool.Disconnect();
                } catch (ArchiveConnectionStatus& status) {
                    pool.status = status;
                    pool.Disconnect();
                } catch (...) {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Unexpected error");
                    pool.Disconnect();
                }
            }
        }
        static void paralelDelete(_Archive& arch, _ConnectionPool& pool) {
            typedef typename _ConnectionPool::connection_type connection_type;
            while (pool.isOpen()) {
                try {
                    connection_type con = pool.getConnect();
                    arch.InitDelete(con);
                    while (pool.isOpen()) {
                        arch.RunDelete(con);
                        arch.wait();
                    }
                } catch (const std::exception& ex) {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Some error\n%s", ex.what());  //-V111
                    pool.Disconnect();
                } catch (ArchiveConnectionStatus& status) {
                    pool.status = status;
                    pool.Disconnect();
                } catch (...) {
                    OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Unexpected error");
                    pool.Disconnect();
                }
            }
        }
    };

    template<class IArchiveProc>
    class ArchiveProcFactory {};
}}  // namespace mplc::archive
