#pragma once

namespace system_events {
    //
    enum EventTypeGroup {
        AuditEvent = 1,
        SystemAuditEvent = 2,
        SystemEvent = 3,
    };
    enum SystemAuditEvents {
        KeyErrorEvent = 1,
        ProjectIntegrityErrorEvent = 2,
        SystemIntegrityErrorEvent = 3,
        SystemUserAction = 4,
        ProjectIntegrityEvent = 5
    };
    enum SystemEvents {
        SystemErrorEvent = 1
    };
}  // namespace system_events
