#pragma once
#include "events_archive_rec.h"
#include <mplc/libs/threads.hpp>
#include <mplc/libs/optional.hpp>

/*Описатель сообщения для службы подписки.
Через него идет взаимодействие со службой подписки, хранится состояние, используемое в службе
подписки*/
namespace mplc { namespace events {

    _PACKED(struct EventId {
        EventId() {
            active_time = 0;
            item_id = 0;
            event_type_id = 0;
            upd_type = 0;
            reserv = 0;
        }
        EventId(int64_t _item_id,
                int _event_type_id,
                EventsArchiveRec::update_type _update_type,
                int64_t _active_time) {
            active_time = _active_time;
            item_id = _item_id;
            event_type_id = _event_type_id;
            upd_type = _update_type;
            reserv = 0;
        }
        int64_t active_time;
        uint8_t upd_type;
        uint8_t reserv;
        int32_t event_type_id;
        int32_t item_id;
    });

    inline std::size_t event_instance_hash(int64_t itemId,
                                           const std::string& path,
                                           int64_t eventTypeId,
                                           const std::string& external_path) noexcept {
        std::size_t seed{};
        boost::hash_combine(seed, itemId);
        boost::hash_combine(seed, path);
        boost::hash_combine(seed, eventTypeId);
        boost::hash_combine(seed, external_path);
        return seed;
    }

    std::size_t event_instance_hash(const CEventInstanceDef& instance) noexcept;

    class CEventInstanceDef {
    public:
        ADD_PTR_TYPEDEF(CEventInstanceDef);

        struct KeyRef {
            const int64_t item_id;
            const std::string& item_path;
            const int64_t event_type_id;
            const std::string& external_path;
            friend bool operator==(const KeyRef& left, const KeyRef& right) {
                return left.item_id == right.item_id && left.event_type_id == right.event_type_id &&
                       left.item_path == right.item_path && left.external_path == right.external_path;
            }
            friend bool operator==(const KeyRef& left, const CEventInstanceDef& right) {
                return left.item_id == right.itemId.id && left.event_type_id == right.eventTypeId &&
                       left.item_path == right.itemId.path && left.external_path == right.externalPath;
            }
        };
        struct Hash {
            using is_transparent = int;
            size_t operator()(const KeyRef& key) const noexcept {
                return event_instance_hash(key.item_id, key.item_path, key.event_type_id, key.external_path);
            }
            size_t operator()(const CEventInstanceDef* inst) const noexcept {
                return event_instance_hash(*inst);
            }
        };
        struct EqualTo {
            using is_transparent = int;
            bool operator()(const KeyRef& left, const CEventInstanceDef* right) const {
                return left == *right;
            }
            bool operator()(const CEventInstanceDef* left, const CEventInstanceDef* right) const {
                return left->itemId.id == right->itemId.id && left->eventTypeId == right->eventTypeId &&
                       left->itemId.path == right->itemId.path && left->externalPath == right->externalPath;
            }
            bool operator()(const CEventInstanceDef* left, const KeyRef& right) const {
                return *left == right;
            }
        };
        typedef lib::unordered_set<int> IntSet;

        EventsArchiveRec rec;  // Последняя запись из events_recs

        /*
         * Список Id выборок, в которых было послано уведомление об активности сообщения.
         * Необходимо для посылки уведомления о его деактивизации
         */
        IntSet monitoredItemIdSet;
        vm::ItemID itemId;
        std::string eventType;
        std::string externalPath;
        int32_t archiveAlarmId;
        const int64_t eventTypeId;

        int GetInstanceId() const {
            return instanceId;
        }
        static ptr make(int id,
                        int64_t item_id,
                        int64_t event_type_id = 0,
                        const std::string& path = "",
                        const std::string& event_type = "",
                        const std::string& external_path = "") {
            return lib::make_shared<CEventInstanceDef>(id, item_id, event_type_id, path, event_type, external_path);
        }

        int64_t GetArchiveId() const {
            return archive_id;
        }

        int64_t GetLastActiveTime() const {
            return lastActiveTime;
        }

        bool InArchive(int64_t _archive_id) const {
            return _archive_id == 0 || archive_id == _archive_id;
        }
        MPLC_EVENTS_API OpcUa_StatusCode AddValue(const EventsArchiveRec& rec);

        MPLC_EVENTS_API OpcUa_StatusCode ChangeArchive(int64_t archive_id);

        MPLC_EVENTS_API void SetLastActiveTime(int64_t _lastActiveTime, bool _lastEventAcked);

        MPLC_EVENTS_API OpcUa_StatusCode AcknowledgeFiltered(const std::string& _user, const std::string& _comment);

        MPLC_EVENTS_API CEventInstanceDef(int id,
                                          int64_t item_id,
                                          int64_t event_type_id = 0,
                                          const std::string& _path = "",
                                          const std::string& event_type = "",
                                          const std::string& external_path = "");

        MPLC_EVENTS_API static CEventInstanceDef* Get(int64_t item_id,
                                                      int64_t event_type_id = 0,
                                                      const std::string& _path = "",
                                                      const std::string& event_type = "",
                                                      const std::string& external_path = "");

        MPLC_EVENTS_API OpcUa_StatusCode Acknowledge(int64_t active_time,
                                                     const std::string& _comment,
                                                     const std::string& _user,
                                                     const std::string& _source);

        MPLC_EVENTS_API std::string GetEventID(int64_t activeTime, EventsArchiveRec::update_type updtype) const;
        static std::pair<EventId, std::string> ParseEventID(const char* eventId, size_t len);
        MPLC_EVENTS_API std::string GetEventFullObjectName();
        MPLC_EVENTS_API bool IsChild(const vm::ItemID& parent) const;
        MPLC_EVENTS_API bool IsChild(const vm::ObjectID& parent) const;
        MPLC_EVENTS_API std::string GetWindowId() const;
        MPLC_EVENTS_API std::string GetEventObjectName() const;
        MPLC_EVENTS_API std::string GetEventSourceOrObjectName() const;
        MPLC_EVENTS_API std::string GetEventDescription() const;
        MPLC_EVENTS_API std::string GetEventDescriptionOrSource() const;
        MPLC_EVENTS_API std::string GetFullEventPath() const;

        KeyRef GetKeyRef();

    private:
        OpcUa_StatusCode _Acknowledge(const std::string& _comment,
                                      const std::string& _user,
                                      const std::string& _source = {});

        int64_t lastActiveTime;  // Время последнего активированного сообщения
        int64_t archive_id;
        lib::weak_ptr<IEventsArchiveProc> archive;
        lib::mutex m_mtx;
        int instanceId;       // Индекс описания, уникальный в текущем сеансе
        bool lastEventAcked;  // Последнее сообщение квитировано, предотвращает попытку повторного квитирования
        mutable lib::optional<std::string> event_full_object_name;     // Для ускорения доступа
        mutable lib::optional<std::string> event_object_name;          // Для ускорения доступа
        mutable lib::optional<std::string> event_source_or_object_name;// Для ускорения доступа
        mutable lib::optional<std::string> event_description;          // Для ускорения доступа
        mutable lib::optional<std::string> event_description_or_source;  // Для ускорения доступа
        mutable lib::optional<std::string> event_full_path;  // Для ускорения доступа
        mutable lib::optional<std::string> windowId;
        mutable const vm::VmTree::Leaf* from;
        //mutable lib::unordered_map<vm::ItemID, bool> parents;
        friend class EventsArchiveManager;
    };

    inline std::size_t event_instance_hash(const CEventInstanceDef& instance) noexcept {
        return event_instance_hash(instance.itemId.id,
                                   instance.itemId.path,
                                   instance.eventTypeId,
                                   instance.externalPath);
    }

}}  // namespace mplc::events
