#pragma once

#include <mplc/net/ssl/connection.hpp>

namespace mplc { namespace net { namespace ssl {

    template<class T>
    class client_interface {
        typedef client_interface<T> base;

    public:
        typedef ssl_connection<T> connection_t;
        typedef typename connection_t::msg_t msg_t;
        typedef typename connection_t::msg_ptr_t msg_ptr_t;
        typedef typename connection_t::ptr connection_ptr_t;

        explicit client_interface(lib::io_context& ctx_, ssl_context* ssl_ctx_ = nullptr)
            : ctx(ctx_), ssl_ctx(ssl_ctx_ ? ssl_ctx_ : new ssl_context(ssl_context::tlsv13_client)), resolver(ctx) {
            if (ssl_ctx_ == nullptr) {
                ssl_ctx->set_verify_mode(boost::asio::ssl::verify_none);
            }
        }

        virtual ~client_interface() = default;

        void connect(const std::string& host, uint16_t port) {
            resolver.async_resolve(host,
                                   mplc::to_string(port),
                                   [this](const lib::error_code& ec,
                                          const lib::tcp::resolver::results_type& endpoints) {
                                       if (!ec) {
                                           m_connection = lib::make_shared<connection_t>(ctx,
                                                                                         lib::tcp::socket(ctx),
                                                                                         *ssl_ctx);
                                           m_connection->connect_to_server(endpoints,
                                                                           lib::bind(&base::connect_result,
                                                                                     this,
                                                                                     lib::placeholders::_1));
                                       } else {
                                           connect_result(ec);
                                       }
                                   });
        }
        void connect_result(const lib::error_code& ec) {
            if (!ec) {
                m_connection->set_on_disconnect([this](const lib::error_code& ec) {
                    on_disconnect(ec);
                    m_connection.reset();
                });
                m_connection->set_on_message([this](msg_ptr_t msg) { on_message(std::move(msg)); });
                on_connect();
            } else {
                on_connect_error(ec);
            }
        }
        void disconnect() {
            lib::asio::post(ctx, [this] {
                if (m_connection)
                    m_connection->disconnect();
            });
        }

        bool is_connected() const noexcept {
            return m_connection && m_connection->is_connected();
        }

        connection_state_t get_connection_state() const {
            if (!m_connection)
                return connection_state_t::disconnected;
            return m_connection->get_connection_state();
        }

        void send(const msg_ptr_t& msg) {
            lib::asio::dispatch(ctx, [this, msg] {
                if (m_connection)
                    m_connection->send(msg);
            });
        }

    protected:
        lib::io_context& ctx;
        ssl_context* ssl_ctx;
        lib::tcp::resolver resolver;
        lib::tcp::resolver_results m_endpoints;
        connection_ptr_t m_connection;

        virtual void on_connect() {}
        virtual void on_connect_error(const lib::error_code& ec) {}
        virtual void on_disconnect(const lib::error_code& ec) {}
        virtual void on_message(msg_ptr_t&& msg) {}
    };
}}}  // namespace mplc::net::ssl
