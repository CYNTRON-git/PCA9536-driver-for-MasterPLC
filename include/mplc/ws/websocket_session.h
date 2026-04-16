#pragma once
#include <mplc/libs/tslist.hpp>
#include <mplc/libs/string_view.hpp>
#include <mplc/time_point.h>
#include "dependency.h"
#include "session.h"
#pragma warning(push)
#pragma warning(disable : 4275)
namespace mplc { namespace ws {

    MPLCSHARE_API size_t remaining_packet_payload(lws* wsi);
    MPLCSHARE_API bool frame_is_binary(lws* wsi);
    MPLCSHARE_API bool is_first_fragment(lws* wsi);
    MPLCSHARE_API bool is_final_fragment(lws* wsi);
    MPLCSHARE_API std::string get_peer_ip(lws* wsi);

    enum class msg_type {
        WS_BIN,
        WS_TEXT,
        WS_PING,
        WS_ERROR,
        WS_CLOSE,
    };
    // enum class ws_opcode {
    //     NONE = 0,
    //     BIN = 1,
    //     TEXT = 1 << 1,
    //     PING = 1 << 1,
    //     PONG = 1 << 1,
    //     FIRST = 1 << 1,
    //     LAST = 1 << 1,
    // };
    struct message_t {
        message_t(msg_type type_ = msg_type::WS_BIN, size_t remaining_len = 0) {
            type = type_;
            m_data.reserve(remaining_len);
        }
        MPLCSHARE_API void receive(lib::string_view data, bool is_bin);
        MPLCSHARE_API void append(const void* in, size_t len);
        MPLCSHARE_API void append(lib::string_view data);
        MPLCSHARE_API virtual void* data();
        std::string& raw() {
            return m_data;
        }
        virtual size_t size() const {
            return m_data.size();
        }
        virtual ~message_t() {}
        msg_type type{msg_type::WS_BIN};

    protected:
        std::string m_data;
    };
    using message_ptr = lib::unique_ptr<message_t>;
    struct msg_out_t : message_t {
        MPLCSHARE_API msg_out_t(msg_type type_ = msg_type::WS_BIN, size_t remaining_len = 0);
        MPLCSHARE_API msg_out_t(message_ptr&& msg);
        // return true if all data sent
        MPLCSHARE_API bool is_sent() const;
        // MPLCSHARE_API int write(lws* wsi, size_t tx_size);
        MPLCSHARE_API void* data() override;
        MPLCSHARE_API size_t size() const override;
        void consumed(size_t size) {
            m_pos += size;
        }
        size_t pos() const {
            return m_pos;
        }
        MPLCSHARE_API static const int WS_HDR;
        size_t m_pos{0};
        static lib::unique_ptr<msg_out_t> make(msg_type type, lib::string_view data = {}) {
            auto msg = lib::make_unique<msg_out_t>(type, data.size());
            msg->append(data.data(), data.size());
            return msg;
        }
        static lib::unique_ptr<msg_out_t> from(const message_ptr& in_msg) {
            auto msg = lib::make_unique<msg_out_t>(in_msg->type, in_msg->size());
            msg->append(in_msg->raw());
            return msg;
        }
    };
    using msg_out_ptr = lib::unique_ptr<msg_out_t>;
    struct MPLCSHARE_API session_stat_t {
        int64_t rx_bytes{};
        int64_t rx_packs{};
        int64_t rx_msg{};
        int64_t tx_bytes{};
        int64_t tx_packs{};
        int64_t tx_msg{};
        int64_t tx_queue{};
        int64_t on_write_count{};
        TimeSpan on_write_delay{};
        TimeSpan on_write_time{};
        session_stat_t operator+=(const session_stat_t& v);
        void print(int period, const char* name) const;
    };

    struct websocket_session_t : session_t {
        MPLCSHARE_API virtual int on_write(session_stat_t* stats = nullptr);
        // MPLCSHARE_API virtual int on_closed(const char* in, size_t len);
        //  MPLCSHARE_API int on_established(lws* _wsi) override;

        MPLCSHARE_API websocket_session_t(lws wsi): session_t(wsi) {}
        MPLCSHARE_API virtual void ping(lib::string_view msg = {});
        MPLCSHARE_API virtual void emplace_back(msg_type type, lib::string_view msg = {});
        MPLCSHARE_API virtual void emplace_back(msg_out_ptr&& msg);
        MPLCSHARE_API void callback_on_writable() override;
        MPLCSHARE_API virtual void close(lib::string_view reason = {});
        MPLCSHARE_API lib::ts_deque<msg_out_ptr>& tx_queue();
        // MPLCSHARE_API void clear();
    private:
        lib::ts_deque<msg_out_ptr> m_tx_queue;
        lib::atomic_flag call_on_write;
        msg_out_ptr tx_buf;
        TimePoint wait_on_write;
    };
    using websocket_session_ptr = lib::shared_ptr<websocket_session_t>;

}}  // namespace mplc::ws
#pragma warning(pop)
