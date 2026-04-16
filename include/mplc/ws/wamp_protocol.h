//#pragma once
//#include "websocket_protocol.h"
//#include "wamp_service.h"
//
//namespace mplc { namespace lwsxx {
//    struct wamp_protocol_t : websocket_protocol_t {
//        // MPLCSHARE_API int on_event(lws* wsi, int reason, void* user, void* in, size_t len) override;
//        MPLCSHARE_API int add_service(wamp_service_t* service, lib::string_view url);
//        // Server
//        MPLCSHARE_API int on_connect(lws* wsi, void* user) override;
//        MPLCSHARE_API int on_closed(lws* wsi, void* user, void* in, size_t len) override;
//        MPLCSHARE_API int on_write(lws* wsi, void* user) override;
//        MPLCSHARE_API int on_receive(lws* wsi, void* user, void* in, size_t len) override;
//        MPLCSHARE_API int on_pong(lws* wsi, void* user, void* in, size_t len) override;
//        MPLCSHARE_API int on_peer_disconnect(lws* wsi, void* user, void* in, size_t len) override;
//        // Client
//        MPLCSHARE_API int on_client_error(lws* wsi, void* user, void* in, size_t len) override;
//        MPLCSHARE_API int on_client_established(lws* wsi, void* user) override;
//        MPLCSHARE_API int on_client_closed(lws* wsi, void* user, void* in, size_t len) override;
//        MPLCSHARE_API int on_client_write(lws* wsi, void* user) override;
//        MPLCSHARE_API int on_client_receive(lws* wsi, void* user, void* in, size_t len) override;
//        MPLCSHARE_API int on_client_pong(lws* wsi, void* user, void* in, size_t len) override;
//        size_t per_session_data_size() const override {
//            return sizeof(pss_data);
//        }
//
//    protected:
//        struct pss_data {
//            websocket_session_t* session;
//            websocket_service_t* service;
//            wamp_realm_t* realm;
//        };
//
//    private:
//        wamp_service_t* find_service(const std::string& url);
//        // std::map<std::string, wamp_service_t*> m_services;
//    };
//}}  // namespace mplc::lwsxx
