#pragma once
namespace mplc { namespace archive {
    struct TableSizeInfo {
        std::string name;
        int64_t rows;
        int64_t data;
        TableSizeInfo(): rows(0), data(0) {};
    };

}}  // namespace mplc::archive
