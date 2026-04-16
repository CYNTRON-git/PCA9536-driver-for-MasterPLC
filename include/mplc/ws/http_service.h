#pragma once

#include "http_session.h"
#include "mplc/libs/string_view.hpp"
#include <mplc/libs/bind.hpp>

namespace mplc { namespace ws {
    struct IRequest {
        enum Type { tGET, tPOST };
        virtual void init(http_session_ptr& con, lib::string_view uri) = 0;
        virtual Type type() = 0;
        virtual int on_write(http_session_ptr& con) = 0;
        virtual uint64_t max_buf_size() {
            return 0;
        }
        virtual int on_http_file_complete(http_session_ptr& con) {
            return 0;
        }
        virtual ~IRequest() = default;
    };

    struct GetRequest : IRequest {
        Type type() override {
            return tGET;
        }
    };

    struct PostRequest : IRequest {
        Type type() override {
            return tPOST;
        }
        virtual int on_data(http_session_ptr& con, void* in, size_t len) {
            return 0;
        }
        virtual int on_data_complite(http_session_ptr& con) {
            con->callback_on_writable();
            return 0;
        }
    };

    MPLCSHARE_API int reject_request(lws* wsi, int code = 404);

    struct MPLCSHARE_API http_service_t {
        using request_factory_t =
            lib::function<lib::unique_ptr<IRequest>(http_session_ptr& connection, lib::string_view url)>;

        template<class T>
        static lib::unique_ptr<IRequest> create(http_session_ptr& connection, lib::string_view url) {
            auto request = lib::make_unique<T>();
            request->init(connection, url);
            return request;
        }
        // TODO может потом просто определять по типу T
        template<class T>
        void add_get(const std::string& path) const {
            internal_reg(path, create<T>, IRequest::tGET);
        }
        template<class T>
        void add_post(const std::string& path) const {
            internal_reg(path, create<T>, IRequest::tPOST);
        }

        struct impl;
        impl* pimpl;

        http_service_t(std::string url);
        virtual ~http_service_t();

        virtual bool try_match(lws* wsi, lib::string_view url);
        virtual int filter_connection(lws* wsi, lib::string_view url);
        virtual int on_http(lws* wsi, lib::string_view uri, pss_node_ptr& user);
        virtual int on_http_body(lws* wsi, pss_node_ptr& user, void* in, size_t len);
        int on_http_body_complete(lws* lws, pss_node_ptr& user, void* in, size_t len);
        int on_http_file_complete(lws* lws, pss_node_ptr& user, void* in, size_t len);
        int on_write(lws* lws, pss_node_ptr& user, void* in, size_t len);
        int on_closed(lws* wsi, pss_node_ptr& user, void* in, size_t len);
        virtual void run();
        virtual void start() {}
        virtual void stop();

    private:
        void internal_reg(const std::string& path, const request_factory_t& factory, IRequest::Type type) const;
    };

}}  // namespace mplc::ws
