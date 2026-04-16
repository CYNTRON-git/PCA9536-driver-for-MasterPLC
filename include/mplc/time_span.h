#pragma once
#include <share/config.h>
#include <cstdint>
#include <string>
#include <mplc/libs/string_view.hpp>

namespace mplc {

    /**
     * \brief Класс для представления временого интервала, значение хранится в FILETIME тиках
     */
    class MPLCSHARE_API TimeSpan {
    public:
        constexpr TimeSpan(): m_time(0) {}
        TimeSpan(const TimeSpan&) = default;
        TimeSpan& operator=(const TimeSpan&) = default;
        TimeSpan(TimeSpan&& time) noexcept = default;
        TimeSpan& operator=(TimeSpan&& time) noexcept = default;

        TimeSpan& operator=(int64_t val) {
            m_time = val;
            return *this;
        }
        TimeSpan& operator=(double val) {
            m_time = static_cast<int64_t>(val * Millisecond);
            return *this;
        }
        TimeSpan(int64_t val): m_time(val) {}
        TimeSpan(int val): m_time(val) {}
        TimeSpan(double val): m_time(static_cast<int64_t>(val * Millisecond)) {}
        TimeSpan& operator-=(const TimeSpan& time) {
            m_time -= time.m_time;
            return *this;
        }

        TimeSpan& operator+=(const TimeSpan& time) {
            m_time += time.m_time;
            return *this;
        }

        TimeSpan operator-() const {
            return -m_time;
        }

        TimeSpan operator-(const TimeSpan& time) const {
            return m_time - time.m_time;
        }

        TimeSpan operator+(const TimeSpan& time) const {
            return m_time + time.m_time;
        }

        TimeSpan operator*(int64_t count) const {
            return m_time * count;
        }
        TimeSpan operator*=(int64_t count) const {
            return m_time * count;
        }
        TimeSpan operator/(int64_t count) const {
            return m_time / count;
        }

        TimeSpan operator*(int count) const {
            return m_time * count;
        }

        TimeSpan operator/(int count) const {
            return m_time / count;
        }

        bool operator<(const TimeSpan& time) const {
            return m_time < time.m_time;
        }

        bool operator>(const TimeSpan& time) const {
            return m_time > time.m_time;
        }

        bool operator==(const TimeSpan& time) const {
            return m_time == time.m_time;
        }

        bool operator!=(const TimeSpan& time) const {
            return m_time != time.m_time;
        }

        bool operator<=(const TimeSpan& time) const {
            return m_time <= time.m_time;
        }

        bool operator>=(const TimeSpan& time) const {
            return m_time >= time.m_time;
        }
        int64_t ticks() const {
            return m_time;
        }
        int64_t weaks() const {
            return m_time / Weak;
        }
        int64_t days() const {
            return m_time / Day;
        }
        int64_t hours() const {
            return m_time / Hour;
        }
        int64_t minutes() const {
            return m_time / Minute;
        }
        int64_t sec() const {
            return m_time / Second;
        }
        int64_t msec() const {
            return m_time / Millisecond;
        }
        int64_t usec() const {
            return m_time / Microsecond;
        }

        double weaks_f() const {
            return static_cast<double>(m_time) / Weak;
        }
        double days_f() const {
            return static_cast<double>(m_time) / Day;
        }
        double hours_f() const {
            return static_cast<double>(m_time) / Hour;
        }
        double minutes_f() const {
            return static_cast<double>(m_time) / Minute;
        }
        double sec_f() const {
            return static_cast<double>(m_time) / Second;
        }
        double msec_f() const {
            return static_cast<double>(m_time) / Millisecond;
        }
        double usec_f() const {
            return static_cast<double>(m_time) / Microsecond;
        }
        void clear() {
            m_time = 0;
        }

        static TimeSpan Weaks(int count = 1) {
            return Weak * count;
        }
        static TimeSpan Days(int count = 1) {
            return Day * count;
        }
        static TimeSpan Hours(int count = 1) {
            return Hour * count;
        }
        static TimeSpan Minutes(int count = 1) {
            return Minute * count;
        }
        static TimeSpan Seconds(int count = 1) {
            return Second * count;
        }
        static TimeSpan Milliseconds(int count = 1) {
            return Millisecond * count;
        }
        static TimeSpan Microseconds(int count = 1) {
            return Microsecond * count;
        }
        static TimeSpan Ticks(int64_t count = 1) {
            return Tick * count;
        }
        static TimeSpan from_string(lib::string_view str);
        explicit operator bool() const {
            return m_time != 0;
        }
        /* Format string
            '   The escape for text
            W	Weaks
            D	Days
            H	Hour
            m	Minutes
            s	Seconds
            S	Milliseconds
            u	Microseconds
        */
        std::string human(const std::string& format = "D H:m:s.S\\'u") const;
        std::string str() const;
        // TODO:
        // strftime format
        // MPLCSHARE_API size_t to_string(const char* format, char* buf, size_t buf_size) const;
        // MPLCSHARE_API static TimeSpan from_string(const char* str);

        static const TimeSpan Tick;
        static const TimeSpan Microsecond;
        static const TimeSpan Millisecond;
        static const TimeSpan Second;
        static const TimeSpan Minute;
        static const TimeSpan Hour;
        static const TimeSpan Day;
        static const TimeSpan Weak;

    private:
        operator int64_t() const {
            return m_time;
        }

        int64_t m_time;
    };
}  // namespace mplc
