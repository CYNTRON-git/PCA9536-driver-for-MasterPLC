#pragma once
#include "events_archive_share.h"
#include "events_condition.h"
#include <mplc/libs/threads.hpp>

#include "events_archive_request.h"

namespace mplc { namespace events {
    struct IEventsArchiveProcFactory;
    class EventsSubscription;
    class CEventInstanceDef;
    /*OpcUa_StatusCode TestFilter(const IEventBase& event,
                                const std::vector<EventsCondition::ptr>& conditions,
                                int64_t object_id,
                                const std::string& path,
                                bool& res);*/

    MPLC_EVENTS_API void
        StartedEventsLoaded();  // Заглушка на время пока не реализована нормальная загрузка собщений перед стартом
    class EventsArchiveManager final {
        typedef std::map<std::string, IEventsArchiveProcFactory*> FactoryMap;
        std::map<int64_t, IEventsArchiveProc::ptr> archives;
        std::vector<IEventsArchiveProc::ptr> archives_by_pos;
        // std::map<int, IEventsArchiveProc::ptr> archives;
        FactoryMap factories;
        int64_t _defaultArchiveId;
        mutable lib::mutex mtx_archives;
        mutable lib::mutex mtx_instances;
        /*
        typedef std::map<std::string, EventsArchiveSourceFactory*> EventsArchiveSourceFactoryMap;
        static EventsArchiveSourceFactoryMap _eventsArchiveSourceFactoryMap;*/
        std::vector<CEventInstanceDef*> event_instances;
        /*typedef std::map<mplc::triple<int64_t, int64_t, std::string>, CEventInstanceDef*> EventsByIdMap;
        EventsByIdMap eventsByIdMap;*/
        bool m_stop{false};
        // std::vector<IEventsArchiveSource::ptr> eventsArchiveSources;
        // typedef std::map<int64_t, IEventsArchiveSource::ptr> EventsArchiveSourceMap;
        // EventsArchiveSourceMap _eventsArchiveSourcesMap;
        struct impl;
        impl* pimpl;
        EventsArchiveManager();

    public:
        ~EventsArchiveManager();
        // IEventsArchiveSource::ptr GetEventsArchiveSource(int eventsArchiveIndex) const;
        MPLC_EVENTS_API CEventInstanceDef* GetEventInstanceById(int64_t itemId,
                                                                int64_t eventTypeId,
                                                                const std::string& path,
                                                                const std::string& external_path) const;
        MPLC_EVENTS_API int64_t GetLastEventTime(int archive_id, int64_t itemId) const;
        MPLC_EVENTS_API CEventInstanceDef* CreateInstanceDef(int64_t itemId,
                                                             const std::string& path,
                                                             int64_t eventTypeId,
                                                             const std::string& eventType,
                                                             const std::string& external_path);
        MPLC_EVENTS_API std::vector<CEventInstanceDef*> AllEventInstances() const;
        MPLC_EVENTS_API OpcUa_StatusCode AddRequest(lib::weak_ptr<EventsArchiveRequest> req, int archive_id) const;
        MPLC_EVENTS_API OpcUa_StatusCode AddRequest(lib::shared_ptr<DeleteEventsRequest>& request,
                                                    int archive_id) const;
        MPLC_EVENTS_API static EventsArchiveManager& instance();
        // MPLC_EVENTS_API OpcUa_StatusCode CountEvents(int64_t object_id,
        //                                             const std::string& path,
        //                                             int64_t archive_id,
        //                                             const std::vector<EventsCondition::ptr>& conditions,
        //                                             int& count) const;
        // MPLC_EVENTS_API IEventsArchiveSource::ptr GetEventsArchiveSourceById(int64_t archiveId);

        // MPLC_EVENTS_API IEventsArchiveProc::ptr GetArchiveById(int64_t archiveId);
        MPLC_EVENTS_API IEventsArchiveProc::ptr GetArchive(int64_t archive_id) const;
        MPLC_EVENTS_API void GetTableSizeInfo(archive::TableSizeInfo& table_info, int archive_id) const;
        MPLC_EVENTS_API void GetStatistics(CountStatistics& stats, int archive_id) const;
        MPLC_EVENTS_API void CheckConnected(ArchiveConnectionStatus& status, int archive_id) const;
        MPLC_EVENTS_API std::string GetHost(int archive_id) const;
        MPLC_EVENTS_API int64_t GetMaxTableSize(int64_t archive_id) const;
        MPLC_EVENTS_API void regFactory(const std::string& name, IEventsArchiveProcFactory* factory);
        EventsArchiveRequest::ContinuationPoint getContinuationPoint() const;

        // void Stop() { stop = true; }
        OpcUa_StatusCode Init();
        void Stop();
        void Start();
        MPLC_EVENTS_API void ClearArchives();
        MPLC_EVENTS_API void ClearCache();
        void WaitEventsLoaded() const;
        OpcUa_StatusCode RefreshEvents(EventsSubscription& sub) const;
        void addArchive(int db_id, const Value& config);
        int64_t GetDefaultArchiveId() const {
            return _defaultArchiveId;
        }
    };

}}  // namespace mplc::events
