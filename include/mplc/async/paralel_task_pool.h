#pragma once
#include <share/config.h>
#include <mplc/libs/sync_timed_queue.hpp>
#include <mplc/libs/bind.hpp>
#include <mplc/libs/threads.hpp>
#include <mplc/libs/smart_ptr.hpp>
#include <mplc/time_point.h>
#include <mplc/time_span.h>

namespace mplc { namespace async {
    typedef lib::function<void()> parallel_task_t;
    class worker_t;
    class task_t {
        friend class TaskPool;

    public:
        enum State { waiting, running, finished, canceled, exception_handled };
        typedef lib::shared_ptr<task_t> ptr;
        typedef lib::weak_ptr<task_t> weak_ptr;
        task_t() = delete;
        task_t(const task_t&) = delete;
        task_t& operator=(const task_t&) = delete;
        task_t(task_t&&) = delete;
        task_t& operator=(task_t&&) = delete;

        ~task_t() {
            cancel();
        }
        int state() const {
            return m_state;
        }
        void cancel() {
            lib::unique_lock lk(mtx);
            m_task.clear();
            m_state = canceled;
        }
        const std::string name;
        const TimeSpan repeat_delay;

    protected:
        void run();
        task_t(std::string name, parallel_task_t task, TimeSpan repeat_delay = {});

    private:
        /*template<class... Args>
        static ptr make(Args&&... args) {
            return lib::make_shared<task_t>(std::forward<Args>(args)...);
        }*/
        parallel_task_t m_task;
        // std::exception_ptr ex_hndl;
        lib::atomic<State> m_state{waiting};
        lib::recursive_mutex mtx;
    };

    using AsyncTask = task_t::ptr;
    class TaskPool {
        using task_weak_ref = task_t::weak_ptr;
        using queue = lib::cuncurent::sync_timed_queue<task_weak_ref>;
        using op_status = queue::op_status;
        queue main_q;
        std::vector<lib::thread> _workers;
        mutable lib::mutex mtx_workers;

        void WorkerFn();
        void Finalize(const AsyncTask& task, TimePoint t_start);

    public:
        enum { Stopped, Running, PendingStop };
        TaskPool(const TaskPool&) = delete;
        TaskPool(TaskPool&&) = delete;
        TaskPool& operator=(const TaskPool&) = delete;
        TaskPool& operator=(TaskPool&&) = delete;
        MPLCSHARE_API ~TaskPool();
        MPLCSHARE_API int Status() const;
        MPLCSHARE_API void Start(size_t threads_count);
        MPLCSHARE_API void Stop();
        MPLCSHARE_API void Clear();
        MPLCSHARE_API TaskPool();
        MPLCSHARE_API AsyncTask CreateTask(std::string name,
                                           parallel_task_t fn,
                                           TimeSpan repeat_delay = 0,
                                           TimeSpan start_delay = 0);
    };

    MPLCSHARE_API AsyncTask CreateRepeatedTask(std::string name, TimeSpan period, parallel_task_t fn);
    MPLCSHARE_API AsyncTask CreateDelayedTask(std::string name, TimeSpan delay, parallel_task_t fn);
    MPLCSHARE_API AsyncTask CreateTask(std::string name,
                                       parallel_task_t fn,
                                       TimeSpan repeat_delay = 0,
                                       TimeSpan start_delay = 0);

    class AsyncLogger {
        AsyncTask hndl;

    public:
        MPLCSHARE_API AsyncLogger(const char* name, TimeSpan ft_period);
        AsyncLogger(const AsyncLogger&) = delete;
        AsyncLogger& operator=(const AsyncLogger&) = delete;
        AsyncLogger(AsyncLogger&& task) noexcept {
            hndl.swap(task.hndl);
        }
        AsyncLogger& operator=(AsyncLogger&& task) noexcept {
            hndl.swap(task.hndl);
            return *this;
        }
        MPLCSHARE_API virtual ~AsyncLogger();
        virtual void log() {}
    };

    /*class AsyncLogger {
        AsyncTask
    public:
        MPLCSHARE_API AsyncLogger(int64_t ft_period);
        AsyncLogger(const AsyncLogger&) = delete;
        AsyncLogger& operator=(const AsyncLogger&) = delete;
        AsyncLogger(AsyncLogger&& task) noexcept = default;
        AsyncLogger& operator=(AsyncLogger&& task) noexcept = default;
        ~AsyncLogger() override = default;
    };*/

}}  // namespace mplc::async
