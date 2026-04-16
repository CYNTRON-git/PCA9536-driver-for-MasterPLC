#pragma once


#include "archive/archive_statistics.h"
#include "share/base_macros.h"

// Структура для ограничений архива
struct ArchiveLimits {
    int64_t Id;
    // Максимальное кол-во записей в арзивк
    int64_t MaxArchLen;
    // Максимальная глубина хранения в 100 ns. интервалах
    int64_t MaxTime;
    // Период очистки в 100 ns. интервалах
    int64_t ClearPeriod;
    // мертвая зона
    double Deadband;
    // запись по изменению (по умолчанию true, если false, то только периодическая запись)
    bool Ch;
    int deletePerOp;
    // минимальный период записи (значения пишутся не чаще данного периода)
    int64_t TMin;
    // максимальный период записи (значения пишутся с данным периодом, даже если не менялись)
    int64_t TMax;
    // Путь к архиву
    std::string Path;
    // Настройка хранения транзакций https://sqlite.org/pragma.html#pragma_journal_mode
    std::string JournalMode;
    bool UseHDAServerAdapter;
    bool Sync;
    int64_t WriteTaskPeriod;
    // Период подсчета статистики (сейчас совпадает с vacuum_time)
    FileTime StatCheckPeriod;
    ArchiveLimits() {
        Id = 0;
        deletePerOp = 100000;
        WriteTaskPeriod = 0;
        MaxArchLen = 0;
        MaxTime = 0;
        ClearPeriod = 0;
        Deadband = 0.0;
        Ch = true;
        TMin = 0;
        TMax = 0;
        UseHDAServerAdapter = false;
        Sync = true;
        JournalMode = std::string("memory");
        StatCheckPeriod = 0;
    }
};

class BaseSource {
protected:
    mplc::lib::thread* _thread;
    bool _stop;
    bool _inited;
    bool _initError;

    // Максимальное кол-во записей, записываемых за одно обращение к архиву
    size_t _maxRecsToWrite;
    // Период статистики
    int64_t _statisticTime;

    MPLCSHARE_API virtual void LoadArchSettings(const Value* archiveSettings);

    // Статистика обработки архивов
    ArchiveStatistics Statistics;

    int64_t _beginStatTime;

    // Время последней очистки архива
    int64_t _lastClearTime;

    // Время последней записи в архив
    int64_t _lastWriteTime;

public:
    // Настройки архива
    ArchiveLimits ArchLimits;

    BaseSource(): _lastWriteTime(0) {
        _beginStatTime = 0;
        _thread = NULL;
        _stop = false;
        _inited = false;
        _initError = false;
        _lastClearTime = getTimePoint();
        _maxRecsToWrite = 1000;
        _statisticTime = 60 * FT_SECOND;
    }

    virtual ~BaseSource() {
        StopThread();
    }

protected:
    void StopThread() {
        if (_thread != NULL) {
            _thread->join();
            delete _thread;
            _thread = NULL;
        }
    }
};
