#pragma once
#include "addincmn.h"
#include "events_archive_share.h"
// #include "events_request_processor.h"
#include "share/addins_share.h"
#include "share/addin_base.h"

namespace mplc::async {
    class task_t;
}

namespace mplc::events {
    class EventsArchiveManager;
    class EventsRequestProcessor;
    class IEventsRequestProcessor;
    struct events_backup_service_t;
    struct events_backup_client_t;
    // Базовый класс модуля
    class EventsAddin final : public AddinBase {
        static EventsAddin* Instance;
        EventsArchiveManager& events_archive;
        lib::shared_ptr<events_backup_service_t> backup_service;
        lib::shared_ptr<events_backup_client_t> backup_client;
        lib::thread backup_th;
        CCriticalSection _sec;
        lib::shared_ptr<async::task_t> project_integrity_check_task;
        lib::shared_ptr<async::task_t> system_integrity_check_task;
        EventsAddin();

    public:
        EventsRequestProcessor* RequestProcessor;
        OpcUa_StatusCode Init(ProcessRequestCallback func);

        static MPLC_EVENTS_API EventsAddin* GetInstance();
        static MPLC_EVENTS_API IEventsRequestProcessor* GetRequestProcessor();
        MPLC_EVENTS_API lib::shared_ptr<events_backup_service_t> GetBackupService();

    protected:
        // Вызывается при запуске/останове конфигурации системы
        OpcUa_StatusCode OnConfigEvent(ConfigProcessorMode mode, ControllerConfig* config) override;
    };
}  // namespace mplc::events
