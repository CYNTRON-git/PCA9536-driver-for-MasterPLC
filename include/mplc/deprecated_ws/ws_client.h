#pragma once
#include <libwebsockets.h>
#include <string>
#include <mplc/deprecated_ws/ws_connect.h>
#include <mplc/libs/bind.hpp>
#include "wamp.h"

namespace mplc { namespace deprecated_ws {
    struct VHostStorage {
        struct lws_context* context;
        struct lws_vhost* vhost;
        lws_sorted_usec_list_t sul;
    };
    typedef lib::function<void(struct lws*, void**)> on_connected_cb;
    typedef lib::function<void(void*)> on_error_cb;
    typedef lib::function<void(void*)> on_disconnected_cb;
    struct wsi_interface {
        void* arg;
        wsi_interface(): arg(nullptr) {}

        on_connected_cb on_connected;
        on_error_cb on_error;
        on_disconnected_cb on_disconnected;
    };
    class WsClient : public WsConnect {
    public:
        // enum State { NIL, CLIENT_IDLE = 1, CLIENT_CONNECTING };

        WsClient();
        ~WsClient() override;
        // void Send(const char* data);
        void Send(const pWsData& msg) {
            push_tx(msg);
            lws_callback_on_writable(m_wsi);
        }
        // bool Connect(lws_context* context);
        static bool Connect(lws_context* context,
                            const std::string& ip,
                            uint16_t port,
                            const std::string& url,
                            const std::string& protocol,
                            const on_connected_cb& on_connected,
                            const on_error_cb& on_error,
                            const on_disconnected_cb& on_disconnected);
        // int OnReceive(void* in, size_t len, bool is_final, bool is_binary);
        int OnEstablished(lws* wsi);
        int OnClosed(lws_context* context);
        // int OnWrite();
        // State GetState() const { return m_state; }
        // void Init(const std::string& ip, uint16_t port, const std::string& url, const std::string& protocol);

    private:
        struct lws* m_wsi;
        // int index;
        ////State m_state;
        // std::string m_ip;
        // uint16_t m_port;
        // std::string m_url;
        // std::string m_protocol;
    };
    // struct subscribe_request {
    //    std::string uri;
    //    wamp::on_subscribed_fn subscribed_cb;
    //    wamp::on_event_fn event_cb;
    //    void* user;
    //};
    // struct subscription {
    //    wamp::on_event_fn event_cb;
    //    void* user;
    //};

}}  // namespace mplc::ws
