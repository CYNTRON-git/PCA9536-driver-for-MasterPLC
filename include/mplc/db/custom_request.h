#pragma once

#include <mplc/libs/smart_ptr.hpp>
#include <mplc/libs/threads.hpp>
#include <msgpack.hpp>

namespace mplc { namespace db {
    struct CustomRequest {
        ADD_MAKE_PTR(CustomRequest)

        std::string sql;
        msgpack::zone _z;
        std::vector<std::string> cols;
        std::vector<msgpack::object> rows;
        lib::condition_variable cv;
        lib::mutex mtx;
        CustomRequest() {}
    };
}}  // namespace mplc::db
