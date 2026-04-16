#pragma once
#include <list>
#include <mplc/deprecated_ws/ws_data.h>
#include <mplc/deprecated_wsapi.h>
#include <mplc/libs/threads.hpp>
#include <mplc/libs/tslist.hpp>

namespace mplc { namespace deprecated_ws {

    struct WsConnect {
        lws* m_hndl;

        explicit WsConnect(lws* hndl): m_hndl(hndl), m_stop(false) {}

        virtual ~WsConnect() {}
        mutable lib::mutex m_mtx;
        void swap_rx(std::list<pWsData>& queue) {
            rx_queue.swap(queue);
        }
        void push_tx(const pWsData& msg) {
            tx_queue.push_back(msg);
            tx_size += msg->size();
            if (m_hndl && tx_queue.size() == 1) {
                lws_callback_on_writable(m_hndl);
            }
        }
        void push_rx(const pWsData& msg) {
            rx_queue.push_back(msg);
        }
        size_t size_tx() const {
            return tx_size;
        }
        bool empty_tx() const {
            return tx_queue.empty();
        }
        bool empty_rx() const {
            return rx_queue.empty();
        }
        MPLC_WS_API virtual int OnReceive(void* in, size_t len);
        MPLC_WS_API int OnWrite();
        void Disconnect() {
            m_stop = true;
            if (m_hndl) {
                lws_callback_on_writable(m_hndl);
            }
        }

        pWsData rx_buf;

    private:
        bool m_stop;
        lib::atomic<size_t> tx_size{0};

        pWsData tx_buf;
        lib::tslist<pWsData> tx_queue;
        lib::tslist<pWsData> rx_queue;
    };

}}  // namespace mplc::deprecated_ws
