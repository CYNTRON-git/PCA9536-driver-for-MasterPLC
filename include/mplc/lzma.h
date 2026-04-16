#pragma once

#ifndef MPLC_LZMA_API
#    if defined(_WIN32) || defined(_WIN64)
#        ifdef MPLC_LZMA_EXPORTS
#            define MPLC_LZMA_API __declspec(dllexport)
#        else
#            define MPLC_LZMA_API __declspec(dllimport)
#        endif
#    else
#        define MPLC_LZMA_API
#    endif
#endif

#include <vector>
#include <mplc/config.hpp>

namespace mplc { namespace lzma {

    class blob {
        typedef lib::shared_ptr<blob> ptr;
        blob(): m_data(nullptr), m_size(0) {}
        MPLC_LZMA_API static ptr compress(const uint8_t* input, uint32_t inputSize);
        MPLC_LZMA_API ptr decompress();
        uint8_t* data() { return m_data; }
        size_t size() { return m_size; }

    private:
        MPLC_LZMA_API int alloc(size_t size);
        MPLC_LZMA_API int free();
        uint8_t header[5];
        size_t m_size;
        size_t m_raw_size;
        uint8_t* m_data;
    };
    MPLC_LZMA_API size_t compress(const uint8_t* input, uint32_t inputSize, uint8_t*& result);
    MPLC_LZMA_API int decompress(const uint8_t* input, uint32_t inputSize, std::vector<uint8_t>& result);
}}  // namespace mplc::lzma
