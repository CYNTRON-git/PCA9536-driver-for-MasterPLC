#pragma once

#include <mplc/libs/tslist.hpp>
#include <mplc/net/message.hpp>
#include <mplc/libs/asio.hpp>
#include <mplc/libs/system.hpp>
#include <mplc/libs/bind.hpp>
#include <opcua/opcua.h>

namespace mplc { namespace net {

    enum class connection_state_t {
        disconnected,
        connecting,
        connected,
    };

    template<class T, class SocketT = lib::tcp::socket>
    class connection : public lib::enable_shared_from_this<connection<T, SocketT>> {
    public:
        typedef lib::shared_ptr<connection> ptr;
        typedef message<T> msg_t;
        typedef typename msg_t::ptr msg_ptr_t;

        connection(lib::io_context& ctx_, SocketT socket_): ctx(ctx_), m_socket(std::move(socket_)) {}

        connection(const connection&) = delete;
        connection& operator=(const connection&) = delete;
        connection& operator=(connection&&) = delete;
        connection(connection&&) = delete;
        virtual ~connection() = default;

        uint32_t get_id() const {
            return id;
        }

        std::string get_ip() {
            lib::error_code ec;
            return m_socket.lowest_layer().remote_endpoint(ec).address().to_string();
        }

        void connect_to_client(uint32_t uid = 0) {
            id = uid;
            connection_state == connection_state_t::connected;
            read_header();
        }

        void connect_to_server(const lib::tcp::resolver_results& endpoints,
                               const lib::function<void(const lib::error_code& ec)>& cb) {
            auto conn = this->shared_from_this();
            boost::asio::async_connect(m_socket.lowest_layer(),
                                       endpoints,
                                       [this, conn, cb = std::move(cb)](const lib::error_code& ec,
                                                                        const lib::tcp::endpoint& endpoint) {
                                           cb(ec);
                                           if (!ec) {
                                               connection_state = connection_state_t::connected;
                                               read_header();
                                           } else {
                                               connection_state = connection_state_t::disconnected;
                                           }
                                       });
        }

        void disconnect() {
            auto conn = this->shared_from_this();
            boost::asio::post(ctx, [this, conn] { _disconnect(lib::error_code()); });
        }

        bool is_connected() const {
            return m_socket.lowest_layer().is_open() && connection_state == connection_state_t::connected;
        }

        connection_state_t get_connection_state() const {
            return connection_state;
        }

        void send(const msg_ptr_t& msg) {
            if (!is_connected())
                return;
            auto conn = this->shared_from_this();
            boost::asio::post(ctx, [this, conn, msg] {
                bool need_write = m_tx_data.empty() && !tx_msg_buf;
                m_tx_data.push_back(msg);
                if (need_write) {
                    write_header();
                }
            });
        }

        void set_on_disconnect(const lib::function<void(lib::error_code)>& cb) {
            disconnect_cb = cb;
        }

        void set_on_message(const lib::function<void(msg_ptr_t)>& cb) {
            message_cb = cb;
        }

    protected:
        void read_header() {
            rx_msg_buf = msg_t::make();
            auto conn = this->shared_from_this();
            boost::asio::async_read(m_socket,
                                    boost::asio::buffer(&rx_msg_buf->header, sizeof(rx_msg_buf->header)),
                                    [this, conn](const lib::error_code& ec, size_t len) {
                                        if (!ec) {
                                            if (rx_msg_buf->header.size > 0) {
                                                rx_msg_buf->body.resize(rx_msg_buf->header.size);
                                                read_body();
                                            } else {
                                                add_incoming_message();
                                            }
                                        } else {
                                            _disconnect(ec);
                                        }
                                    });
        }

        void add_incoming_message() {
            if (message_cb) {
                message_cb(std::move(rx_msg_buf));
            }
            read_header();
        }

        void read_body() {
            auto conn = this->shared_from_this();
            boost::asio::async_read(m_socket,
                                    boost::asio::buffer(rx_msg_buf->body.data(), rx_msg_buf->body.size()),
                                    [this, conn](lib::error_code ec, size_t length) {
                                        if (!ec) {
                                            add_incoming_message();
                                        } else {
                                            _disconnect(ec);
                                        }
                                    });
        }

        void write_header() {
            m_tx_data.pop_front(tx_msg_buf);
            auto conn = this->shared_from_this();
            boost::asio::async_write(m_socket,
                                     boost::asio::buffer(&tx_msg_buf->header, sizeof(tx_msg_buf->header)),
                                     [this, conn](const lib::error_code& ec, size_t length) {
                                         if (!ec) {
                                             if (tx_msg_buf->body.size() > 0) {
                                                 write_body();
                                             } else {
                                                 tx_msg_buf.reset();
                                                 if (!m_tx_data.empty()) {
                                                     write_header();
                                                 }
                                             }
                                         } else {
                                             _disconnect(ec);
                                         }
                                     });
        }

        void write_body() {
            auto conn = this->shared_from_this();
            boost::asio::async_write(m_socket,
                                     boost::asio::buffer(tx_msg_buf->body.data(), tx_msg_buf->body.size()),
                                     [this, conn](lib::error_code ec, size_t length) {
                                         if (!ec) {
                                             tx_msg_buf.reset();
                                             if (!m_tx_data.empty()) {
                                                 write_header();
                                             }
                                         } else {
                                             _disconnect(ec);
                                         }
                                     });
        }

        void _disconnect(const lib::error_code& ec) {
            if (connection_state == connection_state_t::disconnected)
                return;
            connection_state = connection_state_t::disconnected;
            lib::error_code close_ec;
            m_socket.lowest_layer().shutdown(m_socket.lowest_layer().shutdown_both, close_ec);
            if (close_ec) {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Connection socket shutdown error %s", close_ec.message().c_str());
            }
            close_ec.clear();
            m_socket.lowest_layer().close(close_ec);
            if (close_ec) {
                OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Connection socket close error %s", close_ec.message().c_str());
            }
            if (disconnect_cb) {
                disconnect_cb(ec);
            }
        }

        lib::io_context& ctx;
        SocketT m_socket;
        uint32_t id = 0;

    private:
        lib::ts_deque<msg_ptr_t> m_tx_data;
        msg_ptr_t rx_msg_buf;
        msg_ptr_t tx_msg_buf;
        lib::function<void(lib::error_code)> disconnect_cb;
        lib::function<void(msg_ptr_t)> message_cb;

    protected:
        connection_state_t connection_state = connection_state_t::connecting;
    };
}}  // namespace mplc::net
