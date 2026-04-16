#pragma once
#include "base_event_type.h"
#include "system_events.h"

namespace mplc { namespace events {

class AuditEventType : public BaseEventType {
public:
	AuditEventType(int typeId, int64_t itemId, const std::string& path = "", int severity = 100) :
		BaseEventType(itemId, path, severity, BaseEventType::MakeSystemEventTypeId(system_events::AuditEvent, typeId), "AuditEvent") {}
};

class SystemAuditEventType : public BaseEventType {
public:
	SystemAuditEventType(system_events::SystemAuditEvents typeId, int64_t itemId, const std::string& path = "", int severity = 100) :
		BaseEventType(itemId, path, severity, BaseEventType::MakeSystemEventTypeId(system_events::SystemAuditEvent, (int)typeId), "SystemAuditEvent") {}
};

class SystemEventType : public BaseEventType {
public:
	SystemEventType(system_events::SystemEvents typeId, int64_t itemId, const std::string& path = "", int severity = 100) :
		BaseEventType(itemId, path, severity, BaseEventType::MakeSystemEventTypeId(system_events::SystemEvent, (int)typeId), "SystemEvent") {}
};
}}  // namespace mplc::events
