#pragma once
#include "wamp.h"

namespace mplc { namespace ws {
    using connection_ptr = lib::shared_ptr<websocket_session_t>;
    using connection_hndl = lib::weak_ptr<websocket_session_t>;
    /* struct wamp_realm_t;
     using wamp_realm_ptr = lib::shared_ptr<wamp_realm_t>;
     using wamp_realm_hndl = lib::weak_ptr<wamp_realm_t>;*/
    struct MPLCSHARE_API wamp_realm_t {
        virtual ~wamp_realm_t();
        wamp_realm_t();
        virtual int on_message(const connection_ptr& con,
                               pss_node_ptr& user,
                               wamp::code::Type type,
                               const msgpack::object_array& args);
        virtual int on_custom_method(int code, const connection_ptr& con, const msgpack::object_array& args);

        virtual int on_unknown(const connection_ptr& con, const msgpack::object& obj);

        virtual int on_hello(const connection_ptr& con, lib::string_view uri, const wamp::t_options_ref& opts);

        virtual int on_welcome(const connection_ptr& con, wamp::t_session_id id, const wamp::t_options_ref& opts);

        virtual int on_abort(const connection_ptr& con, const wamp::t_options_ref& dict, lib::string_view uri);

        virtual int on_goodbye(const connection_ptr& con, const wamp::t_options_ref& dict, lib::string_view uri);

        virtual int on_error(const connection_ptr& con,
                             wamp::code::Type msg_type,
                             wamp::t_request_id req_id,
                             const wamp::t_details_ref& details,
                             lib::string_view uri);

        virtual int on_publish(const connection_ptr& con,
                               wamp::t_request_id req_id,
                               const wamp::t_details_ref& details,
                               lib::string_view uri,
                               const wamp::t_args_ref& args,
                               const wamp::t_kw_args_ref& args_kv);

        virtual int on_published(const connection_ptr& con, wamp::t_request_id req_id, wamp::t_publication_id pub_id);

        virtual int on_subscribe(const connection_ptr& con,
                                 wamp::t_request_id req_id,
                                 const wamp::t_details_ref& details,
                                 lib::string_view uri);

        virtual int on_subscribed(const connection_ptr& con, wamp::t_request_id req_id, wamp::t_subscription_id sub_id);

        virtual int on_unsubscribe(const connection_ptr& con,
                                   wamp::t_request_id req_id,
                                   wamp::t_subscription_id sub_id);

        virtual int on_unsubscribed(const connection_ptr& con, wamp::t_request_id req_id);

        virtual int on_event(const connection_ptr& con,
                             wamp::t_subscription_id sub_id,
                             wamp::t_publication_id pub_id,
                             const wamp::t_details_ref& details,
                             const wamp::t_args_ref& args,
                             const wamp::t_kw_args_ref& args_kv);

        virtual int on_call(const connection_ptr& con,
                            wamp::t_request_id req_id,
                            const wamp::t_options_ref& opts,
                            lib::string_view proc,
                            const wamp::t_args_ref& args,
                            const wamp::t_kw_args_ref& args_kv);

        virtual int on_result(const connection_ptr& con,
                              wamp::t_request_id req_id,
                              const wamp::t_details_ref& details,
                              const wamp::t_args_ref& args,
                              const wamp::t_kw_args_ref& args_kv);

        virtual int on_register(const connection_ptr& con,
                                wamp::t_request_id req_id,
                                const wamp::t_options_ref& opts,
                                lib::string_view proc);

        virtual int on_registered(const connection_ptr& con, wamp::t_request_id req_id, wamp::t_registration_id reg_id);

        virtual int on_unregister(const connection_ptr& con, wamp::t_request_id req_id, wamp::t_registration_id reg_id);

        virtual int on_unregistered(const connection_ptr& con, wamp::t_request_id req_id);

        virtual int on_invocation(const connection_ptr& con,
                                  wamp::t_request_id req_id,
                                  wamp::t_registration_id reg_id,
                                  const wamp::t_details_ref& details,
                                  const wamp::t_args_ref& args,
                                  const wamp::t_kw_args_ref& args_kv);

        virtual int on_yield(const connection_ptr& con,
                             wamp::t_request_id req_id,
                             const wamp::t_options_ref& opts,
                             const wamp::t_args_ref& args,
                             const wamp::t_kw_args_ref& args_kv);

        virtual int on_challenge(const connection_ptr& con,
                                 lib::string_view auth_metod,
                                 const wamp::t_options_ref& dict);

        virtual int on_authenticate(const connection_ptr& con, lib::string_view sign, const wamp::t_options_ref& dict);

        virtual int on_cancel(const connection_ptr& con, wamp::t_request_id req_id, const wamp::t_options_ref& opts);

        virtual int on_interrupt(const connection_ptr& con, wamp::t_request_id req_id, const wamp::t_options_ref& opts);

        virtual int on_open_stream(const connection_ptr& con);

        virtual int on_opened_stream(const connection_ptr& con);

        virtual int on_stream_data(const connection_ptr& con);

        virtual int on_close_stream(const connection_ptr& con);

        virtual int on_closed_stream(const connection_ptr& con);

        virtual int on_acknowledge(const connection_ptr& con);

        virtual int on_event_acknowledge(const connection_ptr& con,
                                         wamp::t_subscription_id sub_id,
                                         wamp::t_publication_id pub_id);

        virtual void write(const connection_hndl& hndr, wamp::msgpack_out_ptr&& msg);

        virtual void unknown(const connection_hndl& hndr, const msgpack::object& obj);

        virtual void hello(const connection_hndl& hndr, lib::string_view uri, const wamp::t_options& dict = {});

        virtual void welcome(const connection_hndl& hndr, wamp::t_session_id id, const wamp::t_options& opts = {});

        virtual void abort(const connection_hndl& hndr, const wamp::t_options& dict, lib::string_view uri);
        virtual void goodbye(const connection_hndl& hndr, const wamp::t_options& dict, lib::string_view uri);

        virtual void error(const connection_hndl& hndr,
                           wamp::code::Type msg_type,
                           wamp::t_request_id req_id,
                           const wamp::t_details& details,
                           lib::string_view uri,
                           const wamp::t_args& args,
                           const wamp::t_kw_args& args_kv);

        virtual void publish(const connection_hndl& hndr,
                             wamp::t_request_id req_id,
                             const wamp::t_details& details,
                             lib::string_view uri,
                             const wamp::t_args& args,
                             const wamp::t_kw_args& args_kv);
        virtual void published(const connection_hndl& hndr, wamp::t_request_id req_id, wamp::t_publication_id pub_id);

        virtual void subscribe(const connection_hndl& hndr,
                               wamp::t_request_id req_id,
                               const wamp::t_details& details,
                               lib::string_view uri);

        virtual void subscribed(const connection_hndl& hndr, wamp::t_request_id req_id, wamp::t_subscription_id sub_id);

        virtual void unsubscribe(const connection_hndl& hndr,
                                 wamp::t_request_id req_id,
                                 wamp::t_subscription_id sub_id);
        virtual void unsubscribed(const connection_hndl& hndr, wamp::t_request_id req_id);

        virtual void event(const connection_hndl& hndr,
                           wamp::t_subscription_id sub_id,
                           wamp::t_publication_id pub_id,
                           const wamp::t_details& details = {},
                           const wamp::t_args& args = {},
                           const wamp::t_kw_args& args_kv = {});

        virtual void call(const connection_hndl& hndr,
                          wamp::t_request_id req_id,
                          const wamp::t_options& opts,
                          lib::string_view proc,
                          const wamp::t_args& args,
                          const wamp::t_kw_args& args_kv);

        virtual void result(const connection_hndl& hndr,
                            wamp::t_request_id req_id,
                            const wamp::t_details& details,
                            const wamp::t_args& args,
                            const wamp::t_kw_args& args_kv);

        virtual void register_(const connection_hndl& hndr,
                               wamp::t_request_id req_id,
                               const wamp::t_options& opts,
                               lib::string_view proc);

        virtual void registered(const connection_hndl& hndr, wamp::t_request_id req_id, wamp::t_registration_id reg_id);

        virtual void unregister(const connection_hndl& hndr, wamp::t_request_id req_id, wamp::t_registration_id reg_id);
        virtual void unregistered(const connection_hndl& hndr, wamp::t_request_id req_id);

        virtual void invocation(const connection_hndl& hndr,
                                wamp::t_request_id req_id,
                                wamp::t_registration_id reg_id,
                                const wamp::t_details& details = {},
                                const wamp::t_args& args = {},
                                const wamp::t_kw_args& args_kv = {});

        virtual void yield(const connection_hndl& hndr,
                           wamp::t_request_id req_id,
                           const wamp::t_options& opts,
                           const wamp::t_args& args,
                           const wamp::t_kw_args& args_kv);

        virtual void challenge(const connection_hndl& hndr,
                               lib::string_view auth_metod,
                               const wamp::t_options& dict = {});

        virtual void authenticate(const connection_hndl& hndr, lib::string_view sign, const wamp::t_options& dict = {});
        virtual void cancel(const connection_hndl& hndr, wamp::t_request_id req_id, const wamp::t_options& opts);
        virtual void interrupt(const connection_hndl& hndr, wamp::t_request_id req_id, const wamp::t_options& opts);
    };

}}  // namespace mplc::lwsxx
