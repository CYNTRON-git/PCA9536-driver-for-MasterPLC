#pragma once
#include "dependency.h"
namespace mplc { namespace ws {
    struct context_t;
    struct protocol_t;
    using protocol_ptr = lib::unique_ptr<protocol_t>;
    struct MPLCSHARE_API protocol_t {
        static constexpr const char* name = "defprot";
        virtual ~protocol_t() = default;
        virtual int on_event(lws* wsi, int event_type, void* user, void* in, size_t len) = 0;
        /*{ return lws_callback_http_dummy(wsi, reason, user, in, len); }*/
        virtual int on_init(lws* wsi) {
            return 0;
        }
        /*{ return 0; }*/
        virtual int on_destroy(lws* wsi) {
            return 0;
        }
        /* { return 0; }*/
        virtual size_t rx_buf_size() const {
            return 4096;
        }
        virtual size_t tx_buf_size() const {
            return 4096;
        }
        virtual size_t per_session_data_size() const {
            return sizeof(void*);
        }
        virtual void stop() = 0;
        virtual bool stopped() = 0;
        virtual void restart() = 0;
        context_t* get_ctx(struct lws* wsi);
    };

}}  // namespace mplc::ws
