#pragma once
#include "share/config.h"
#include "dependency.h"
#include "wamp.h"
#include "websocket_client.h"

namespace mplc { namespace ws {
    // struct wamp_client_t : wamp_session_t {
    //     // MPLCSHARE_API wamp_client_t(const std::string& url);
    //     // lib::string_view protocol, addr, path;
    //     // uint16_t port;
    // private:
    //     std::string _m_url;
    // };

    struct wamp_client_t : websocket_client_t {
        ~wamp_client_t() override = default;
        MPLCSHARE_API virtual int on_method(wamp::code::Type method, const msgpack::object_array& args);
        MPLCSHARE_API int on_message(message_ptr&& msg) override;

        virtual int on_custom_method(int code, const msgpack::object_array& args) {
            return 1;
        }
        virtual int on_unknown(const msgpack::object& obj) {
            return 1;
        }
        virtual int on_hello(lib::string_view uri, const wamp::t_options_ref& dict) {
            return 1;
        }
        virtual int on_challenge(lib::string_view auth_metod, const wamp::t_options_ref& dict) {
            return 0;
        }
        virtual int on_authenticate(lib::string_view sign, const wamp::t_options_ref& dict) {
            return 0;
        }
        virtual int on_welcome(wamp::t_session_id id, const wamp::t_options_ref& opts) {
            return 1;
        }
        virtual int on_abort(const wamp::t_options_ref& dict, lib::string_view uri) {
            return 1;
        }
        virtual int on_goodbye(const wamp::t_options_ref& dict, lib::string_view uri) {
            return 1;
        }
        virtual int on_error(wamp::code::Type msg_type,
                             wamp::t_request_id req_id,
                             const wamp::t_details_ref& details,
                             lib::string_view uri) {
            return 0;
        }
        virtual int on_publish(wamp::t_request_id req_id,
                               const wamp::t_details_ref& details,
                               lib::string_view uri,
                               const wamp::t_args_ref& args,
                               const wamp::t_kw_args_ref& args_kv) {
            return 0;
        }
        virtual int on_published(wamp::t_request_id req_id, wamp::t_publication_id pub_id) {
            return 0;
        }
        virtual int on_subscribe(wamp::t_request_id req_id, const wamp::t_details_ref& details, lib::string_view uri) {
            return 0;
        }
        virtual int on_subscribed(wamp::t_request_id req_id, wamp::t_subscription_id sub_id) {
            return 0;
        }
        virtual int on_unsubscribe(wamp::t_request_id req_id, wamp::t_subscription_id sub_id) {
            return 0;
        }
        virtual int on_unsubscribed(wamp::t_request_id req_id) {
            return 0;
        }
        virtual int on_event(wamp::t_subscription_id sub_id,
                             wamp::t_publication_id pub_id,
                             const wamp::t_details_ref& details,
                             const wamp::t_args_ref& args,
                             const wamp::t_kw_args_ref& args_kv) {
            return 0;
        }
        virtual int on_call(wamp::t_request_id req_id,
                            const wamp::t_options_ref& opts,
                            lib::string_view proc,
                            const wamp::t_args_ref& args,
                            const wamp::t_kw_args_ref& args_kv) {
            return 0;
        }
        virtual int on_result(wamp::t_request_id req_id,
                              const wamp::t_details_ref& details,
                              const wamp::t_args_ref& args,
                              const wamp::t_kw_args_ref& args_kv) {
            return 0;
        }
        virtual int on_register(wamp::t_request_id req_id, const wamp::t_options_ref& opts, lib::string_view proc) {
            return 0;
        }
        virtual int on_registered(wamp::t_request_id req_id, wamp::t_registration_id reg_id) {
            return 0;
        }
        virtual int on_unregister(wamp::t_request_id req_id, wamp::t_registration_id reg_id) {
            return 0;
        }
        virtual int on_unregistered(wamp::t_request_id req_id) {
            return 0;
        }
        virtual int on_invocation(wamp::t_request_id req_id,
                                  wamp::t_registration_id reg_id,
                                  const wamp::t_details_ref& details,
                                  const wamp::t_args_ref& args,
                                  const wamp::t_kw_args_ref& args_kv) {
            return 0;
        }
        virtual int on_yield(wamp::t_request_id req_id,
                             const wamp::t_options_ref& opts,
                             const wamp::t_args_ref& args,
                             const wamp::t_kw_args_ref& args_kv) {
            return 0;
        }
        virtual int on_cancel(wamp::t_request_id req_id, const wamp::t_options_ref& opts) {
            return 0;
        }
        virtual int on_interrupt(wamp::t_request_id req_id, const wamp::t_options_ref& opts) {
            return 0;
        }
        virtual int on_open_stream() {
            return 0;
        }
        virtual int on_opened_stream(wamp::t_request_id req_id, wamp::t_stream_id stream_id) {
            return 0;
        }
        virtual int on_stream_data() {
            return 0;
        }
        virtual int on_close_stream() {
            return 0;
        }
        virtual int on_closed_stream(wamp::t_stream_id stream_id) {
            return 0;
        }
        virtual int on_acknowledge() {
            return 0;
        }
        virtual int on_event_acknowledge(wamp::t_subscription_id sub_id, wamp::t_publication_id pub_id) {
            return 0;
        }
        MPLCSHARE_API virtual void unknown(const msgpack::object& obj);
        MPLCSHARE_API virtual void hello(lib::string_view uri, const wamp::t_options& dict = {});
        MPLCSHARE_API virtual void challenge(lib::string_view auth_metod, const wamp::t_options& dict = {});
        MPLCSHARE_API virtual void authenticate(lib::string_view sign, const wamp::t_options& dict = {});
        MPLCSHARE_API virtual void welcome(wamp::t_session_id id, const wamp::t_options& opts = {});
        MPLCSHARE_API virtual void abort(const wamp::t_options& dict, lib::string_view uri);
        MPLCSHARE_API virtual void goodbye(const wamp::t_options& dict, lib::string_view uri);
        MPLCSHARE_API virtual void error(wamp::code::Type msg_type,
                                         wamp::t_request_id req_id,
                                         const wamp::t_details& details,
                                         lib::string_view uri);
        MPLCSHARE_API virtual void publish(wamp::t_request_id req_id,
                                           const wamp::t_details& details,
                                           lib::string_view uri,
                                           const wamp::t_args& args = {},
                                           const wamp::t_kw_args& args_kv = {});
        MPLCSHARE_API virtual void published(wamp::t_request_id req_id, wamp::t_publication_id pub_id);
        MPLCSHARE_API virtual void subscribe(wamp::t_request_id req_id,
                                             const wamp::t_details& details,
                                             lib::string_view uri);
        MPLCSHARE_API virtual void subscribed(wamp::t_request_id req_id, wamp::t_subscription_id sub_id);
        MPLCSHARE_API virtual void unsubscribe(wamp::t_request_id req_id, wamp::t_subscription_id sub_id);
        MPLCSHARE_API virtual void unsubscribed(wamp::t_request_id req_id);
        MPLCSHARE_API virtual void event(wamp::t_subscription_id sub_id,
                                         wamp::t_publication_id pub_id,
                                         const wamp::t_details& details = {},
                                         const wamp::t_args& args = {},
                                         const wamp::t_kw_args& args_kv = {});
        MPLCSHARE_API virtual void call(wamp::t_request_id req_id,
                                        const wamp::t_options& opts,
                                        lib::string_view proc,
                                        const wamp::t_args& args = {},
                                        const wamp::t_kw_args& args_kv = {});
        MPLCSHARE_API virtual void result(wamp::t_request_id req_id,
                                          const wamp::t_details& details,
                                          const wamp::t_args& args,
                                          const wamp::t_kw_args& args_kv = {});
        MPLCSHARE_API virtual void register_(wamp::t_request_id req_id,
                                             const wamp::t_options& opts,
                                             lib::string_view proc);
        MPLCSHARE_API virtual void registered(wamp::t_request_id req_id, wamp::t_registration_id reg_id);
        MPLCSHARE_API virtual void unregister(wamp::t_request_id req_id, wamp::t_registration_id reg_id);
        MPLCSHARE_API virtual void unregistered(wamp::t_request_id req_id);
        MPLCSHARE_API virtual void invocation(wamp::t_request_id req_id,
                                              wamp::t_registration_id reg_id,
                                              const wamp::t_details& details = {},
                                              const wamp::t_args& args = {},
                                              const wamp::t_kw_args& args_kv = {});
        MPLCSHARE_API virtual void yield(wamp::t_request_id req_id,
                                         const wamp::t_options& opts = {},
                                         const wamp::t_args& args = {},
                                         const wamp::t_kw_args& args_kv = {});
        MPLCSHARE_API virtual void cancel(wamp::t_request_id req_id, const wamp::t_options& opts = {});
        MPLCSHARE_API virtual void interrupt(wamp::t_request_id req_id, const wamp::t_options& opts = {});
        MPLCSHARE_API virtual void event_acknowledge(wamp::t_subscription_id sub_id, wamp::t_publication_id pub_id);
        MPLCSHARE_API virtual void write(wamp::msgpack_out_ptr&& msg);

        MPLCSHARE_API int64_t get_req_id() {
            return wamp::valid_id(m_req_id++);
        }

    protected:
        friend struct wamp_service_t;
        int64_t m_req_id{1};
        template<class... Args>
        int _meta_call(int (wamp_client_t::*Call)(Args...), const msgpack::object_array& arr) {
            return wamp::call_wamp_func(this, Call, arr, [this](const msgpack::object_array& arr) {
                abort({}, "wamp.invalid_args_count");
                return 1;
            });
        }

        template<class... Args>
        void _write(int /* wamp::code::Type */ type, Args&&... args) {
            write(wamp::msgpack_out_t::make(type, args...));
        }
    };
}}  // namespace mplc::lwsxx
