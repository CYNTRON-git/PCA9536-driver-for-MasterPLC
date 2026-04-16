#pragma once
// #include <boost/chrono/time_point.hpp>
//#include <boost/chrono/system_clocks.hpp>
#include <share/config.h>
#include <cstdint>

#include "date_time.h"
#include "time_span.h"

namespace mplc {
    /**
     * \brief Класс для представления монотонного таймера и ровно числу тиков прошедших с момента старта системы
     */
    class TimePoint {
    public:
        TimePoint(): m_time(0) {}
        TimePoint(const TimePoint&) = default;
        TimePoint& operator=(const TimePoint&) = default;
        TimePoint(TimePoint&& time) noexcept = default;
        TimePoint& operator=(TimePoint&& time) noexcept = default;

        TimePoint& operator-=(const TimeSpan& time) {
            m_time -= time.ticks();
            return *this;
        }
        TimePoint& operator+=(const TimeSpan& time) {
            m_time += time.ticks();
            return *this;
        }

        TimePoint operator-(const TimeSpan& time) const {
            return m_time - time.ticks();
        }
        TimeSpan operator-(const TimePoint& time) const {
            return m_time - time.m_time;
        }

        TimePoint operator+(const TimeSpan& time) const {
            return m_time + time.ticks();
        }

        bool operator<(const TimePoint& time) const {
            return m_time < time.m_time;
        }

        bool operator>(const TimePoint& time) const {
            return m_time > time.m_time;
        }

        bool operator==(const TimePoint& time) const {
            return m_time == time.m_time;
        }

        bool operator!=(const TimePoint& time) const {
            return m_time != time.m_time;
        }

        bool operator<=(const TimePoint& time) const {
            return m_time <= time.m_time;
        }
        void clear() {
            m_time = 0;
        }
        bool operator>=(const TimePoint& time) const {
            return m_time >= time.m_time;
        }
        //
        /**
         * \brief Конвертирует текущее относительное время в DateTime
         * \return По текущему относительному времени возвращает системное время.
         */
        DateTime date_time() const {
            return DateTime::now() + (TimePoint::now() - *this);
        }
        MPLCSHARE_API static TimePoint now();
        explicit operator bool() const {
            return m_time != 0;
        }
        std::string str() const {
            return (*this - TimePoint(0)).str();
        }

        int64_t tiks() const {
            return m_time;
        }

    private:
        TimePoint(int64_t val): m_time(val) {}
        operator int64_t() const {
            return m_time != 0;
        }
        int64_t m_time;
    };
}  // namespace mplc
