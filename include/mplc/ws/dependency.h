#pragma once
#include <mplc/libs/smart_ptr.hpp>
#include <share/config.h>

struct lws_extension; /* needed even with ws exts disabled for create context */
struct lws_token_limits;
struct lws_protocols;
struct lws_context;
struct lws_tokens;
struct lws_vhost;
struct lws_http_mount;
struct lws;
struct lws_context_creation_info;
struct lws_client_connect_info;
namespace mplc { namespace ws {

    struct pss_data_t {
        virtual ~pss_data_t() = default;
    };
    struct pss_node_t {
        lib::shared_ptr<pss_node_t> next{};
        lib::unique_ptr<pss_data_t> data{};
        enum type_t : uint8_t { none = 0, client, server } type = none;
        static lib::shared_ptr<pss_node_t> make() {
            return lib::make_shared<pss_node_t>();
        }
        virtual ~pss_node_t() {
            int wtf = 1;
        };
    };
    using pss_node_ptr = lib::shared_ptr<pss_node_t>;
    /*struct ll_pss_data_t {
        pss_node_t head;
    };*/
    struct websocket_session_t;
}}  // namespace mplc::ws

// namespace lib {
//    using std::condition_variable;
//    using std::lock_guard;
//    using std::make_shared;
//    using std::mutex;
//    using std::shared_ptr;
//    using std::thread;
//    namespace this_thread = std::this_thread;
//    using std::atomic_flag;
//    using std::make_unique;
//    using std::string_view;
//    using std::unique_lock;
//    using std::unique_ptr;
//    using std::unordered_map;
//    inline string_view make_string_view(const void* data, size_t len) {
//        return string_view(static_cast<const char*>(data), len);
//    }
//    template<typename TO, typename FROM>
//    unique_ptr<TO> static_pointer_cast(unique_ptr<FROM>&& old) {
//        return unique_ptr<TO>{static_cast<TO*>(old.release())};
//        // conversion: unique_ptr<FROM>->FROM*->TO*->unique_ptr<TO>
//    }
//}  // namespace lib
