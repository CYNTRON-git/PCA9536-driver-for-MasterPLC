#pragma once

#include <mplc_stdint.h>

#ifndef MPLC_ZLIB_API
#    ifdef _WIN32
#        ifdef MPLC_ZLIB_EXPORTS
#            define MPLC_ZLIB_API __declspec(dllexport)
#        else
#            define MPLC_ZLIB_API __declspec(dllimport)
#        endif
#    else
#        define MPLC_ZLIB_API
#    endif
#endif

namespace mplc { namespace zlib {
    namespace compression_level {
        enum { no = 0, fast = 1, best = 9, uber = 10, default_ = -1 };
    }  // namespace compression_level
    MPLC_ZLIB_API int compress(const uint8_t* src, size_t src_len, uint8_t*& dst, size_t& dst_len);
    MPLC_ZLIB_API int decompress(const uint8_t* src, size_t src_len, uint8_t*& dst, size_t& dst_len);
}}  // namespace mplc::zlib
