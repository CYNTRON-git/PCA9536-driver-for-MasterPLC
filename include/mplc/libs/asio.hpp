#pragma once

#include <boost/asio.hpp>

namespace mplc { namespace lib {
    namespace tcp {
        using acceptor = boost::asio::ip::tcp::acceptor;
        using resolver = boost::asio::ip::tcp::resolver;
        using resolver_results = boost::asio::ip::tcp::resolver::results_type;
        using socket = boost::asio::ip::tcp::socket;
        using endpoint = boost::asio::ip::tcp::endpoint;
        static boost::asio::ip::tcp v4() {
            return boost::asio::ip::tcp::v4();
        }
    }  // namespace tcp

    namespace udp {
        using resolver = boost::asio::ip::udp::resolver;
        using resolver_results = boost::asio::ip::udp::resolver::results_type;
        using socket = boost::asio::ip::udp::socket;
        using endpoint = boost::asio::ip::udp::endpoint;
        static boost::asio::ip::udp v4() {
            return boost::asio::ip::udp::v4();
        }
    }  // namespace udp
    namespace ip {
        using boost::asio::ip::address;
        using boost::asio::ip::icmp;
        using boost::asio::ip::make_address;
    }  // namespace ip
    using boost::asio::io_context;

    namespace asio {
        using boost::asio::steady_timer;
        template<class T>
        void post(io_context& ctx, T&& token) {
            boost::asio::post(ctx, std::forward<T>(token));
        }
        template<class T>
        void dispatch(io_context& ctx, T&& token) {
            boost::asio::dispatch(ctx, std::forward<T>(token));
        }
        template<class T>
        void defer(io_context& ctx, T&& token) {
            boost::asio::defer(ctx, std::forward<T>(token));
        }
    }  // namespace asio
}}     // namespace mplc::lib
