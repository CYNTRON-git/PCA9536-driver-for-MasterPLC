#pragma once
#include <mplc/msgpack_ext.hpp>
#include <mplc/deprecated_ws/ws_data.h>

namespace mplc { namespace deprecated_ws { namespace wamp {

    /*
     * | Cod | Message        |  Pub |  Brk | Subs |  Calr | Dealr | Callee|
     * |-----|----------------|------|------|------|-------|-------|-------|
     * |  1  | `HELLO`        | Tx   | Rx   | Tx   | Tx    | Rx    | Tx    |
     * |  2  | `WELCOME`      | Rx   | Tx   | Rx   | Rx    | Tx    | Rx    |
     * |  3  | `ABORT`        | Rx   | TxRx | Rx   | Rx    | TxRx  | Rx    |
     * |  6  | `GOODBYE`      | TxRx | TxRx | TxRx | TxRx  | TxRx  | TxRx  |
     * |     |                |      |      |      |       |       |       |
     * |  8  | `ERROR`        | Rx   | Tx   | Rx   | Rx    | TxRx  | TxRx  |
     * |     |                |      |      |      |       |       |       |
     * | 16  | `PUBLISH`      | Tx   | Rx   |      |       |       |       |
     * | 17  | `PUBLISHED`    | Rx   | Tx   |      |       |       |       |
     * |     |                |      |      |      |       |       |       |
     * | 32  | `SUBSCRIBE`    |      | Rx   | Tx   |       |       |       |
     * | 33  | `SUBSCRIBED`   |      | Tx   | Rx   |       |       |       |
     * | 34  | `UNSUBSCRIBE`  |      | Rx   | Tx   |       |       |       |
     * | 35  | `UNSUBSCRIBED` |      | Tx   | Rx   |       |       |       |
     * | 36  | `EVENT`        |      | Tx   | Rx   |       |       |       |
     * |     |                |      |      |      |       |       |       |
     * | 48  | `CALL`         |      |      |      | Tx    | Rx    |       |
     * | 50  | `RESULT`       |      |      |      | Rx    | Tx    |       |
     * |     |                |      |      |      |       |       |       |
     * | 64  | `REGISTER`     |      |      |      |       | Rx    | Tx    |
     * | 65  | `REGISTERED`   |      |      |      |       | Tx    | Rx    |
     * | 66  | `UNREGISTER`   |      |      |      |       | Rx    | Tx    |
     * | 67  | `UNREGISTERED` |      |      |      |       | Tx    | Rx    |
     * | 68  | `INVOCATION`   |      |      |      |       | Tx    | Rx    |
     * | 70  | `YIELD`        |      |      |      |       | Rx    | Tx    |
     * |     |                |      |      |      |       |       |       |
     * |  4  | `CHALLENGE`    | Rx   | Tx   | Rx   | Rx    | Tx    | Rx    |
     * |  5  | `AUTHENTICATE` | Tx   | Rx   | Tx   | Tx    | Rx    | Tx    |
     * | 49  | `CANCEL`       |      |      |      | Tx    | Rx    |       |
     * | 69  | `INTERRUPT`    |      |      |      |       | Tx    | Rx    |
     */

    typedef int64_t t_wapm_id;
    typedef int64_t t_session_id;
    typedef int64_t t_request_id;

    typedef int64_t t_stream_id;
    typedef int64_t t_stream_pack_id;

    typedef int64_t t_subscription_id;
    typedef int64_t t_publication_id;
    typedef int64_t t_registration_id;
    inline t_wapm_id CreateRandId() {
        return std::rand();
    }
    typedef lib::unordered_map<lib::string_view, const msgpack::object*> t_options_ref;
    // typedef lib::unordered_map<std::string, msgpack::object> t_options1;
    class t_options {
        msgpack::zone z;
        lib::unordered_map<std::string, msgpack::object> m_data;

    public:
        template<class T>
        void set(const std::string& name, const T& val) {
            m_data[name] = msgpack::object(val, z);
        }
        const lib::unordered_map<std::string, msgpack::object>& data() const {
            return m_data;
        }
    };

    template<class T>
    bool safe_get_opt(const t_options_ref& opts, lib::string_view name, T& val) {
        try {
            auto opt_it = opts.find(name);

            if (opt_it == opts.end()) {
                return false;
            }
            if (!opt_it->second->convert_if_not_nil(val))
                return false;
            return true;
        } catch (msgpack::type_error&) {
            return false;
        }
    }
    // struct event_info {
    //    t_subscription_id subscription_id;
    //    t_options_ref details;
    //    // wamp_args args;
    //    void* user;
    //};
    // typedef lib::function<void(event_info)> on_event_fn;

    // struct subscribed_info {
    //    t_request_id request_id;
    //    t_subscription_id subscription_id;
    //    bool was_error;
    //    std::string error_uri;
    //    void* user;

    //    /** Returns whether this message indicates the request was successful. */
    //    explicit operator bool() const noexcept { return was_error == false; }
    //};

    ///** Callback invoked when a subscribe request is successful or fails. Error
    //    contains the error code when the subscription is not successful.
    //*/
    // typedef lib::function<void(subscribed_info)> on_subscribed_fn;

    // struct subscribe_request {
    //    std::string uri;
    //    on_subscribed_fn subscribed_cb;
    //    on_event_fn event_cb;
    //    void* user;
    //};
    struct MsgPackData : deprecated_ws::WsData {
        template<class T>
        static lib::shared_ptr<MsgPackData> create(T const& data) {
            lib::shared_ptr<MsgPackData> msg = lib::make_shared<MsgPackData>();
            msgpack::pack(*msg, data);
            msg->m_final = true;
            msg->binary = true;
            return msg;
        }

        void write(const char* buf, size_t len) {
            insert(buf, len);
        }
        MsgPackData() {
            binary = true;
        }
        static lib::shared_ptr<MsgPackData> make() {
            return lib::make_shared<MsgPackData>();
        }
    };

    typedef lib::shared_ptr<MsgPackData> pMsgPackData;

    namespace code {
        enum Type {
            M_UNKNOWN = 0,
            M_HELLO = 1,
            M_WELCOME = 2,
            M_ABORT = 3,
            M_GOODBYE = 6,
            M_ERROR = 8,
            M_PUBLISH = 16,
            M_PUBLISHED = 17,
            M_SUBSCRIBE = 32,
            M_SUBSCRIBED = 33,
            M_UNSUBSCRIBE = 34,
            M_UNSUBSCRIBED = 35,
            M_EVENT = 36,
            M_CALL = 48,
            M_RESULT = 50,
            M_REGISTER = 64,
            M_REGISTERED = 65,
            M_UNREGISTER = 66,
            M_UNREGISTERED = 67,
            M_INVOCATION = 68,
            M_YIELD = 70,
            M_CHALLENGE = 4,
            M_AUTHENTICATE = 5,
            M_CANCEL = 49,
            M_INTERRUPT = 69,

            // Extension

            M_OPEN_STREAM = 300,
            M_OPENED_STREAM = 301,
            M_STREAM_DATA = 302,
            M_CLOSE_STREAM = 303,
            M_CLOSED_STREAM = 304,
            M_ACKNOWLEDGE = 305,
        };
    }

}}}  // namespace mplc::deprecated_ws::wamp

namespace msgpack {
    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
        namespace adaptor {
            // --- Enum
            template<>
            struct convert<mplc::deprecated_ws::wamp::t_options_ref> {
                msgpack::object const& operator()(msgpack::object const& o,
                                                  mplc::deprecated_ws::wamp::t_options_ref& v) const {
                    if (o.type != msgpack::type::MAP)
                        throw msgpack::type_error();

                    for (const object_kv* it = msgpack::begin(o.via.map); it != msgpack::end(o.via.map); ++it) {
                        if (it->key.type != msgpack::type::STR)
                            continue;
                        v[boost::string_view(it->key.via.str.ptr, it->key.via.str.size)] = &it->val;
                    }
                    return o;
                }
            };

            template<>
            struct pack<mplc::deprecated_ws::wamp::t_options> {
                template<typename Stream>
                packer<Stream>& operator()(msgpack::packer<Stream>& o,
                                           const mplc::deprecated_ws::wamp::t_options& v) const {
                    o.pack(v.data());
                    return o;
                }
            };
        }  // namespace adaptor
    }
}  // namespace msgpack
