#pragma once
#include "share/config.h"
#include <mplc/libs/bind.hpp>
#include "dependency.h"
#include "vhost.h"
#include "wamp_realm.h"
#include "websocket_protocol.h"

namespace mplc { namespace ws {
    struct websocket_session_t;
    struct websocket_client_t;
    enum LOG_LVL {
        LL_ERR = (1 << 0),
        LL_WARN = (1 << 1),
        LL_NOTICE = (1 << 2),
        LL_INFO = (1 << 3),
        LL_DEBUG = (1 << 4),
        LL_PARSER = (1 << 5),
        LL_HEADER = (1 << 6),
        LL_EXT = (1 << 7),
        LL_CLIENT = (1 << 8),
        LL_LATENCY = (1 << 9),
        LL_USER = (1 << 10),
        LL_THREAD = (1 << 11),
    };
    /**
     * \brief Default value  LOG_LVL::ERR
     * \param logs set like LOG_LVL::ERR | LOG_LVL::WARN
     */
    typedef void (*log_emit_t)(int level, const char* line);
    MPLCSHARE_API void set_log_level(/* LOG_LVL */ int ll, log_emit_t);
    struct vhost_t;
    struct client_interface;
    struct MPLCSHARE_API context_t {
        context_t();
        virtual ~context_t();
        int add_vhost(lib::unique_ptr<vhost_t>&& host);
        /**
         * \brief Register service in ctx
         * \param bind_url "<protocol_name>://<vhost_name></path>
         * \param service
         */
        int add_service(lib::weak_ptr<websocket_service_t> service,
                        lib::string_view path,
                        lib::string_view protocol,
                        lib::string_view host);
        int add_service(lib::weak_ptr<http_service_t> service,
                        lib::string_view protocol,
                        lib::string_view host);
        int add_realm(lib::weak_ptr<wamp_realm_t> realm,
                      lib::string_view realm_name,
                      lib::string_view path,
                      lib::string_view protocol,
                      lib::string_view host);

        int add_service(lib::weak_ptr<websocket_service_t> service, lib::string_view bind_url);
        int add_realm(lib::weak_ptr<wamp_realm_t> realm, lib::string_view realm_name, lib::string_view bind_url);
        vhost_t* get_vhost(lib::string_view name);
        void init();
        virtual void clear();
        void async_connect(lib::weak_ptr<client_interface> client);

        void delete_vhost(lib::string_view name);
        void post(lib::function<void()>&& cb);
        void post(lws* wsi, lib::function<void()>&& cb);
        /*MPLCSHARE_API void set_log_level(int level);
        MPLCSHARE_API void sul_schedule(lib::function<void()>&& cb);*/
        /*MPLCSHARE_API void wait();
        MPLCSHARE_API void stop();
        MPLCSHARE_API void start();*/

        /// from 0 to m_threads_count-1
        /// You must first build libwebsockets itself with cmake -DLWS_MAX_SMP=8 or some other number greater than one.
        void run(/*int thread_id = 0*/);
        int run_one(/*int thread_id = 0*/);
        bool is_stop() const;
        virtual void stop();
        void restart();
        void notify();
        int on_notify();

    protected:
        void on_stop();
        // void _service(int thread) {}
        void init_new_vhosts();
        void connect_new_clients();

    private:
        void _delete_vhost(const std::string& str);
        struct impl;
        impl* pimpl;
        lws_context* m_ctx{};
        // std::vector<lib::thread> m_threads;
        // std::map<std::string, lib::unique_ptr<vhost_t>> m_vhsots;
        // std::list<lib::unique_ptr<vhost_t>> pending_vhosts;
        // std::list<lib::weak_ptr<websocket_client_t>> new_clients;
        // lib::unique_ptr<websocket_protocol_t> defprot;
        // lib::recursive_mutex mtx_vhost;
        // lib::recursive_mutex mtx_clients;
        // lws_context* m_ctx;
        // int m_threads_count;
        //bool m_stop;
        bool m_stoped;
        // static lws_sorted_usec_list_t sul, sul_timeout;
    };

}}  // namespace mplc::ws
