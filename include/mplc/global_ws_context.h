#pragma once
#include "ws/context.h"
#include "ws/vhost.h"
#include "ws/wamp_realm.h"
namespace mplc { namespace gws {

    // enum GlobalWsContextMode {
    //     gwsCtxInited,
    //     gwsVHostsInited,
    //     gwsProtocolsInited,
    //     gwsCtxStart,
    //     gwsCtxStop,
    // };

    // using gws_config_cb = lib::function<int(GlobalWsContextMode mode, ControllerConfig* config)>;

    // int RegisterGwsContextProcessor(const char* name,
    //                                 uint32_t modeMask,
    //                                 gws_config_cb func,
    //                                 const void* data,
    //                                 int priority,
    //                                 int* pnIndex);

    struct default_vhost_t : ws::vhost_t {
        static constexpr const char* name = "default";
        MPLCSHARE_API default_vhost_t();
    };
    struct wamp_2_msgpack : ws::websocket_protocol_t {
        static constexpr const char* name = "wamp.2.msgpack";
    };
    MPLCSHARE_API uint16_t get_base_port();
    MPLCSHARE_API uint16_t get_exemplar_port(int exemplar);
    MPLCSHARE_API int add_vhost(lib::unique_ptr<ws::vhost_t>&& vhost);
    MPLCSHARE_API void delete_vhost(lib::string_view name);
    MPLCSHARE_API int add_ws_protocol(lib::unique_ptr<ws::websocket_protocol_t>&& protocol,
                                      lib::string_view name,
                                      lib::string_view host = default_vhost_t::name);
    MPLCSHARE_API int add_service(lib::weak_ptr<ws::websocket_service_t> service,
                                  lib::string_view path,
                                  lib::string_view protocol = ws::protocol_t::name,
                                  lib::string_view host = default_vhost_t::name);
    MPLCSHARE_API int add_realm(lib::weak_ptr<ws::wamp_realm_t> realm,
                                lib::string_view realm_name,
                                lib::string_view path,
                                lib::string_view protocol = wamp_2_msgpack::name,
                                lib::string_view host = default_vhost_t::name);

    MPLCSHARE_API ws::context_t& get_context();
    // ConfigProcessorWsConextInit
    // MPLCSHARE_API void InitContext();
    // ConfigProcessorWsHostsInit
    // MPLCSHARE_API int InitHsots();
    // ConfigProcessorWsProtocolsInit
    // MPLCSHARE_API void InitProtocolst();
    // ConfigProcessorLoaded
    //  MPLCSHARE_API void InitServices();
    // ConfigProcessorBeforeStartTasks
    // MPLCSHARE_API void StartContext();
    // ConfigProcessorAfterStopTasks
    // MPLCSHARE_API void StopContext();
    // ConfigProcessorStopped
    // MPLCSHARE_API void ClearContext();
}}  // namespace mplc::gws
