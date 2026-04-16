#pragma once
#include <mplc/deprecated_wsapi.h>
#include <share/mplcshare.h>

//#include "data_archive_manager.h"
//#include "request_processor.h"

namespace mplc {
    class CustomServer;
    namespace deprecated_ws {
        class WsServer;
        class WebSocketAddin : public AddinBase {
            // archive::DataArchiveManager *archive_manager;
            WsServer* server;
            CustomServer* tcp_server;

        public:
            OpcUa_StatusCode Init(ProcessRequestCallback func);
            virtual OpcUa_StatusCode OnConfigEvent(ConfigProcessorMode mode, ControllerConfig* config);
            MPLC_WS_API static WebSocketAddin& GetInstance();
            WebSocketAddin();
            virtual ~WebSocketAddin();
        };
    }  // namespace ws
}  // namespace mplc
