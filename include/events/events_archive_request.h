#pragma once
#include <mplc/libs/threads.hpp>
#include "events_archive_rec.h"
#include "events_condition.h"
#include <mplc/date_time.h>

#include "mplc/time_point.h"

namespace mplc { namespace events {
    // Описания поля архива сообщения
    //  struct ArchiveEventInstanceDef {
    //     ArchiveEventInstanceDef(int64_t itemId = 0, std::string path = "", int64_t eventTypeId = 0)
    //         : ArchiveAlarmId(0), ItemId(itemId), EventTypeId(eventTypeId), Path(path) {}
    //     int ArchiveAlarmId;
    //     int64_t ItemId;
    //     int64_t EventTypeId;
    //     std::string Path;
    //     std::string EventType;
    // };

    // Описания поля архива сообщения
    class ArchiveEventsField {
    private:
        int id;
        int typeHash;
        std::string name;
        OpcUa_BuiltInType type;

    public:
        int GetFieldId() const {
            return id;
        }
        const std::string& GetFieldName() const {
            return name;
        }
        int GetFieldTypeHash() const {
            return typeHash;
        }
        OpcUa_BuiltInType GetFieldType() const {
            return type;
        }

        void SetFieldId(int _id) {
            id = _id;
        };
        void SetFieldName(std::string _name) {
            name = _name;
        };
        void SetFieldTypeHash(int type_hash) {
            typeHash = type_hash;
        };

        ArchiveEventsField(): id(0), typeHash(0), type(OpcUaType_Null) {}

        ArchiveEventsField(int _id, const std::string& _name, std::string _type): typeHash(0) {
            id = _id;
            name = _name;
            type = OpcUa_VariantHlp::GetUaTypeFromName(_type);
        }

        OpcUa_StatusCode Load(const Value& field) {
            id = GetSafeIntValue(field, "id", 0);
            name = GetSafeStringValue(field, "name");
            type = OpcUa_VariantHlp::GetUaTypeFromName(GetSafeStringValue(field, "type"));
            typeHash = GetSafeIntValue(field, "typeHash", 0);
            return OpcUa_Good;
        }
    };

    class EventsArchiveRequest : public lib::enable_shared_from_this<EventsArchiveRequest> {
    public: // =================== PUBLIC STRUCTS ===================
        ADD_MAKE_PTR(EventsArchiveRequest)

        struct ContinuationPoint {
        public: // =================== fields ===================
            int64_t time;
            int64_t active_time;
            int32_t alarm_id;
            EventsArchiveRec::update_type update_type;

        public: // =================== constructors/destructor ===================
            ContinuationPoint(): time(0), active_time(0), alarm_id(0), update_type(EventsArchiveRec::None) {}
            ContinuationPoint(int64_t _time,
                              EventsArchiveRec::update_type _update_type,
                              int _alarm_id,
                              int64_t _active_time) {
                active_time = _active_time;
                time = _time;
                alarm_id = _alarm_id;
                update_type = _update_type;
            }

        public: // =================== operators ===================
            ContinuationPoint& operator=(const EventsArchiveRec& rec) {
                time = rec.Time;
                update_type = rec.UpdateType;
                alarm_id = rec.GetArchiveAlarmId();
                active_time = rec.ActiveTime;
                return *this;
            }
            ContinuationPoint& operator=(const std::string& b64) {
                if (b64.empty())
                    return *this;
                *this = mplc::from_base64<ContinuationPoint>(b64);
                return *this;
            }
            bool operator<(const EventsArchiveRec& rec) const {
                return time < rec.Time.dt() ||
                       time == rec.Time.dt() &&
                           (update_type < rec.UpdateType ||
                            update_type == rec.UpdateType &&
                                (alarm_id < rec.GetArchiveAlarmId() ||
                                 alarm_id == rec.GetArchiveAlarmId() && active_time < rec.ActiveTime.dt()));
            }
            bool operator==(const EventsArchiveRec& rec) const {
                return time == rec.Time.dt() && update_type == rec.UpdateType && alarm_id == rec.GetArchiveAlarmId() &&
                       active_time == rec.ActiveTime.dt();
            }
            bool operator>(const EventsArchiveRec& rec) const {
                return time > rec.Time.dt() ||
                       time == rec.Time.dt() &&
                           (update_type > rec.UpdateType ||
                            update_type == rec.UpdateType &&
                                (alarm_id > rec.GetArchiveAlarmId() ||
                                 alarm_id == rec.GetArchiveAlarmId() && active_time > rec.ActiveTime.dt()));
            }
        };

    public: // =================== CONSTRUCTORS/DESTRUCTOR ===================
        EventsArchiveRequest(): limit(0), inverse(false), sc(OpcUa_BadWaitingForResponse), completed(false) {}

    public: // ===================     PUBLIC METHODS      ===================
        OpcUa_StatusCode Load(const Document& request);
        void WriteToJson(ResponseWriter& writer);

        bool isWaiting() const {
            return TimePoint::now() - req_add < timeout;
        }

        void finish(OpcUa_StatusCode code) {
            sc = code;
            completed = true;
            if (_on_finished_cb) {
                _on_finished_cb(shared_from_this());
            }
            auto sem = semaphore.lock();
            if (sem)
                sem->Post();
        }

        MPLC_EVENTS_API void Wait(void) const;

        void set_on_finished(lib::function<void(EventsArchiveRequest::ptr)>&& cb) {
            _on_finished_cb = std::move(cb);
        }

    public: // ===================     PUBLIC FIELDS     ===================
        std::vector<ArchiveEventsField> fields;
        std::vector<std::string> other_fields;
        std::vector<EventsCondition::ptr> conditions;
        std::vector<EventsArchiveRec> values;
        ContinuationPoint cp;
        lib::weak_ptr<CSemaphore> semaphore;
        DateTime t_start, t_end;
        TimePoint req_add;
        int64_t limit;
        TimeSpan timeout;
        bool inverse;
        lib::atomic<OpcUa_StatusCode> sc;
        volatile bool completed;

    private: // ===================     PRIVATE FIELDS     ===================
        lib::function<void(EventsArchiveRequest::ptr)> _on_finished_cb;
    };

    struct DeleteEventsRequest {
        ADD_PTR_TYPEDEF(DeleteEventsRequest)
        DeleteEventsRequest()
            : all_before_rec_id(0), deleted_count(0), completed(false), semaphore(nullptr), archive_id(0) {}

        std::vector<int64_t> rec_ids;
        int64_t all_before_rec_id;
        size_t deleted_count;
        lib::atomic<bool> completed;
        CSemaphore* semaphore;
        int archive_id;
    };
}}  // namespace mplc::events
