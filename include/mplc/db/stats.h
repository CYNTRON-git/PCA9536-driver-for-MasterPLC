#pragma once
//#include <mplc/database.h>
#include "mplc/async/paralel_task_pool.h"
#include <mplc/db/config.h>

namespace mplc {
    struct CountStatistics {
        CountStatistics(): ReadCount(0), WriteCount(0), LostCount(0), Waiting(0) {}
        CountStatistics(int64_t r, int64_t w, int64_t l, int64_t ww)
            : ReadCount(r), WriteCount(w), LostCount(l), Waiting(ww) {}
        int64_t ReadCount, WriteCount, LostCount, Waiting;
    };

    class Stats : async::AsyncLogger {
        lib::mutex mtx;
        int64_t rq_count, rq_queue;
        int64_t r_count, w_count, d_count;
        int64_t r_time, w_time, d_time;
        int64_t waiting, lose;
        int64_t intervals;
        std::string m_header;
        MPLC_DATABASE_API void log() override;

    public:
        const char* get_header() const {
            return m_header.c_str();
        }
        MPLC_DATABASE_API Stats(const char* name);
        MPLC_DATABASE_API void set_header(std::string header);
        MPLC_DATABASE_API void update_request(int64_t count, int64_t queue);
        MPLC_DATABASE_API void update_read(int64_t time, int64_t count);
        MPLC_DATABASE_API void update_write(int64_t time, int64_t count);
        MPLC_DATABASE_API void update_delete(int64_t time, int64_t count);
        MPLC_DATABASE_API void set_wait_data(int64_t count);
        MPLC_DATABASE_API void add_lose_data(int64_t count);
        MPLC_DATABASE_API void skiped_intervals(int64_t count);

        CountStatistics getStatistics() const {
            return {r_count, w_count, lose, waiting};
        }
    };

}  // namespace mplc
