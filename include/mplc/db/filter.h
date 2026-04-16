#pragma once
#include <mplc/db/config.h>
namespace mplc { namespace archive {
    inline OpcUa_DateTime operator-(const OpcUa_DateTime& l, const OpcUa_DateTime& r) {
        OpcUa_DateTime dt;
        dt.dwLowDateTime = l.dwLowDateTime - r.dwLowDateTime;
        dt.dwHighDateTime = l.dwHighDateTime - r.dwHighDateTime;
        return dt;
    }

    struct Filter {
        double deadband;  // Мертвая зона
        bool write_by_change;  // запись по изменению (если false, то только периодическая запись)
        int64_t t_min;  // Минимальный период записи (значения пишутся не чаще данного периода) в мс
        int64_t t_max;  // максимальный период записи (значения пишутся с данным периодом, даже если
                        // не менялись)  в мс
        bool all;
        bool automatic_write;  //запись из ФБ архивирования
        mutable FileTime last_reported_invalid_time;  // Время последней записи с некорректным временем

        MPLC_DATABASE_API Filter();
        MPLC_DATABASE_API Filter(const Value& config);
        MPLC_DATABASE_API void load(const Value& config);
        MPLC_DATABASE_API bool operator()(const PinValue::ptr& last_rec, const PinValue::ptr& new_rec) const;
        MPLC_DATABASE_API bool operator()(const PinValue::ptr& prev_rec,
                                          const OpcUa_VariantHlp& var,
                                          FileTime time,
                                          OpcUa_StatusCode sc) const;
    };
}}  // namespace mplc::archive
