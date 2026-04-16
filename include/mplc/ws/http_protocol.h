#pragma once
#include <mplc/libs/string_view.hpp>
#include "share/config.h"
#include "protocol.h"
#include "http_service.h"
#include <mplc/libs/containers.hpp>

namespace mplc { namespace ws {
    struct http_protocol_t : protocol_t {
        MPLCSHARE_API static constexpr const char* name = "http";
        MPLCSHARE_API int on_event(lws* wsi, int reason, void* user, void* in, size_t len) override;
        MPLCSHARE_API virtual int filter_http_connection(lws* wsi, lib::string_view url);
        MPLCSHARE_API virtual int check_access(lws* wsi, lib::string_view url, session_t* session);
        MPLCSHARE_API int on_init(lws* wsi) override;
        MPLCSHARE_API int on_destroy(lws* wsi) override;
        MPLCSHARE_API static std::string get_url(lws* wsi);
        MPLCSHARE_API void stop() override;
        MPLCSHARE_API bool stopped() override;
        MPLCSHARE_API void restart() override;
        MPLCSHARE_API virtual int add_service(lib::weak_ptr<http_service_t> service_hndl);
        MPLCSHARE_API virtual lib::shared_ptr<http_service_t> find_http_service(lws* wsi, lib::string_view uri);

    private:
        lib::vector<lib::weak_ptr<http_service_t>> m_services;
        lib::mutex mtx_service;
        bool m_stop{};
    };

}}  // namespace mplc::ws
