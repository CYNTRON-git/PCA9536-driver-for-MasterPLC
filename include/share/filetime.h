#pragma once
#include <string>
#include <cstring>
//#include <mplc_stdint.h>
//#include <rapidjson/document.h>
#include <boost/static_assert.hpp>
#include "share/config.h"
#include <opcua.h>
#include <main.h>

struct lua_State;
struct _OpcUa_DateTime;
class OpcUa_VariantHlp;

#undef unix
class FileTime {
public:
    enum Duration { raw, us, ms, s, m, h, d, w };
    static constexpr int64_t Microsecond = 10;
    static constexpr int64_t Millisecond = 1000 * Microsecond;
    static constexpr int64_t Second = 1000 * Millisecond;
    static constexpr int64_t Minute = 60 * Second;
    static constexpr int64_t Hour = 60 * Minute;
    static constexpr int64_t Day = 24 * Hour;
    static constexpr int64_t Weak = 7 * Day;

    // Return true if file_time < 1970.1.1
    MPLCSHARE_API bool isTime() const;
    MPLCSHARE_API static int64_t local_tz();
    FileTime(): file_time(0) {}
    MPLCSHARE_API FileTime(int64_t count, Duration duration = raw);
    explicit FileTime(const FileTime& rv): file_time(rv.file_time) {}
    template<class T>
    FileTime& operator=(const T& ft) {
        file_time = from(ft);
        return *this;
    }
    template<class T>
    FileTime(const T& val): file_time(from(val)) {}
    template<class T>
    operator T() const {
        return into<T>();
    }
    template<class T>
    FileTime operator-(const T& rv) const {
        return file_time - from(rv);
    }
    template<class T>
    FileTime operator-=(const T& rv) {
        return file_time -= from(rv);
    }
    template<class T>
    FileTime operator+(const T& rv) const {
        return file_time + from(rv);
    }
    template<class T>
    FileTime operator+=(const T& rv) {
        return file_time += from(rv);
    }
    template<class T>
    bool operator<(const T& rv) const {
        return file_time < from(rv);
    }
    template<class T>
    bool operator>(const T& rv) const {
        return file_time > from(rv);
    }
    template<class T>
    bool operator==(const T& rv) const {
        return file_time == from(rv);
    }
    template<class T>
    bool operator!=(const T& rv) const {
        return file_time != from(rv);
    }
    template<class T>
    bool operator<=(const T& rv) const {
        return file_time <= from(rv);
    }
    template<class T>
    bool operator>=(const T& rv) const {
        return file_time >= from(rv);
    }

    operator int64_t() const {
        return file_time;
    }

    //#ifdef BOOST_COMP_MSVC
    template<class T>
    static int64_t from(const T& val) {
        BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, "Not Implemented");
        return 0;
    }
    template<class T>
    T into() const {
        BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, "Not Implemented");
        return {};
    }
    //#else
    //    template<class T>
    //    static int64_t from(const T& val);
    //    template<class T>
    //    T into() const;
    //#endif

    bool operator==(const FileTime& rvl) const {
        return file_time == rvl.file_time;
    }
    MPLCSHARE_API static FileTime from_unix(time_t unix_t, Duration duration = s) {
        return FileTime(int64_t(unix_t) * cv[duration] + 116444736000000000ll);
    }
    MPLCSHARE_API OpcUa_StatusCode from_lua(lua_State* L, int idx);
    MPLCSHARE_API void to_lua(lua_State* L) const;
    MPLCSHARE_API static int64_t convert(int64_t val, Duration duration);
    MPLCSHARE_API bool isNull() const;
    MPLCSHARE_API void clear();
    // MPLCSHARE_API static int64_t now();
    MPLCSHARE_API static FileTime now();
    //{ return getInt64FileTime(); }
    MPLCSHARE_API int64_t tod(Duration d = raw) const;
    MPLCSHARE_API int64_t date() const;
    int64_t time(Duration d = raw) const {
        return convert(file_time, d);
    }
    int64_t dt() const {
        return file_time;
    }
    MPLCSHARE_API int weekday() const;
    MPLCSHARE_API int64_t unix(Duration duration = s) const;
    int64_t unix_s() const {
        return unix(s);
    }
    int64_t unix_ms() const {
        return unix(ms);
    }
    /* Format string
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
    MPLCSHARE_API std::string human(const std::string& format = "Y-M-D H:m:s.S") const;

    // strftime format
    MPLCSHARE_API size_t to_string(const char* format, char* buf, size_t buf_size) const;

    /*friend const rapidjson::Value& operator>>(const rapidjson::Value& json, FileTime& v) {
        v = json.GetInt64();
        return json;
    }*/
    MPLCSHARE_API static void update_tz();

private:
    int64_t file_time;
    MPLCSHARE_API static const int64_t cv[];
    static constexpr int64_t _1970_1_1 = 25569 * Day;
    static constexpr int64_t const_DTdiffFT = 94353120000000000LL;  // 6552300
    static constexpr int64_t UNIX_DIFF_FT = 11644473600000LL;
};

template<>
inline int64_t FileTime::from<FileTime>(const FileTime& val) {
    return val.file_time;
}
// ------- Into

template<>
inline int64_t FileTime::from<long int>(const long int& ft) {
    return ft;
}
template<>
inline int64_t FileTime::from<long long>(const long long& ft) {
    return ft;
}
template<>
inline int64_t FileTime::from<int>(const int& ft) {
    return ft;
}
template<>
inline int64_t FileTime::from<FILETIME>(const FILETIME& ft) {
    return static_cast<int64_t>(ft.dwHighDateTime) << 32 | static_cast<int64_t>(ft.dwLowDateTime);
}

/* --- _OpcUa_DateTime ---- */
template<>
MPLCSHARE_API int64_t FileTime::from<_OpcUa_DateTime>(const _OpcUa_DateTime& ft);
template<>
MPLCSHARE_API _OpcUa_DateTime FileTime::into<_OpcUa_DateTime>() const;
template<>
/* ----------------------- */

inline int64_t FileTime::from<double>(const double& ft) {
    return ft * cv[ms];
}
template<>
MPLCSHARE_API int64_t FileTime::from<OpcUa_VariantHlp>(const OpcUa_VariantHlp& val);
// ------- Into
template<>
inline FileTime FileTime::into<FileTime>() const {
    return *this;
}
template<>
inline bool FileTime::into<bool>() const {
    return file_time != 0;
}
template<>
MPLCSHARE_API OpcUa_VariantHlp FileTime::into<OpcUa_VariantHlp>() const;

template<>
inline FILETIME FileTime::into<FILETIME>() const {
    FILETIME ft;
    std::memcpy(&ft, &file_time, sizeof(FILETIME));
    return ft;
}
template<>
inline double FileTime::into<double>() const {
    return static_cast<double>(file_time) / cv[ms];
}
template<>
inline int64_t FileTime::into<int64_t>() const {
    return file_time;
}
template<>
inline int FileTime::into<int>() const {
    return file_time;
}
template<>
MPLCSHARE_API int64_t FileTime::from<tm>(const tm& t);
template<>
MPLCSHARE_API tm FileTime::into<tm>() const;

template<>
MPLCSHARE_API int64_t FileTime::from<RTIME>(const RTIME& t);
template<>
MPLCSHARE_API RTIME FileTime::into<RTIME>() const;

// template<>
// inline unsigned int FileTime::into<unsigned int>() const {
//    return file_time;
//}
