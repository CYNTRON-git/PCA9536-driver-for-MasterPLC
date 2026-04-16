#pragma once

#include <share/scada_types.h>

#include "events_archive_rec.h"
#include "events_archive_share.h"

inline const char* get_lua_type(const std::string* str, AsUtf8*) {
    return get_lua_type(str);
};

namespace mplc { namespace events {
    class AlarmCondition : public ScadaFB {
    public:
        MPLC_FB_PARAMS(IN(ItemId, REF_TO),
                       IN(EventType, STRING),
                       // IN(Active, BOOL),  // set ActiveIn
                       IN(Severity, DINT),
                       // IN(Message, TemplateString, AsUtf8),  // set MessageIn
                       // IN(Comment, STRING, AsUtf8),          // set CommentIn
                       IN(ChangeTime, DT),
                       IN(ObjectId, mplc::vm::ObjectID),
                       OUT(AckedTime, DT),
                       OUT(ActiveTime, DT),  //Время активации последнего сообщения
                       OUT(InactiveTime, DT)
                       // IN(Acked, BOOL)
        )
        // set AckedIn
        bool ActiveIn, lastAckedIn;
        bool AckedIn, defaultAcked;
        // std::string MessageIn;
        std::string CommentIn;
        TemplateString MessageIn;

       friend class MultiAlarmCondition;
    private:
        // std::string outComment;
        /* Дополнительные поля, определенные в производных классах сообщения. Напрямую передаются в службу подписок */
        lib::unordered_map<std::string, OpcUa_VariantHlp> otherFields;

        void UpdateRec(EventsArchiveRec::update_type upd_type);

        bool Acknowledge();
        bool Activate();
        MPLC_EVENTS_API void InitEventInstance();

    protected:
        int SetField(const std::string& key, lua_State* L);
        int GetField(const std::string& key, lua_State* L) const;
        ENABLE_DYNAMIC_FIELDS(AlarmCondition, SetField, GetField);
        const vm::PtNode* vm_alarm;
        CEventInstanceDef* event_inst;

    public:
        int32_t eventTypeId;
        bool use_archive;

        OpcUa_VariantHlp& GetEventField(const std::string& name) {
            return otherFields[name];
        }
        MPLC_EVENTS_API bool IsActive();
        MPLC_EVENTS_API void Inited() override;
        MPLC_EVENTS_API AlarmCondition();
        MPLC_EVENTS_API virtual ~AlarmCondition() {}
        MPLC_EVENTS_API void Execute() override;

        MPLC_EVENTS_API bool InsertVariable(const std::string& variable, std::ostream& result) const;
        MPLC_EVENTS_API OpcUa_StatusCode SetEventState(bool active, RDateTime t = RDateTimeZero);
        MPLC_EVENTS_API OpcUa_StatusCode SetLastActiveTime(int64_t _time) const;
        MPLC_EVENTS_API void WriteArchive(EventsArchiveRec& record);
        MPLC_EVENTS_API CEventInstanceDef* GetEventInstance();
        MPLC_EVENTS_API void EventChanged(CEventInstanceDef* instanceDef,
                                          EventsArchiveRec* eventInstance,
                                          EEventRecType recType);
    };
}}  // namespace mplc::events
