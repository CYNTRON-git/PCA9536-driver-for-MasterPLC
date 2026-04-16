#pragma once

#include <mplc/database.h>
#include "directory_request.h"

namespace mplc {
    struct DirectoryArchive {
        ADD_PTR_TYPEDEF(DirectoryArchive)
        virtual OpcUa_StatusCode exec(DirectoryRequest::ptr req) = 0;
        virtual ~DirectoryArchive() {}
    };
    struct IDirectoryArchiveFactory {
        virtual DirectoryArchive::ptr make(const archive::ConnectionCfg& pool_cfg) = 0;
    };
}  // namespace mplc
