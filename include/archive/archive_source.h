#ifndef __ARCHIVE_SOURCE_H__
#define __ARCHIVE_SOURCE_H__

#include <boost/optional.hpp>
#include "mplc/db/table_size_info.h"


//Базовый класс для архива
class ArchiveSource : public BaseSource {
    struct CltLock {
        void* ID;
        int64_t Time;
        bool Allowed;
    };

public:
    ArchiveSource()
        : _clst_sec("clstArchiveSourceCS") {
        _archiveSourceIndex = 0;
    }
    virtual ~ArchiveSource() { StopThread(); }

    int GetIndex() { return _archiveSourceIndex; }

    MPLC_ARCHIVE_API virtual OpcUa_StatusCode Init(int index, const Value* archiveSettings);

    bool MPLC_ARCHIVE_API AddCltLock(void* ID);

    MPLC_ARCHIVE_API virtual OpcUa_StatusCode GetArchiveItemId(int64_t id,
                                                               const std::string& path,
                                                               const std::string& name,
                                                               ArchiveItemIdType& archiveItemId) = 0;
    MPLC_ARCHIVE_API virtual OpcUa_StatusCode FindArchiveItemId(int64_t id,
                                                                const std::string& path,
                                                                ArchiveItemIdType& archiveItemId) = 0;
    MPLC_ARCHIVE_API virtual OpcUa_StatusCode FindByArchiveItemId(ArchiveItemIdType archiveItemId,
                                                                  int64_t& id,
                                                                  std::string& path) = 0;
    MPLC_ARCHIVE_API virtual OpcUa_StatusCode AddValue(ArchiveItemIdType archiveItemId, const ArchiveRec& rec) = 0;
    MPLC_ARCHIVE_API virtual OpcUa_StatusCode AddValueEx(ArchiveItemIdType archiveItemId,
                                                         const ArchiveRec& rec,
                                                         bool cacheOnly,
                                                         bool ignore_filter,
                                                         bool allow_insert, bool use_custom_last_value) = 0;
    MPLC_ARCHIVE_API virtual OpcUa_StatusCode SetDisableWriteByChange(ArchiveItemIdType archiveItemId, bool disable) = 0;
    MPLC_ARCHIVE_API virtual bool GetDisableWriteByChange(ArchiveItemIdType archiveItemId) = 0;
    MPLC_ARCHIVE_API virtual OpcUa_StatusCode AddRequest(ArchiveRequest* request) = 0;
    OpcUa_StatusCode virtual SetFilter(ArchiveItemIdType archiveItemId,
                                       boost::optional<bool> all = boost::optional<bool>(),
                                       boost::optional<bool> write_by_change = boost::optional<bool>(),
                                       boost::optional<int64_t> t_max = boost::optional<int64_t>(),
                                       boost::optional<int64_t> t_min = boost::optional<int64_t>(),
                                       boost::optional<double> deadband = boost::optional<double>(),
                                       boost::optional<bool> automatic_write = boost::optional<bool>()) {
        if (write_by_change)
            return SetDisableWriteByChange(archiveItemId, *write_by_change);
        return OpcUa_Good;
    }
    MPLC_ARCHIVE_API virtual OpcUa_StatusCode GetLastTime(ArchiveItemIdType itemID, int64_t& lastTime) {
        return OpcUa_Good;
    }
    MPLC_ARCHIVE_API virtual OpcUa_StatusCode RemoveRequest(ArchiveRequest* request) = 0;

protected:

    CCriticalSection _clst_sec;
    std::vector<CltLock> _CltLocks;

private:
    int _archiveSourceIndex;
};

class ArchiveSourceFactory {
public:
    virtual ArchiveSource* CreateArchiveSource() = 0;
};

#endif  //__ARCHIVE_SOURCE_H__
