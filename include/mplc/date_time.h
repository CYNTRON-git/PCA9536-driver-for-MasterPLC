#pragma once
#include <cstdint>
#include <string>
#include <includes.h>
#include <share/config.h>
#include <mplc/libs/string_view.hpp>
#include <cmath>
#include "time_span.h"
#undef unix

struct _OpcUa_DateTime;
struct RTIME;
class OpcUa_VariantHlp;
struct _OpcUa_Variant;
namespace mplc {
    /**
     * \brief Класс для представления времени системы аналогично FileTime
     * но с более строгими проверками на конвертации для работы с TimeSpan
     */
    class MPLCSHARE_API DateTime {
    private:
    public:  // ------------    PUBLIC STRUCTURES     ------------
        enum Duration { tick, us, ms, s, m, h, d, w };

    public:  // ------------       CONSTRUCTORS       ------------
        static TimeSpan local_tz();
        DateTime(): m_date_time(0) {}
        DateTime(const DateTime&) = default;
        DateTime& operator=(const DateTime&) = default;
        DateTime(DateTime&& time) noexcept = default;
        DateTime& operator=(DateTime&& time) noexcept = default;
        operator _OpcUa_DateTime() const;

        /**
         * \param year 0-16000
         * \param month 1-12
         * \param day 1-31
         * \param hour 0-23
         * \param minute 0-59
         * \param sec 0-59
         * \param msec 0-999
         * \param usec 0-999
         */
        DateTime(int year, int month, int day, int hour = 0, int minute = 0, int sec = 0, int msec = 0, int usec = 0);
        DateTime(int64_t ft, Duration duration);
        DateTime(const _OpcUa_DateTime& ft);
        DateTime(const _FILETIME& ft);
        DateTime(const struct tm& val);

    public:  // ------------        OPERATORS        ------------
        template<class T>
        DateTime& operator|=(const T& ft) {
            if (m_date_time == 0) {
                m_date_time = from(ft).ticks();
            }
            return *this;
        }

        template<class T>
        DateTime& operator=(const T& ft) {
            m_date_time = from(ft).ticks();
            return *this;
        }

        /*template<class T>
        operator T() const {
            T val;
            into(val);
            return std::move(val);
        }*/
        DateTime& operator-=(const TimeSpan& rv) {
            auto tmp_date_time = m_date_time - rv.ticks(); // TODO: DateTime by design must be never be negative I think
            if (tmp_date_time > 0) {  // Todo :: do it better
                m_date_time = tmp_date_time;
            }
            return *this;
        }
        DateTime& operator+=(const TimeSpan& rv) {
            auto tmp_date_time = m_date_time + rv.ticks();
            if (tmp_date_time > 0) { // Todo :: do it better
                m_date_time = tmp_date_time;
            }
            return *this;
        }
        TimeSpan operator-(const DateTime& val) const {
            return m_date_time - val.m_date_time;
        }
        DateTime operator-(const TimeSpan& rv) const {
            return m_date_time - rv.ticks();
        }
        DateTime operator-(const int rv) const {
            return m_date_time - rv;
        }

        DateTime operator+(const TimeSpan& rv) const {
            return m_date_time + rv.ticks();
        }
        DateTime operator+(const int rv) const {
            return m_date_time + rv;
        }
        DateTime operator+(const int64_t rv) const {
            return m_date_time + rv;
        }

        bool operator<(const DateTime& val) const {
            return m_date_time < val.m_date_time;
        }
        bool operator>(const DateTime& val) const {
            return m_date_time > val.m_date_time;
        }
        bool operator==(const DateTime& val) const {
            return m_date_time == val.m_date_time;
        }
        bool operator!=(const DateTime& val) const {
            return m_date_time != val.m_date_time;
        }
        bool operator<=(const DateTime& val) const {
            return m_date_time <= val.m_date_time;
        }
        bool operator>=(const DateTime& val) const {
            return m_date_time >= val.m_date_time;
        }
        explicit operator bool() const {
            return m_date_time != 0;
        }

    public:  // ------------     GETTERS/CONVERTORS     ------------
        static constexpr bool is_leap(const int year) {
            return (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
        }
        static int days_in_month(const int year, const int month) {
            static int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if (month < 1 || month > 12) {
                return 0;
            }
            return month == 2 && is_leap(year) ? 29 : days[month - 1];
        }

        static DateTime from_unix(const time_t unix_t);
        static DateTime now();
        static DateTime from_string(lib::string_view str);
        static int64_t convert(int64_t val, Duration duration);

        template<class T>
        T into() const {
            T tmp;
            into(tmp);
            return std::move(tmp);
        }
        static DateTime from(const int& val) {
            return static_cast<int64_t>(val);
        }

        /* -------------  std::tm -----------------*/

        static DateTime from(const struct tm& val);

        void into(struct tm& res) const;

        /* -------------  _OpcUa_DateTime -----------------*/

        static DateTime from(const _OpcUa_DateTime& ft);

        void into(_OpcUa_DateTime& val) const;

        /* -------------  OpcUa_VariantHlp -----------------*/

        static DateTime from(const OpcUa_VariantHlp& val);

        void into(OpcUa_VariantHlp& res) const;

        // ----------- _FILETIME

        static DateTime from(const _FILETIME& ft) {
            return static_cast<int64_t>(ft.dwHighDateTime) << 32 | static_cast<int64_t>(ft.dwLowDateTime);
        }

        void into(_FILETIME& ft) const {
            memcpy(&ft, &m_date_time, sizeof(_FILETIME));
        }

        /* -------------  double -----------------*/

        static DateTime from(const double& val);

        void into(double& val) const;

        /* -------------  int64_t -----------------*/

        static DateTime from(const long int& val) {
            return static_cast<int64_t>(val);
        }

        static DateTime from(const long long& val) {
            return static_cast<long long>(val);
        }

        /* -------------  RTIME -----------------*/

        static DateTime from(const RTIME& val);

        void into(RTIME& res) const;

        int64_t tod(Duration d = tick) const;
        int64_t date() const;
        int64_t time(Duration d) const {
            return convert(m_date_time, d);
        }
        int64_t ticks() const {
            return m_date_time;
        }
        int weekday() const;
        int64_t unix(Duration duration = s) const;
        int64_t unix_s() const {
            return unix(s);
        }
        int64_t unix_ms() const {
            return unix(ms);
        }
        /* Format string for debug (Slow)
            '   The escape for text
            Y	The year				2002
            M	The month				April & 04
            D	The day of the month	20
            h	The hour(12-hour time)	12
            H	The hour(24-hour time)	00
            m	The minute				45
            s	The second				52
            S	The millisecond			970
            u	The microsecond			220
        */
        std::string human(const std::string& format = "Y.M.D H:m:s.S\\'u") const;

        // strftime format
        size_t to_string(const char* format, char* buf, size_t buf_size) const;
        std::string to_string() const;
        int months_diff(DateTime date) const;

        // std::string rfc3339() const;

    public:  // ------------      MODIFIERS     ------------
        void round_up(Duration d);
        void clear();
        DateTime& add_months(int months);
        static void update_tz();

    protected:
        DateTime(const int64_t ft): m_date_time(ft) {}

        int64_t m_date_time;
    };

}  // namespace mplc
