#ifndef _ARCHIVE_SHARE_H_
#define _ARCHIVE_SHARE_H_
#include <mplc/libs/threads.hpp>
#include <mplc/libs/chrono.hpp>

struct ArchiveRec {
    ArchiveRec(): Time(0), ServerTime(0), Quality(OpcUa_Good), TypeHash(0) {}
    int64_t Time;
    int64_t ServerTime;
    OpcUa_StatusCode Quality;
    uint32_t TypeHash;
    OpcUa_VariantHlp Value;
};

typedef int32_t ArchiveItemIdType;
typedef uint64_t ComposedArchiveItemIdType;
#define ArchiveItemIdNull (0)

enum EArchiveRequestFlags {
    ArchiveRequestReturnBounds = 0x1,
    ArchiveRequestReturnFirstLastValue = 0x2,
    GetArchivedItems = 0x4
};

typedef std::vector<ArchiveRec> ArchiveRecArr;
struct ArchiveRequestItem {
    typedef boost::shared_ptr<ArchiveRequestItem> ptr;
    static ptr make(ArchiveItemIdType archiveItemId, const std::string& path = "") {
        return boost::make_shared<ArchiveRequestItem>(archiveItemId, path);
    }
    ArchiveRequestItem(ArchiveItemIdType archiveItemId, const std::string& path = "")
        : ItemId(archiveItemId), Path(path), ContinuationPoint(0), StatusCode(OpcUa_Good) {}
    ~ArchiveRequestItem() {}
    ArchiveItemIdType ItemId;
    std::string Path;
    int64_t ContinuationPoint;
    OpcUa_StatusCode StatusCode;
    ArchiveRecArr Values;
    mplc::lib::function<void(const ArchiveRec&)> callback;
};

typedef std::vector<ArchiveRequestItem::ptr> ArchiveRequestItemArr;

//Запрос на чтение данных
struct ArchiveRequest {
    enum ArchiveRequestState { NotStarted, Started, Finished, Sent, Received };

    ArchiveRequest()
        : SemCompleted(NULL), StartTime(OpcUa_DateTime_Null), EndTime(OpcUa_DateTime_Null), NumValuesPerNode(0),
          Flags(0), ResampleInterval(0), RequestState(NotStarted) {}
    CSemaphore* SemCompleted;
    ArchiveRequestItemArr Items;
    OpcUa_DateTime StartTime;
    OpcUa_DateTime EndTime;
    int NumValuesPerNode;
    int Flags;  // EArchiveRequestFlags
    double ResampleInterval;
    volatile ArchiveRequestState RequestState;

    void Wait(void) const {
        if (SemCompleted != NULL) {
            SemCompleted->Wait(1);
        } else {
            while (RequestState != Finished) {
                mplc::lib::this_thread::sleep_for(mplc::lib::chrono::milliseconds(100));
            }
        }
    }
};

#endif  // _ARCHIVE_SHARE_H_
