#pragma once
#include "includes.h"
#include "communic/netlib.h"
#include <share/mplcshare.h>

namespace mplc {

    class endpoint {
    protected:
        NLAddress address;
        int port;
        bool m_status;
        int64_t controller_id;
        friend class socket;

    public:
        MPLCSHARE_API const char* get_ip(char* buf, size_t buf_size) const;
        MPLCSHARE_API endpoint();
        // MPLCSHARE_API endpoint(int64_t controller_id);
        MPLCSHARE_API void create(int64_t controller_id);
        MPLCSHARE_API void create(TCHAR* ip, int port);
        MPLCSHARE_API bool exist() const;
        MPLCSHARE_API void change();
    };
    class reserv_ep : public endpoint {
        DWORD current_ip;

    public:
        MPLCSHARE_API reserv_ep();
        MPLCSHARE_API void update();
        MPLCSHARE_API static reserv_ep current();
    };

    class socket {
        NLSocket m_socket;
        WORD trans_id;
        int timeout;

    public:
        MPLCSHARE_API socket();
        MPLCSHARE_API bool reconect();
        MPLCSHARE_API bool is_open() const;
        MPLCSHARE_API void set_timeout(int timeout);

        MPLCSHARE_API int send(endpoint& ep, std::vector<BYTE>& request, std::vector<BYTE>& responce);
        MPLCSHARE_API int send(endpoint& ep, BYTE* out, size_t out_s, BYTE* in, size_t in_s);
        MPLCSHARE_API ~socket();
    };

    class SocketStream : public SimpleOStream {
    public:
        typedef char Ch;  //!< Character type. Only support char.

        MPLCSHARE_API SocketStream(std::vector<BYTE>& buffer);

        MPLCSHARE_API void Put(char c);
        size_t Size() const {
            return buffer.size();
        }
        MPLCSHARE_API void PutN(char c, size_t n);

        MPLCSHARE_API void Flush();

    private:
        std::vector<uint8_t>& buffer;
    };

    class JsonRequest : public JsonTextStream<SocketStream>::type {
        // 43 - Request data begin
        std::vector<uint8_t> buffer;
        std::string method;
        SocketStream os;
        socket& sock;
        size_t start;
        uint16_t trans_id;
        uint8_t responce[1024 * 1024 * 4];
        size_t res_limit;

    public:
        MPLCSHARE_API JsonRequest(socket& sock, const std::string& method, bool use_udp2 = false);

        MPLCSHARE_API void clear();

        MPLCSHARE_API OpcUa_StatusCode exec(endpoint& ep, Document& doc, bool& no_response);
    };
}  // namespace mplc
