#pragma once

#include "share/mplcshare.h"
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "boost_filesystem.lib")

#include <boost/asio/ssl.hpp>
#include <mplc/net/connection.hpp>

namespace mplc { namespace net { namespace ssl {

    using ssl_context = boost::asio::ssl::context;
    using ssl_socket_t = boost::asio::ssl::stream<lib::tcp::socket>;

    template<class T>
    class ssl_connection : public connection<T, ssl_socket_t> {
    public:
        typedef lib::shared_ptr<ssl_connection> ptr;

        ssl_connection(lib::io_context& ctx_, lib::tcp::socket socket_, ssl_context& ssl_ctx_)
            : connection<T, ssl_socket_t>(ctx_, ssl_socket_t(std::move(socket_), ssl_ctx_)) {}

        void connect_to_client(uint32_t uid = 0) {
            type = ssl_socket_t::server;
            this->id = uid;
            handshake();
        }

        void connect_to_server(const lib::tcp::resolver_results& endpoints,
                               const lib::function<void(const lib::error_code& ec)>& cb) {
            type = ssl_socket_t::client;
            auto conn = this->shared_from_this();
            boost::asio::async_connect(this->m_socket.lowest_layer(),
                                       endpoints,
                                       [this, conn, cb = std::move(cb)](const lib::error_code& ec,
                                                                        const lib::tcp::endpoint& endpoint) {
                                           cb(ec);
                                           if (!ec) {
                                               handshake();
                                           } else {
                                               this->connection_state = connection_state_t::disconnected;
                                           }
                                       });
        }

    private:
        void handshake() {
            auto conn = this->shared_from_this();
            this->m_socket.async_handshake(type, [this, conn](const lib::error_code& ec) { on_handshake(ec); });
        }

        void on_handshake(const lib::error_code& ec) {
            if (!ec) {
                this->connection_state = connection_state_t::connected;
                this->read_header();
            } else
                this->_disconnect(ec);
        }

        ssl_socket_t::handshake_type type;
    };
}}}  // namespace mplc::net::ssl
