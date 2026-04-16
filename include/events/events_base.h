#pragma once
#include <events/events_archive_share.h>
#include <events/events_condition.h>

namespace mplc { namespace events {
    class CEventInstanceDef;

    // Базовый интерфейс сообщения, через который служба подписки получает значения его полей
    class IEventBase {
    public:
        struct Field {
            std::string name;
            OpcUa_BuiltInType type;
        };
        virtual ~IEventBase() = default;
        MPLC_EVENTS_API virtual OpcUa_StatusCode GetFieldValue(int id,
                                                               lib::string_view name,
                                                               OpcUa_VariantHlp* pValue) const = 0;
        MPLC_EVENTS_API virtual int64_t GetUniqueEventId() const = 0;
        MPLC_EVENTS_API virtual const lib::unordered_map<std::string, OpcUa_VariantHlp>& GetOtherFields() const = 0;
        MPLC_EVENTS_API virtual bool IsChild(int64_t object_id, const std::string& path = "") const;
        MPLC_EVENTS_API virtual OpcUa_StatusCode TestFilter(const std::vector<EventsCondition::ptr>& conditions,
                                                            bool& res) const;
        MPLC_EVENTS_API virtual CEventInstanceDef* instance() const {
            return nullptr;
        }
        // MPLC_EVENTS_API virtual int64_t GetArchiveId() const = 0;
    };

    // Тип уведомления
    enum EEventRecType {
        EventRecTypeUpdate,  // Уведомление в результате изменения состояния сообщения
        EventRecTypeRefresh,       // Уведомление в результате команды Refresh
        EventRecTypeUpdateArchive  // Уведомление в результате изменения состояния архивного сообщения
    };

    // Уведомление
    struct EventRec {
        int _clientHandle;       // клиентский Id выборки
        EEventRecType _recType;  // тип уведомления
        OpcUa_VariantArray _fieldValues;  // срез текущих значений сообщения на момент формирования
                                          // нового уведомления (порядок определяется выборкой)
    };

}}  // namespace mplc::events
