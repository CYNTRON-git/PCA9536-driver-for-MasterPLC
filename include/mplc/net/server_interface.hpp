#pragma once

#include <mplc/net/connection.hpp>
#include <limits>
#include <list>

namespace mplc { namespace net {
    template<class T>
    class server_interface {
    public:
        typedef connection<T> connection_t;
        typedef typename connection_t::msg_t msg_t;
        typedef typename connection_t::msg_ptr_t msg_ptr_t;
        typedef typename connection_t::ptr connection_ptr_t;

        server_interface(uint16_t port, const lib::ip::address& addr, lib::io_context& _ctx)
            : ctx(_ctx), m_acceptor(ctx, lib::tcp::endpoint(addr, port)) {}
        server_interface(uint16_t port, lib::io_context& _ctx)
            : ctx(_ctx), m_acceptor(ctx, lib::tcp::endpoint(lib::tcp::v4(), port)) {}
        virtual ~server_interface() = default;
        void start() {
            wait_client_connection();
        }

        void stop() {
            boost::asio::post(ctx, [this] {
                m_acceptor.close();
                for (auto&& m_connection: m_connections) {
                    m_connection->disconnect();
                }
            });
        }

        void wait_client_connection() {
            m_acceptor.async_accept([this](lib::error_code ec, lib::tcp::socket socket) {
                if (!m_acceptor.is_open()) {
                    return;
                }
                wait_client_connection();
                if (ec)
                    return;
                auto conn = lib::make_shared<connection_t>(ctx, std::move(socket));
                if (on_connect(conn)) {
                    lib::weak_ptr<connection_t> weak_con(conn);
                    conn->set_on_disconnect([this, weak_con](const lib::error_code& ec) {
                        auto conn = weak_con.lock();
                        if (conn) {
                            on_disconnect(conn, ec);
                            m_connections.remove(conn);
                        }
                    });
                    conn->set_on_message([this, weak_con](msg_ptr_t msg) { on_message(weak_con, std::move(msg)); });
                    conn->connect_to_client(n_id_counter++);
                    m_connections.emplace_back(std::move(conn));
                }
            });
        }
        void message_all_clients(const msg_ptr_t& msg, connection_ptr_t ignore = nullptr) {
            for (const auto& client: m_connections) {
                if (client != ignore) {
                    client->send(msg);
                    // message(client, msg);
                }
            }
        }

        // bool message(const connection_ptr_t& client, const msg_ptr_t& msg) {
        // if (client && client->is_open()) {
        // client->send(msg);
        // return true;
        // }
        // return false;
        // }

    protected:
        virtual bool on_connect(const connection_ptr_t& client) = 0;
        virtual void on_disconnect(const connection_ptr_t& client, const lib::error_code& ec) {}
        virtual void on_message(const lib::weak_ptr<connection_t>& weak_client, msg_ptr_t&& msg) {}

        std::list<connection_ptr_t> m_connections;
        lib::io_context& ctx;
        lib::tcp::acceptor m_acceptor;
        uint32_t n_id_counter = 10000;
    };
}}  // namespace mplc::net
