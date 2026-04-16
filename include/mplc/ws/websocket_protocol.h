#pragma once
#include "share/config.h"
#include "dependency.h"
#include "http_protocol.h"
#include "websocket_session.h"
#include "websocket_service.h"
#include <mplc/libs/unordered_map.hpp>

namespace mplc { namespace ws {
    struct client_connect_info;
    struct client_interface;
    struct websocket_client_t;
    // struct websocket_protocol_t;
    // using websocket_protocol_ptr = lib::unique_ptr<websocket_protocol_t>;
    struct websocket_protocol_t : http_protocol_t {
        static constexpr const char* name = "ws";
        // service methods
        MPLCSHARE_API virtual int filter_ws_connection(lws* wsi);
        MPLCSHARE_API virtual int add_service(lib::weak_ptr<websocket_service_t> service_hndl, lib::string_view path);
        MPLCSHARE_API virtual int on_connect(lws* wsi, void* user);
        MPLCSHARE_API virtual int on_closed(lws* wsi, void* user, void* in, size_t len);
        MPLCSHARE_API virtual int on_write(lws* wsi, void* user);
        MPLCSHARE_API virtual int on_receive(lws* wsi, void* user, void* in, size_t len);
        MPLCSHARE_API virtual int on_pong(lws* wsi, void* user, void* in, size_t len);
        MPLCSHARE_API virtual int on_peer_disconnect(lws* wsi, void* user, void* in, size_t len);
        // client methods
        MPLCSHARE_API virtual int on_client_error(lws* wsi, void* user, void* in, size_t len);
        MPLCSHARE_API virtual int on_client_established(lws* wsi, void* user);
        MPLCSHARE_API virtual int on_client_closed(lws* wsi, void* user, void* in, size_t len);
        MPLCSHARE_API virtual int on_client_write(lws* wsi, void* user);
        MPLCSHARE_API virtual int on_client_receive(lws* wsi, void* user, void* in, size_t len);
        MPLCSHARE_API virtual int on_client_pong(lws* wsi, void* user, void* in, size_t len);
        MPLCSHARE_API virtual int on_client_add_headers(lws* wsi, void* user, void* in, size_t len);
        MPLCSHARE_API virtual void client_connect(client_connect_info& info,
                                                  const lib::shared_ptr<client_interface>& client);
        MPLCSHARE_API virtual int on_wsi_destroy(lws* lws, void* user);
        // protocolo methods
        MPLCSHARE_API int on_event(lws* wsi, int reason, void* user, void* in, size_t len) override;
        MPLCSHARE_API int on_init(lws* wsi) override;
        MPLCSHARE_API int on_destroy(lws* wsi) override;
        MPLCSHARE_API ~websocket_protocol_t() override;
        MPLCSHARE_API void stop() override;
        // MPLCSHARE_API void start();
        MPLCSHARE_API virtual lib::shared_ptr<websocket_service_t> find_service(lib::string_view url);

        // int connect(lws_context* ctx);

    public:
        size_t per_session_data_size() const override {
            return sizeof(pss_node_t);
        }

        bool stopped() override {
            return m_stop;
        }
        MPLCSHARE_API void restart() override;

    protected:
        /*struct pss_data {
            websocket_service_t* service;
            void* data;
        };*/
        lib::unordered_map<std::string, lib::weak_ptr<websocket_service_t>> m_services;
        lib::mutex mtx_servie;
        // lib::atomic<bool> m_inited{};
        bool m_stop{};
    };
}}  // namespace mplc::ws
