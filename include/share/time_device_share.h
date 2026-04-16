#pragma once
#include <string>
#include <share/config.h>
#include <mplc/date_time.h>

// перечислимый тип архива
enum TypeOfArchive { HOURARCHIVE = 0, DAYARCHIVE = 1, MONTHARCHIVE = 2, HALFHOURARCHIVE = 3 };

// класс времени устройства
class TimeDevice : public mplc::DateTime {
    void Refresh();  // обновить целые числа
public:
    uint32_t UnixTime = 0;
    int Year = 0;
    int Month = 0;
    int Day = 0;
    int Hour = 0;
    int Minute = 0;
    int Second = 0;
    bool Init = false;
    MPLCSHARE_API void InitByElement(int YearIn,
                                     int MonthIn = 1,
                                     int DayIn = 1,
                                     int HourIn = 0,
                                     int MinuteIn = 0,
                                     int SecondIn = 0);  // инициализация по элементам
    MPLCSHARE_API static TimeDevice local_time();

    TimeDevice() = default;

    MPLCSHARE_API TimeDevice(DateTime dt);
    MPLCSHARE_API TimeDevice& operator=(DateTime dt);
    TimeDevice(const TimeDevice& dt) noexcept = default;
    TimeDevice& operator=(const TimeDevice& dt) noexcept = default;
    TimeDevice(TimeDevice&& dt) noexcept = default;
    TimeDevice& operator=(TimeDevice&& dt) noexcept = default;
    MPLCSHARE_API TimeDevice(uint32_t UnixTimeIn);  // конструктор из UnixTime

    // конструктор из отдельных чисел
    MPLCSHARE_API TimeDevice(int YearIn, int MonthIn, int DayIn, int HourIn, int MinuteIn, int SecondIn);

    MPLCSHARE_API TimeDevice(tm SctuctTm);

    MPLCSHARE_API tm GetTm() const;

    MPLCSHARE_API void SubMinute();  // вычесть одну минуту

    MPLCSHARE_API std::string ToString() const;

    // сравнение времени, true если полученное больше текущего
    // MPLCSHARE_API bool CompareWith(TimeDevice& Dev, TypeOfArchive Type);

    MPLCSHARE_API void Zero(TypeOfArchive Type);  // округлить время в меньшую сторону в зависимости от типа архива

    MPLCSHARE_API void Zero();  // округлить прибавив секунду

    // прибавить нужное количество интервалов в зависимости от типа архива
    MPLCSHARE_API void AddElementTime(int Col, TypeOfArchive Type);

    uint32_t GetUnixTime() const {
        return unix_s();
    }

    MPLCSHARE_API void AddMinutes(int Col);  // прибавить нужное количество минут

    MPLCSHARE_API void AddSeconds(int Col);  // прибавить нужное количество секунд

    /*TimeDevice& operator=(const TimeDevice& T)
    {
        if (this != &T)
        {
            UnixTime = T.UnixTime;
            t = from_time_t(UnixTime);
            tm cttm = to_tm(t);
            Refresh();
            Init = true;
        }
        return* this;
    }*/
};

MPLCSHARE_API int TimeDeviceDiff(const TimeDevice& Tim1, const TimeDevice& Tim2, TypeOfArchive Type);
// разница между датами в часах/сутка/месяцах, в зависимости от типа архива
