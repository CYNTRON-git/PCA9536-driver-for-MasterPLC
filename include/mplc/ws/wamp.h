#pragma once
#pragma warning(push)
#pragma warning(disable : 4267)
#include <mplc/msgpack_ext.hpp>
#include "share/config.h"
#include "websocket_session.h"

namespace mplc { namespace ws { namespace wamp {
    MPLCSHARE_API int64_t get_rand_id();
#pragma warning(push)
#pragma warning(disable : 4554)
    inline int64_t valid_id(uint64_t u64) {
        static const uint64_t mask = (0xFFFFFFFFFFFFFFFFULL >> 64 - 53);
        return u64 & mask;
    }
#pragma warning(pop)
    namespace code {
        enum Type {
            M_UNKNOWN = 0,
            M_HELLO = 1,
            M_WELCOME = 2,
            M_ABORT = 3,
            M_CHALLENGE = 4,
            M_AUTHENTICATE = 5,
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
            M_CANCEL = 49,
            M_INTERRUPT = 69,
            // Extension
            M_OPEN_STREAM = 300,
            M_OPENED_STREAM = 301,
            M_STREAM_DATA = 302,
            M_CLOSE_STREAM = 303,
            M_CLOSED_STREAM = 304,
            M_ACKNOWLEDGE = 305,
            M_EVENT_ACKNOWLEDGE = 306,
            _LAST = M_EVENT_ACKNOWLEDGE,
        };
    }
    /*
     * | Cod | Message        |  Pub |  Brk | Subs |  Calr | Dealr | Callee|
     * |-----|----------------|------|------|------|-------|-------|-------|
     * |  1  | `HELLO`        | Tx   | Rx   | Tx   | Tx    | Rx    | Tx    |
     * |  2  | `WELCOME`      | Rx   | Tx   | Rx   | Rx    | Tx    | Rx    |
     * |  3  | `ABORT`        | Rx   | TxRx | Rx   | Rx    | TxRx  | Rx    |
     * |  4  | `CHALLENGE`    | Rx   | Tx   | Rx   | Rx    | Tx    | Rx    |
     * |  5  | `AUTHENTICATE` | Tx   | Rx   | Tx   | Tx    | Rx    | Tx    |
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
    using t_kw_args_ref = lib::unordered_map<lib::string_view, const msgpack::object*>;
    using t_args_ref = std::vector<const msgpack::object*>;
    using t_options_ref = t_kw_args_ref;
    using t_details_ref = t_kw_args_ref;
    // typedef lib::unordered_map<std::string, msgpack::object> t_options1;
    struct t_kw_args {
        t_kw_args() {}
        /*template<class T>
        void set(const std::string& name, const T& val) {
            m_data[name] = msgpack::object(val, z);
        }*/
        template<class T>
        void set(lib::string_view name, const T& val) {
            m_data[std::string(name)] = msgpack::object(val, z);
        }
        void reserve(size_t size) {
            m_data.reserve(size);
        }
        const lib::unordered_map<std::string, msgpack::object>& data() const {
            return m_data;
        }

    private:
        msgpack::zone z;
        lib::unordered_map<std::string, msgpack::object> m_data;
    };
    struct t_args {
        template<class T>
        void push(T&& val) {
            m_data.emplace_back(msgpack::object(val, z));
        }
        template<class T, class... Args>
        void push(T&& val, Args&&... args) {
            m_data.reserve(m_data.size() + 1 + sizeof...(Args));
            m_data.emplace_back(msgpack::object(val, z));
            push(args...);
        }
        void reserve(size_t size) {
            m_data.reserve(size);
        }
        const std::vector<msgpack::object>& data() const {
            return m_data;
        }
        template<class T>
        t_args& operator<<(const T& val) {
            push(val);
            return *this;
        }

    private:
        msgpack::zone z;
        std::vector<msgpack::object> m_data;
    };

    using t_options = t_kw_args;
    using t_details = t_kw_args;
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
    struct msgpack_out_t : msg_out_t {
        msgpack_out_t(): msg_out_t(msg_type::WS_BIN) {}
        void write(const char* buf, size_t len) {
            append(buf, len);
        }
        static lib::unique_ptr<msgpack_out_t> make() {
            return lib::make_unique<wamp::msgpack_out_t>();
        }
        template<class... Args>
        static lib::unique_ptr<msgpack_out_t> make(Args&&... args) {
            auto msg = lib::make_unique<wamp::msgpack_out_t>();
            if (sizeof...(Args)) {
                msgpack::packer<wamp::msgpack_out_t> pk(*msg);
                pk.pack_array(sizeof...(Args));
                _pack(pk, args...);
            }
            return msg;
        }
        template<class Head, class... Tail>
        static void _pack(msgpack::packer<wamp::msgpack_out_t>& pk, Head&& arg, Tail&&... tail) {
            pk.pack(arg);
            _pack(pk, tail...);
        }
        template<class Head>
        static void _pack(msgpack::packer<wamp::msgpack_out_t>& pk, Head&& arg) {
            pk.pack(arg);
        }
    };
    using msgpack_out_ptr = lib::unique_ptr<msgpack_out_t>;

    namespace _detail {
        template<class T>
        using clr_t = boost::remove_const_t<boost::remove_reference_t<T>>;
        template<size_t N, class T>
        clr_t<T> _get(const msgpack::object_array& arr) {
            return arr.ptr[N].as<clr_t<T>>();
        }
#define _PP_GET_VAL(z, N, arg) BOOST_PP_COMMA_IF(N) _get<N, Arg##N>(arg)

#define _PP_CALL(z, N, FuncName)                                                                                       \
    template<class Res, class T BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, class Arg)>                               \
    Res FuncName(T* obj, Res (T::*Call)(BOOST_PP_ENUM_PARAMS(N, Arg)), const msgpack::object_array& arr) {             \
        return (obj->*Call)(BOOST_PP_REPEAT(N, _PP_GET_VAL, arr));                                                     \
    }

        BOOST_PP_REPEAT(10, _PP_CALL, _proxy_call_mpack_func)
    }  // namespace _detail
#undef _PP_GET_VAL
#undef _PP_CALL
    template<class Fail, class Res, class T, class... Args>
    Res call_wamp_func(T* obj, Res (T::*Call)(Args...), const msgpack::object_array& arr, Fail on_error) {
        if (arr.size == sizeof...(Args)) {
            return _detail::_proxy_call_mpack_func(obj, Call, arr);
        }
        on_error(arr);
        return 1;
    }

    template<class T>
    _detail::clr_t<T> safe_get(const msgpack::object* obj) try {
        if (obj) {
            return obj->as<_detail::clr_t<T>>();
        }
        return _detail::clr_t<T>{};
    } catch (const msgpack::type_error& ex) {
        return _detail::clr_t<T>{};
    }
    template<class T>
    _detail::clr_t<T> safe_get(const msgpack::object_array& arr, size_t n) try {
        if (arr.size > n) {
            return arr.ptr[n].as<_detail::clr_t<T>>();
        }
        return _detail::clr_t<T>{};
    } catch (const msgpack::type_error& ex) {
        return _detail::clr_t<T>{};
    }
    template<class T>
    _detail::clr_t<T> safe_get(const t_args_ref& arr, size_t n) try {
        if (arr.size() > n) {
            return arr[n]->as<_detail::clr_t<T>>();
        }
        return _detail::clr_t<T>{};
    } catch (const msgpack::type_error& ex) {
        return _detail::clr_t<T>{};
    }
    template<class T>
    _detail::clr_t<T> safe_get(const t_kw_args_ref& kw, lib::string_view key) try {
        auto it = kw.find(key);
        if (it != kw.end()) {
            return it->second->as<_detail::clr_t<T>>();
        }
        return _detail::clr_t<T>{};
    } catch (const msgpack::type_error& ex) {
        return _detail::clr_t<T>{};
    }
}}}  // namespace mplc::ws::wamp
template<>
struct msgpack::adaptor::convert<mplc::ws::wamp::t_kw_args> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::ws::wamp::t_kw_args& v) const {
        if (o.type != msgpack::type::MAP)
            throw msgpack::type_error();
        v.reserve(o.via.map.size);
        for (const msgpack::object_kv* it = msgpack::begin(o.via.map); it != msgpack::end(o.via.map); ++it) {
            if (it->key.type != msgpack::type::STR) {
                continue;
            }
            v.set(it->key.as<mplc::lib::string_view>(), it->val);
        }
        return o;
    }
};
template<>
struct msgpack::adaptor::pack<mplc::ws::wamp::t_kw_args> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, const mplc::ws::wamp::t_kw_args& v) const {
        o.pack_map(v.data().size());
        for (auto it = v.data().begin(); it != v.data().end(); ++it) {
            o.pack(it->first);
            o.pack(it->second);
        }
        return o;
    }
};
template<>
struct msgpack::adaptor::object_with_zone<mplc::ws::wamp::t_kw_args> {
    void operator()(msgpack::object::with_zone& o, const mplc::ws::wamp::t_kw_args& v) const {
        o.type = type::MAP;
        o.via.map.size = v.data().size();
        o.via.map.ptr = static_cast<msgpack::object_kv*>(
            o.zone.allocate_align(sizeof(msgpack::object_kv) * o.via.map.size,
                                  MSGPACK_ZONE_ALIGNOF(msgpack::object_kv)));
        size_t pos = 0;
        for (auto it = v.data().begin(); it != v.data().end(); ++it) {
            o.via.map.ptr[pos].key = msgpack::object(it->first, o.zone);
            o.via.map.ptr[pos].val = msgpack::object(it->second, o.zone);
        }
    }
};
// t_args
template<>
struct msgpack::adaptor::convert<mplc::ws::wamp::t_args> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::ws::wamp::t_args& v) const {
        if (o.type != msgpack::type::ARRAY)
            throw msgpack::type_error();
        v.reserve(o.via.array.size);
        for (const msgpack::object* it = msgpack::begin(o.via.array); it != msgpack::end(o.via.array); ++it) {
            v.push(*it);
        }
        return o;
    }
};
template<>
struct msgpack::adaptor::pack<mplc::ws::wamp::t_args> {
    template<typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, const mplc::ws::wamp::t_args& v) const {
        o.pack_array(v.data().size());
        for (auto it = v.data().begin(); it != v.data().end(); ++it) {
            o.pack(*it);
        }
        return o;
    }
};
template<>
struct msgpack::adaptor::object_with_zone<mplc::ws::wamp::t_args> {
    void operator()(msgpack::object::with_zone& o, const mplc::ws::wamp::t_args& v) const {
        o.type = type::ARRAY;
        o.via.array.size = v.data().size();
        o.via.array.ptr = static_cast<msgpack::object*>(
            o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size, MSGPACK_ZONE_ALIGNOF(msgpack::object)));
        size_t pos = 0;
        for (auto it = v.data().begin(); it != v.data().end(); ++it) {
            o.via.array.ptr[pos] = msgpack::object(*it, o.zone);
        }
    }
};
// mplc::lwsxx::wamp::t_kw_args_ref
template<>
struct msgpack::adaptor::convert<mplc::ws::wamp::t_kw_args_ref> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::ws::wamp::t_kw_args_ref& v) const {
        if (o.type != msgpack::type::MAP)
            throw msgpack::type_error();
        v.reserve(o.via.map.size);
        for (const msgpack::object_kv* it = msgpack::begin(o.via.map); it != msgpack::end(o.via.map); ++it) {
            v[it->key.as<mplc::lib::string_view>()] = &it->val;
        }
        return o;
    }
};
// mplc::lwsxx::wamp::t_args_ref
template<>
struct msgpack::adaptor::convert<mplc::ws::wamp::t_args_ref> {
    msgpack::object const& operator()(msgpack::object const& o, mplc::ws::wamp::t_args_ref& v) const {
        if (o.type != msgpack::type::ARRAY)
            throw msgpack::type_error();
        v.reserve(o.via.array.size);
        for (const msgpack::object* it = msgpack::begin(o.via.array); it != msgpack::end(o.via.array); ++it) {
            v.push_back(it);
        }
        return o;
    }
};

// object*
// template<>
// struct msgpack::adaptor::convert<msgpack::object*> {
//    msgpack::object const& operator()(msgpack::object const& o, msgpack::object*& v) const {
//        if (o.type != msgpack::type::MAP)
//            throw msgpack::type_error();
//        for (const msgpack::object_kv* it = msgpack::begin(o.via.map); it != msgpack::end(o.via.map); ++it) {
//            if (it->key.type != msgpack::type::STR) {
//                continue;
//            }
//            v.set(it->key.as<lib::string_view>(), it->val);
//        }
//        return o;
//    }
//};
// template<>
// struct msgpack::adaptor::pack<msgpack::object*> {
//    template<typename Stream>
//    packer<Stream>& operator()(msgpack::packer<Stream>& o, const msgpack::object*& v) const {
//        if (v) {
//            o.pack(*v);
//        } else {
//            o.pack_nil();
//        }
//        return o;
//    }
//};
// template<>
// struct msgpack::adaptor::object_with_zone<msgpack::object*> {
//    void operator()(msgpack::object::with_zone& o, const msgpack::object*& v) const {
//        if (v) {
//            object_with_zone<msgpack::object> adaptr;
//            adaptr(o, *v);
//        } else {
//            o.type = type::NIL;
//        }
//    }
//};
#pragma warning(pop)
