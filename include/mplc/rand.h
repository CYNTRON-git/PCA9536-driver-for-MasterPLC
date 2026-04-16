#pragma once
#include <cstdint>
#include <stddef.h>
#include <share/config.h>
namespace mplc { namespace rand {
    void init();
    MPLCSHARE_API void gen_bytes(void* buf, size_t buf_size);
    MPLCSHARE_API int64_t gen_i64();
}}  // namespace mplc::utils
