#pragma once

#include "session.h"

namespace mplc::ws {
    struct http_session_t : public session_t {
        MPLCSHARE_API explicit http_session_t(lws wsi): session_t(wsi) {}
        MPLCSHARE_API ~http_session_t() override = default;

        MPLCSHARE_API int send_responce(unsigned int code, const char* html_body) const;
        MPLCSHARE_API int send_file(const char* file, const char* content_type) const;
        MPLCSHARE_API int64_t get_content_length() const;
        MPLCSHARE_API std::string get_urlarg_by_name(const std::string& name) const;
    };

    using http_session_ptr = lib::shared_ptr<http_session_t>;

}  // namespace mplc::ws
