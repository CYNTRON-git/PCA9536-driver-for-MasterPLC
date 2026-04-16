#pragma once

#include <iterator>
#include <libwebsockets.h>
#include <boost/container/vector.hpp>
#include <mplc/libs/smart_ptr.hpp>

namespace mplc { namespace lib {
    using boost::container::vector;
}}  // namespace mplc::lib

namespace mplc { namespace deprecated_ws {
    class WsData {
    public:
        WsData(): binary(false), m_data(LWS_PRE, 0), m_pos(0), m_final(false) {}
        bool final() const {
            return m_final;
        }
        int pos() const {
            return m_pos;
        }
        void insert(const char* data, size_t len) {
            m_data.insert(m_data.end(), data, data + len);
        }
        bool finished() const {
            return pos() >= size();
        }
        unsigned char* data() {
            return m_data.data() + LWS_PRE;
        }
        size_t size() const {
            return m_data.size() - LWS_PRE;
        }
        int full_write(lws* wsi);
        int write(lws* wsi);
        void read(void* in, size_t len, bool is_final, bool is_binary) {
            insert((const char*)in, len);
            m_final = is_final;
            binary = is_binary;
            if (m_final && !binary) {
                m_data.push_back(0);
            }
        }
        lib::vector<unsigned char>& vector() {
            return m_data;
        }
        bool binary;

    protected:
        static const size_t mtu_size = 1024;
        lib::vector<unsigned char> m_data;
        int m_pos;
        bool m_final;
    };
    typedef lib::shared_ptr<WsData> pWsData;
}}  // namespace mplc::ws
