#pragma once
#include <mplc/libs/string_view.hpp>
#include "websocket_session.h"
#pragma warning(push)
#pragma warning(disable : 4275)
namespace mplc { namespace ws {
    struct context_t;
    struct client_connect_info {
        uint16_t port;
        std::string address;
        std::string path;
        std::string host;
        std::string origin;
        std::string protocol;
        std::string local_protocol_name;
        int ssl_opt;
        lws_vhost* vhost;
        void* userdata;
    };
    struct client_interface : lib::enable_shared_from_this<client_interface> {
        virtual ~client_interface() = default;
        virtual int on_established(lws* _wsi, pss_node_ptr& user) = 0;
        MPLCSHARE_API virtual void configure(client_connect_info& info);
        virtual std::string get_vhost() const = 0;
        virtual std::string get_addr() const = 0;
        virtual std::string get_path() const = 0;
        virtual uint16_t get_port() const = 0;
        virtual bool is_ssl() const = 0;
        // maybe return nullptr for default
        virtual std::string get_local_protocol() const {
            return "";
        }
        // maybe return nullptr for default
        virtual std::string get_protocol() const {
            return "";
        }
        virtual void on_connect_error(lib::string_view error) = 0;
        virtual int on_closed(lws* wsi, lib::string_view error, pss_node_ptr& user) = 0;
        virtual int on_write(lws* wsi, pss_node_ptr& user) = 0;
        virtual int on_receive(lws* wsi, lib::string_view data, bool is_bin, bool is_final, pss_node_ptr& user) = 0;
        virtual int on_error(lws* wsi, lib::string_view error, pss_node_ptr& user) = 0;
        virtual int on_pong(lws* wsi, lib::string_view error, pss_node_ptr& user) {
            return 0;
        }
        virtual int on_add_headers(lws* wsi, void* in, size_t len, pss_node_ptr& user) {
            return 0;
        }

        virtual int on_session_destroy(pss_node_ptr& user) = 0;
        // websocket_session_t& session();
    };

    struct MPLCSHARE_API websocket_client_t : client_interface {
        websocket_client_t() noexcept;
        ~websocket_client_t() noexcept override;
        websocket_client_t(lib::string_view vhost_name, lib::string_view url);

        websocket_client_t(lib::string_view vhost_name,
                           lib::string_view addr,
                           lib::string_view path,
                           uint16_t port,
                           bool ssl = false,
                           lib::string_view sub_protocol = {},
                           lib::string_view vhost_protocol = {});
        void init(lib::string_view vhost_name,
                  lib::string_view url,
                  bool use_ssl = false,
                  lib::string_view vhost_protocol = {});

        void init(lib::string_view vhost_name,
                  lib::string_view addr,
                  lib::string_view path,
                  uint16_t port,
                  bool ssl = false,
                  lib::string_view sub_protocol = {},
                  lib::string_view vhost_protocol = {});

        virtual void connect(context_t& ctx);
        virtual void reconnect();
        virtual void disconnect();
        virtual bool is_connected() const;

        void set_vhost(lib::string_view val);
        void set_protocol(lib::string_view val);
        void set_addr(lib::string_view val);
        void set_local_protocol(lib::string_view val);
        void set_path(lib::string_view val);
        void set_port(uint16_t val);
        void use_ssl(bool val);

        std::string get_vhost() const override;
        std::string get_protocol() const override;
        std::string get_addr() const override;
        std::string get_path() const override;
        std::string get_local_protocol() const override;
        uint16_t get_port() const override;
        bool is_ssl() const override;

        void on_connect_error(lib::string_view error) override;
        int on_established(lws* wsi, pss_node_ptr& user) override;
        int on_closed(lws* wsi, lib::string_view error, pss_node_ptr& user) override;
        int on_write(lws* wsi, pss_node_ptr& user) override;
        int on_receive(lws* wsi, lib::string_view data, bool is_bin, bool is_final, pss_node_ptr& user) override;
        int on_error(lws* wsi, lib::string_view error, pss_node_ptr& user) override;
        int on_pong(lws* wsi, lib::string_view error, pss_node_ptr& user) override;
        int on_add_headers(lws* wsi, void* in, size_t len, pss_node_ptr& user) override;

        session_stat_t get_stat() const;
        void swap_stat(session_stat_t& stat);
        void clear_stat() {
            session_stat_t stat{};
            swap_stat(stat);
        }
        websocket_session_ptr get_session() const;

        void send_msg(msg_out_ptr&& msg);
        void send_msg(msg_type type, lib::string_view msg);
        virtual int on_message(message_ptr&& msg) {
            return 0;
        }

        int on_session_destroy(pss_node_ptr& user) override;

    protected:
        static constexpr int connect_try_without_timeout = 5;
        static constexpr int reconnect_timeout = 5;
        static constexpr int pong_timeout = 5;
        static constexpr int idle_timeout = 10;
        context_t* m_ctx{};
        struct impl;
        impl* pimpl;
    };
}}  // namespace mplc::ws
#pragma warning(pop)
