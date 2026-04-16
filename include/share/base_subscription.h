#pragma once
#include "share/timer.h"

class BaseSubscription {
protected:
    // Время последнего обращщения по данной подписке (счетчик времени в мс.)
    // OpcUa_DateTime _lastAccessTime;

    Timer _activityTimer;
    int _lifetimeInterval;
    const int _maxRecordsSize;
    uint32_t _changeToSlaveCounter;

public:
    virtual ~BaseSubscription() = default;
    BaseSubscription(): _lifetimeInterval(0), _maxRecordsSize(100), _changeToSlaveCounter(0) {}

    bool IsValid() const {
        if (_changeToSlaveCounter != GetChangeToSlaveCounter())
            return false;
        return true;
    }

    bool IsActive(int subNum, const char* type) {
        int64_t ms = _activityTimer.DurationMS();
        if (!IsValid()) {
            PRINTLN("Delete invalid sub(%d) %s CurTimeMS=%lld Time=%llx ChangeToSlaveCounter=%ud (%ud)",
                    subNum,
                    type,
                    ms,
                    _activityTimer.TimeStart(),
                    _changeToSlaveCounter,
                    GetChangeToSlaveCounter());
            return false;
        }

        if (ms > _lifetimeInterval) {
            PRINTLN("Inactive sub(%d) %s CurTimeMS=%lld Time=%llx", subNum, type, ms, _activityTimer.TimeStart());
            return false;
        } else
            return true;
    }
};
