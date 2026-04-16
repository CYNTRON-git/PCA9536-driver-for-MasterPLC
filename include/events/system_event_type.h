#pragma once
#include "base_event_type.h"

class SystemEventType : public BaseEventType {


public:
	MPLC_EVENTS_API SystemEventType(int eventTypeId, int severity = 700, int64_t itemId = 0, std::string path = std::string()) : 
		BaseEventType(itemId, path, severity, eventTypeId, "SystemEvent")
	{

	}
};
