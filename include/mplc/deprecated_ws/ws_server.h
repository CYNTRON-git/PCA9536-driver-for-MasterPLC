#pragma once

#include <libwebsockets.h>
#include "mplc/deprecated_wsapi.h"
#include <mplc/libs/threads.hpp>

namespace mplc { namespace deprecated_ws {
    typedef void(on_protocol_inited_cb)(void* _this, void* protocol, const char* proto_name);

    class WsMount {};

    class WsProtocol {};

    class WsVHost {
        virtual void AddProtocol(const lws_protocols* proto) {
            if (m_protocols.empty()) {
                m_protocols.push_back(proto);
            } else {
                m_protocols[m_protocols.size() - 1] = proto;
            }
            m_protocols.push_back(nullptr);
        }
        virtual const lws_protocols** GetProtocols() {
            return m_protocols.data();
        }
        virtual const lws_extension* GetExtensions();
        virtual const lws_http_mount* GetMounts() {
            return nullptr;
        }
        virtual void Create(lws_context* ctx);
        virtual uint16_t Port() {
            return 7451;
        }
        virtual const char* BindIP() {
            return nullptr;
        }
        virtual bool InitSSL(lws_context_creation_info& info);
        WsVHost(): m_vhost(nullptr) {}
        static void Finalize(struct lws_vhost* vh, void* arg) {
            WsVHost* vhost = static_cast<WsVHost*>(arg);
            delete vhost;
        }

    protected:
        std::vector<const struct lws_protocols*> m_protocols;
        struct lws_vhost* m_vhost;
    };

    class WsContext {
        struct lws_context* ctx;
        WsContext(struct lws_context* ctx): ctx(ctx) {}
        static WsContext* Create();
    };

    class WsServer {
    public:
        WsServer(): m_stop(true), context(nullptr), m_th(nullptr) {}

        ~WsServer() {
            if (m_th) {
                m_th->join();
                lws_context_destroy(context);
            }
        }
        MPLC_WS_API int Create(uint16_t port,
                               const std::string& passwd,
                               lws_protocols* protocols,
                               const std::string& iface = "",
                               const lws_protocol_vhost_options* opts = nullptr,
                               const lws_http_mount* mounts = nullptr);

        MPLC_WS_API void Run();
        MPLC_WS_API bool IsRun() const {
            return !m_stop;
        }
        void Stop() {
            m_stop = true;
        }
        const lws_protocols* GetProtocols();
        const lws_extension* GetExtensions();
        const lws_http_mount* GetMounts();
        // static void OnProtocolInited(void* _this, void* protocol, const char* proto_name);
    private:
        void worker();
        bool m_stop;
        struct lws_context* context;
        lib::unique_ptr<lib::thread> m_th;
    };
}}  // namespace mplc::ws
