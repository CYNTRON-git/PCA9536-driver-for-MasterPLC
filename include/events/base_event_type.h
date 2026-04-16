#pragma once
#include <events/events_archive_share.h>
#include <events/system_events.h>
#include "event_instance_def.h"

namespace mplc { namespace events {

    class BaseEventType {
        bool _historizing;
        bool increment_time_if_equal;
        //IEventsArchiveSource::weak_ptr _eventsArchiveSource;
        CEventInstanceDef* instance{};

    public:
        static int64_t MakeSystemEventTypeId(system_events::EventTypeGroup group, int typeId) {
            return (((uint32_t)group) << 16) | (uint32_t)typeId;
        }
        static system_events::EventTypeGroup GetSystemEventTypeGroup(int64_t eventTypeId) {
            return (system_events::EventTypeGroup)(eventTypeId >> 16);
        }

        static std::string GetStringEventType(int64_t event_type_id, const std::string& event_type) {
            int eventGroup = event_type.empty() ? BaseEventType::GetSystemEventTypeGroup(event_type_id) : 0;
            if (event_type.empty()) {
                switch (eventGroup) {
                case system_events::AuditEvent:
                    return "AuditEvent";
                case system_events::SystemAuditEvent:
                    return "SystemAuditEvent";
                case system_events::SystemEvent:
                    return "SystemEvent";
                default:
                    break;
                }
            }
            return event_type;
        }

        void setSeverity(int severity);
        EventsArchiveRec& getRec();
        MPLC_EVENTS_API void setOtherField(const std::string& key, const OpcUa_VariantHlp& val);
        void changeItem(int64_t itemId, const std::string& path) { instance = CEventInstanceDef::Get(itemId, 0, path); }
        void changeArchive(int64_t archiveId);
        MPLC_EVENTS_API void setIncrementTimeIfEqual(bool value);

        MPLC_EVENTS_API ~BaseEventType(){};

        MPLC_EVENTS_API BaseEventType(int archiveId,
                                      int64_t itemId,
                                      const std::string& path,
                                      int severity,
                                      int64_t eventTypeId = 0,
                                      const std::string& event_type = std::string());
        MPLC_EVENTS_API BaseEventType(int64_t itemId,
                                      const std::string& path,
                                      int severity,
                                      int64_t eventTypeId = 0,
                                      const std::string& event_type_name = std::string());

        MPLC_EVENTS_API OpcUa_StatusCode FireEvent(int64_t time,
                                                   const std::string& message,
                                                   const std::string& source = std::string(),
                                                   const std::string& user = std::string(),
                                                   bool acked = true,
                                                   int64_t ack_time = 0,
                                                   const std::string& comment = std::string());
    };

}}  // namespace mplc::events
