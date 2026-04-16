#pragma once
#include "events_archive_share.h"
#include "events_base.h"
#include "events_subscription_base.h"

namespace mplc { namespace events {
    class CEventInstanceDef;

    class EventsArchiveRec : public IEventBase {
    public:  // ================ PUBLIC FIELDS ================
        enum update_type { None, Activeted, Deactiveted, Acknowledged };
        IEventsRequestProcessor* requestProcessor;
        CEventInstanceDef* eventInstance;
        FileTime Time;
        FileTime ActiveTime;
        FileTime InActiveTime;
        FileTime AckedTime;  // Время квитирования
        int64_t RecId;
        int Severity;

        update_type UpdateType;
        bool Active;
        bool Acked;
        std::string Message;
        std::string Comment;
        std::string ClientAddress;
        std::string ClientUserId;
        bool CopyArchiveFields;
        // Дополнительые поля, определенные в производных классах
        // сообщения. Напрямую передаются в службу подписок
        lib::unordered_map<std::string, OpcUa_VariantHlp> OtherFields;
        bool reserved{};

    public:  // ================ CONSTRUCTORS/DESTRUCTOR ================
        MPLC_EVENTS_API EventsArchiveRec();
        ~EventsArchiveRec() override {}
        MPLC_EVENTS_API EventsArchiveRec(EventsArchiveRec&& rec) noexcept;
        MPLC_EVENTS_API EventsArchiveRec& operator=(EventsArchiveRec&& rec) noexcept;
        MPLC_EVENTS_API EventsArchiveRec(const EventsArchiveRec& rec);
        MPLC_EVENTS_API EventsArchiveRec& operator=(const EventsArchiveRec& rec);

    public:  // ================     PUBLIC METHODS      ================
        MPLC_EVENTS_API int GetArchiveAlarmId() const;
        MPLC_EVENTS_API void SetArchiveAlarmId(int id);
        MPLC_EVENTS_API OpcUa_StatusCode WriteField(lib::string_view field, ResponseWriter& writer) const;
        void Clear();
        uint32_t GetEventState() const {
            return ((Active) ? 0x2 : 0x0) | ((Acked) ? 0x1 : 0x0);
        }
        uint32_t GetEventStateSeverity() const {
            if (UpdateType == Deactiveted)
                return 40000 + Severity;
            if (UpdateType == Acknowledged)
                return 50000 + Severity;
            return GetEventState() * 10000 + Severity;
        }
        MPLC_EVENTS_API const lib::unordered_map<std::string, OpcUa_VariantHlp>& GetOtherFields() const override;
        MPLC_EVENTS_API int64_t GetUniqueEventId() const override;

        MPLC_EVENTS_API OpcUa_StatusCode EventChanged(EventsArchiveRec& rec, EEventRecType recType) const;
        MPLC_EVENTS_API OpcUa_StatusCode GetFieldValue(int id,
                                                       lib::string_view name,
                                                       OpcUa_VariantHlp* pValue) const override;
        MPLC_EVENTS_API CEventInstanceDef* instance() const override {
            return eventInstance;
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const EventsArchiveRec& rec) {
        os << "  AlarmId      : " << rec.GetArchiveAlarmId() << '\n';
        os << "  Time         : " << FileTime(rec.Time).human() << '\n';
        os << "  ActiveTime   : " << FileTime(rec.ActiveTime).human() << '\n';
        os << "  UpdateType   : " << rec.UpdateType << '\n' << '\n';
        os << "  InActiveTime : " << FileTime(rec.InActiveTime).human() << '\n';
        os << "  AckedTime    : " << FileTime(rec.AckedTime).human() << '\n';
        os << "  Acked        : " << std::boolalpha << rec.Acked << '\n';
        os << "  Active       : " << std::boolalpha << rec.Active << '\n';
        return os;
    }
}}  // namespace mplc::events
