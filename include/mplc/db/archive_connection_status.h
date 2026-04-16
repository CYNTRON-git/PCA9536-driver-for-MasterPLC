#pragma once

namespace mplc {

    struct ArchiveConnectionStatus {
        ArchiveConnectionStatus(): connected(false), error(true), code(-1), msg("Unknown Error") {}
        ArchiveConnectionStatus(bool isConnected, bool isError, int statusCode, const std::string& message)
            : connected(isConnected), error(isError), code(statusCode), msg(message) {}
        bool connected;
        bool error;
        int code;
        std::string msg;
    };

}  // namespace mplc
