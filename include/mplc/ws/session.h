#pragma once
#include "dependency.h"
#include <string>
#include <mplc/libs/threads.hpp>
namespace mplc { namespace ws {
#define WS_EXIT -1
    struct context_t;
    struct session_t : lib::enable_shared_from_this<session_t> {
        MPLCSHARE_API virtual ~session_t();
        MPLCSHARE_API virtual void callback_on_writable();
        MPLCSHARE_API virtual void unsafe_callback_on_writable();
        // MPLCSHARE_API virtual int on_established(lws* _wsi);
        MPLCSHARE_API virtual void close_reason(int reason, unsigned char* buf, size_t len);
        MPLCSHARE_API virtual int write(void* buf, size_t len, int flags);
        MPLCSHARE_API virtual void on_closed(void* in, size_t len);
        MPLCSHARE_API void rx_stop();
        MPLCSHARE_API void rx_start();
        MPLCSHARE_API bool rx_is_stop() const;
        MPLCSHARE_API std::string get_ip() const {
            return m_ip;
        }
        // MPLCSHARE_API void dispatch(lib::function<void()>&& cb);
        size_t tx_size, rx_size;

        bool in_ctx_thread() const;
        template<class T, class... Args>
        static boost::enable_if_t<boost::is_base_of<session_t, T>::value, lib::shared_ptr<T>> make(::lws* wsi,
                                                                                            Args&&... args) {
            session_t::lws tmp;
            tmp.wsi = wsi;
            return lib::make_shared<T>(tmp, std::forward<Args>(args)...);
        }

    protected:
        // Нужно для запрта прямого создания любого экземпляра без вызова make_session
        class lws {
            ::lws* wsi;
            friend session_t;
        };
        template<class T>
        friend boost::enable_if_t<boost::is_base_of<session_t, T>::value, lib::shared_ptr<T>> make_session(::lws* wsi);
        MPLCSHARE_API session_t(lws wsi);

        ::lws* get_wsi() const {
            return m_wsi;
        }

    private:
        lib::atomic<::lws*> m_wsi;
        context_t* m_ctx;
        lib::thread::id m_ctx_thread;
        // mutable lib::spinlock m_spin;
        lib::atomic_flag m_rx_stop;
        std::string m_ip;
        // volatile bool m_rx_stop;
    };

    template<class T>
    boost::enable_if_t<boost::is_base_of<session_t, T>::value, lib::shared_ptr<T>> make_session(::lws* wsi) {
        /*session_t::lws tmp;
        tmp.wsi = wsi;*/
        return session_t::make<T>(wsi);
        //return lib::make_shared<T>(tmp);
    }
    template<class T, class... Args>
    boost::enable_if_t<boost::is_base_of<session_t, T>::value, lib::shared_ptr<T>> make_session(::lws* wsi,
                                                                                                Args&&... args) {
        return session_t::make<T>(wsi, std::forward<Args>(args)...);
    }

}}  // namespace mplc::ws
