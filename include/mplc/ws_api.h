#pragma once
#include <share/config.h>
#include "ws/context.h"
#include "ws/http_protocol.h"
#include "ws/protocol.h"
#include "ws/session.h"
#include "ws/vhost.h"
#include "ws/websocket_protocol.h"
#include "ws/websocket_session.h"
#include "ws/websocket_client.h"
#include "ws/wamp_protocol.h"
#include "ws/wamp_service.h"
#include "ws/wamp_client.h"
#include "ws/wamp_realm.h"

namespace mplc { namespace ws {
    MPLCSHARE_API void libwscpp_init();
    MPLCSHARE_API const char* get_reason_name(int reason);
    MPLCSHARE_API void read_custom_headers(const char* name, int nlen, void* opaque);
    MPLCSHARE_API std::string read_header(lws* wsi);
    MPLCSHARE_API std::string read_header(lws* wsi, int header_index);
}}  // namespace mplc::lwsxx
