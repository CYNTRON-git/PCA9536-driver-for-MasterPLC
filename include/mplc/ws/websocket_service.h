#pragma once
#include "websocket_session.h"

namespace mplc { namespace ws {
    using connection_ptr = websocket_session_ptr;
    using connection_hndl = lib::weak_ptr<websocket_session_t>;

    struct MPLCSHARE_API websocket_service_t {
        virtual ~websocket_service_t();
        websocket_service_t();
        // lws api
        virtual int on_connect(lws* wsi, pss_node_ptr& user);
        virtual int on_receive(lws* wsi, pss_node_ptr& user, void* in, size_t len);
        virtual int on_closed(lws* wsi, pss_node_ptr& user, void* in, size_t len);
        virtual int on_write(lws* wsi, pss_node_ptr& user);
        virtual int on_pong(lws* wsi, pss_node_ptr& user, void* in, size_t len);
        virtual int on_peer_disconnect(lws* wsi, pss_node_ptr& user, void* in, size_t len);
        virtual int filter_connection(lws* wsi) {
            return 0;
        }
        virtual void on_start() {}
        virtual void on_stop() {}
        virtual void on_wait() {}
        virtual int on_connect(lws* wsi, connection_ptr& con, pss_node_ptr& user) {
            con = ws::make_session<websocket_session_t>(wsi);
            return 0;
        }
        // Return count messages in queue
        int64_t rx_size() const;
        virtual int on_receive(connection_hndl hndl, pss_node_ptr& user, message_ptr&& msg);

        virtual int on_write(connection_ptr conn, pss_node_ptr& user) {
            return conn->on_write();
        }
        virtual int on_pong(connection_ptr conn, pss_node_ptr& user, lib::string_view data) {
            return 0;
        }
        virtual int on_closed(connection_ptr conn, pss_node_ptr& user, lib::string_view reason) {
            return 0;
        }
        virtual int on_peer_disconnect(connection_ptr conn, pss_node_ptr& user) {
            return 0;
        }
        virtual int on_message(connection_ptr conn, message_ptr&& msg, const pss_node_ptr& user) {
            return 0;
        }
        virtual void write(connection_hndl hndl, msg_out_ptr&& msg);
        virtual void write(connection_hndl hndl, msg_type type, lib::string_view data = {}) {
            write(hndl, msg_out_t::make(type, data));
        }

        // ws service api
        void run();
        int run_one();
        int poll();
        int poll_one();
        void stop();
        void restart();
        bool is_stop() const;

    protected:
        struct impl;
        impl* pimpl;
    };
}}  // namespace mplc::ws
