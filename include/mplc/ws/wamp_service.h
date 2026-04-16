#pragma once
#include <share/config.h>
#include "wamp_realm.h"
#include "websocket_service.h"

namespace mplc { namespace ws {

    struct wamp_service_t : websocket_service_t {
        MPLCSHARE_API wamp_service_t() {}
        MPLCSHARE_API ~wamp_service_t() override = default;

        MPLCSHARE_API int add_realm(lib::weak_ptr<wamp_realm_t> realm, lib::string_view name);
        MPLCSHARE_API lib::shared_ptr<wamp_realm_t> find_realm(lib::string_view realm);

        MPLCSHARE_API int on_connect(lws* wsi, connection_ptr& con, pss_node_ptr& user) override;
        MPLCSHARE_API int on_message(connection_ptr con, message_ptr&& msg, const pss_node_ptr& user) override;

        MPLCSHARE_API virtual int on_wamp_message(const connection_ptr& con,
                                                  const pss_node_ptr& user,
                                                  wamp::code::Type type,
                                                  const msgpack::object_array& args);

        MPLCSHARE_API virtual int on_hello(const connection_ptr& con,
                                           lib::string_view uri,
                                           const wamp::t_options_ref& dict);

        virtual int on_authenticate(const connection_ptr& con, lib::string_view sign, const wamp::t_options_ref& dict) {
            return 0;
        }
        virtual int on_abort(const connection_ptr& con, const wamp::t_options_ref& dict, lib::string_view uri) {
            return 1;
        }
        virtual int on_goodbye(const connection_ptr& con, const wamp::t_options_ref& dict, lib::string_view uri) {
            return 1;
        }
        virtual int on_error(const connection_ptr& con,
                             wamp::code::Type msg_type,
                             wamp::t_request_id req_id,
                             const wamp::t_details_ref& details,
                             lib::string_view uri) {
            return 0;
        }
        MPLCSHARE_API virtual void challenge(const connection_hndl& con,
                                             lib::string_view auth_metod,
                                             const wamp::t_options& dict = {});
        MPLCSHARE_API virtual void welcome(const connection_hndl& con,
                                           wamp::t_session_id id,
                                           const wamp::t_options& opts = {});
        MPLCSHARE_API virtual void abort(const connection_hndl& con, const wamp::t_options& dict, lib::string_view uri);
        MPLCSHARE_API virtual void goodbye(const connection_hndl& con,
                                           const wamp::t_options& dict,
                                           lib::string_view uri);
        MPLCSHARE_API virtual void error(const connection_hndl& con,
                                         wamp::code::Type msg_type,
                                         wamp::t_request_id req_id,
                                         const wamp::t_details& details,
                                         lib::string_view uri);

        //        template<class T>
        //        using clr_t = boost::remove_const_t<boost::remove_reference_t<T>>;
        //        template<class T>
        //        clr_t<T> _get(const msgpack::object_array& arr, size_t N) {
        //            return arr.ptr[N].as<clr_t<T>>();
        //        }
        //#define _PP_GET_VAL(z, N, arg) BOOST_PP_COMMA_IF(N) _get<Arg##N>(arg, N)
        //
        //#define _PP_CALL(z, N, FuncName) \
//    template<class Res, class T BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, class Arg)> \
//    Res FuncName(T* obj, \
//                 Res (T::*Call)(const connection_ptr& /*,*/ BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N,
        //                 Arg)),        \
//                 const connection_ptr& con, \
//                 const msgpack::object_array& arr) { \
//        return (obj->*Call)(con BOOST_PP_COMMA_IF(N) BOOST_PP_REPEAT(N, _PP_GET_VAL, arr)); \
//    }
        //
        //        BOOST_PP_REPEAT(10, _PP_CALL, _proxy_call_mpack_func)
        //#undef _PP_GET_VAL
        //#undef _PP_CALL
        //
        //        template<class... Args>
        //        int _meta_call(int (wamp_service_t::*Call)(const connection_ptr&, Args...),
        //                       const connection_ptr& con,
        //                       const msgpack::object_array& arr) {
        //            if (arr.size == sizeof...(Args)) {
        //                return _proxy_call_mpack_func(this, Call, con, arr);
        //            }
        //            abort(con, {}, "wamp.invalid_args_count");
        //            return 1;
        //        }
        template<class... Args>
        void _write(const connection_hndl& con, wamp::code::Type type, Args&&... args) {
            write(con, lib::static_pointer_cast<msg_out_t>(wamp::msgpack_out_t::make(type, args...)));
        }
        /*lib::mutex mtx_sessions;
        lib::unordered_set<wamp_session_t*> m_sessions;*/
        lib::mutex mtx_reams;
        lib::unordered_map<std::string, lib::weak_ptr<wamp_realm_t>> m_realms;
    };
}}  // namespace mplc::lwsxx
