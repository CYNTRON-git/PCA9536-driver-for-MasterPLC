#pragma once
#include <libwebsockets.h>
#include <string>
#include <mplc/deprecated_ws/ws_connect.h>

#include <mplc/deprecated_ws/wamp.h>
#include <mplc/deprecated_ws/ws_client.h>

namespace mplc { namespace deprecated_ws {

    class WampClient : public WsClient {
    public:
        WampClient() { m_next_request_id = 1; }
        ~WampClient() override {}
        wamp::t_request_id NextRequestId() { return m_next_request_id++; }
        int OnReceive(void* in, size_t len) override;
        void SendError(wamp::code::Type msg_type,
                       wamp::t_request_id req_id,
                       const wamp::t_options& opts,
                       const std::string& topic);

        void OpenStream(wamp::t_request_id req_id, const wamp::t_options& opts, const std::string& topic);
        virtual void OnOpenedStream(wamp::t_request_id req_id, wamp::t_stream_id stream_id) {}
        virtual void OnStreamData(wamp::t_stream_id stream_id,
                                  wamp::t_stream_pack_id pack_id,
                                  bool ack,
                                  bool fin,
                                  const wamp::t_options_ref& opts,
                                  const char* in,
                                  size_t len) {}

        virtual void OnEvent(wamp::t_stream_id stream_id,
                             wamp::t_publication_id pub_id,
                             const wamp::t_options_ref& opts) {}

        wamp::t_request_id Subscribe(const wamp::t_options& opts, const std::string& topic);

        wamp::t_request_id CloseStream(wamp::t_stream_id stream_id);
        virtual void OnClosedStream(wamp::t_stream_id stream_id) {}
        virtual void OnError(wamp::code::Type msg_type,
                             wamp::t_request_id req_id,
                             const wamp::t_options_ref& opts,
                             const std::string& err) {}
        void Error(wamp::code::Type msg_type,
                   wamp::t_request_id req_id,
                   const wamp::t_options& opts,
                   const std::string& err);
        virtual void OnCloseStream(wamp::t_request_id req_id, wamp::t_stream_id stream_id) {}
        virtual void OnSubscribed(wamp::t_request_id req_id, wamp::t_subscription_id sub_id) {}
        virtual void OnMessage(const pWsData& msg);
        lib::atomic<wamp::t_request_id> m_next_request_id;
    };
}}  // namespace mplc::ws
