#pragma once

#ifdef _WIN32
#    ifdef MPLC_EVENTS_EXPORTS
#        define MPLC_EVENTS_API __declspec(dllexport)
#    else
#        define MPLC_EVENTS_API __declspec(dllimport)
#    endif
#else
#    define MPLC_EVENTS_API
#endif
#include <mplc/libs/smart_ptr.hpp>
#include <mplc/db/table_size_info.h>

#include "mplc/db/custom_request.h"

namespace mplc {
    struct ArchiveConnectionStatus;
    struct CountStatistics;
}  // namespace mplc

namespace mplc { namespace events {
    struct DeleteEventsRequest;
    class EventsArchiveRequest;
    class EventsArchiveRec;

    enum EventsRequestType {
        tReadEventRec,
        tInsertEventRec,
        tInsertEventsAlarmsRawData,
        tInsertFieldRec,
        tUpdateEventRec,
        tCopyOtherFields,
    };

    //Базовый класс архива сообщений
    struct IEventsArchiveSource {
        virtual OpcUa_StatusCode AddValue(const EventsArchiveRec& rec) = 0;
        virtual OpcUa_StatusCode AddRequest(lib::weak_ptr<EventsArchiveRequest> request) = 0;
        virtual OpcUa_StatusCode AddRequest(lib::shared_ptr<DeleteEventsRequest> request) = 0;
        virtual OpcUa_StatusCode AddRequest(db::CustomRequest::ptr request) = 0;

        // virtual ArchiveEventInstanceDef* GetEventsDef(int64_t itemId, const std::string& path, int64_t eventTypeId) =
        // 0;
        virtual void GetTableSizeInfo(archive::TableSizeInfo& table_info) const = 0;
        virtual void GetStatistics(CountStatistics& stats) = 0;
        virtual void CheckConnected(ArchiveConnectionStatus& status) const = 0;
        virtual int64_t GetMaxEventsRecsSize() const = 0;

        virtual ~IEventsArchiveSource() {}
        ADD_PTR_TYPEDEF(IEventsArchiveSource);
    };
    struct IEventsArchiveProc {
        virtual ~IEventsArchiveProc() {}
        typedef lib::shared_ptr<IEventsArchiveProc> ptr;

        virtual IEventsArchiveSource& getArchive() = 0;
        virtual void run() = 0;
        virtual void clear() = 0;
        virtual void stop() = 0;
        virtual void getStatus(ArchiveConnectionStatus& status) = 0;
        virtual std::string getHost() = 0;
    };

}}  // namespace mplc::events
