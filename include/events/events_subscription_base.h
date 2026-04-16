#pragma once
#include <events/events_base.h>

namespace mplc { namespace events {
    class CEventInstanceDef;
    class EventsArchiveRec;

    // Базовый интерфейс подписки, накопливает уведомления для отправки клиенту
    class IEventsSubscription {
    public:
        MPLC_EVENTS_API virtual OpcUa_StatusCode OnEventChanged(CEventInstanceDef* instanceDef,
                                                                EventsArchiveRec* eventInstance,
                                                                EEventRecType recType) = 0;
        MPLC_EVENTS_API virtual void AddEventRec(const EventRec& rec){};
        MPLC_EVENTS_API virtual void AddDeletedEvent(int clientHandle, const OpcUa_VariantHlp& eventId){};
        MPLC_EVENTS_API virtual ~IEventsSubscription() {}
    };

    class IEventsRequestProcessor {
    public:
        MPLC_EVENTS_API virtual void CorrectNewRec(EventsArchiveRec* rec) = 0;
        MPLC_EVENTS_API virtual void SetClientForNewRec(const std::string& clientUserId) = 0;

        MPLC_EVENTS_API virtual OpcUa_StatusCode OnEventChanged(CEventInstanceDef* instanceDef,
                                                                EventsArchiveRec* eventInstance,
                                                                EEventRecType recType) = 0;
        MPLC_EVENTS_API virtual OpcUa_StatusCode AddEventsSubscription(IEventsSubscription* eventsSub) = 0;
        MPLC_EVENTS_API virtual OpcUa_StatusCode RemoveEventsSubscription(IEventsSubscription* eventsSub) = 0;
        MPLC_EVENTS_API virtual OpcUa_StatusCode GetArchivedEvents(const Document& request,
                                                                   ResponseWriter& writer) const = 0;
    };
}}  // namespace mplc::events
