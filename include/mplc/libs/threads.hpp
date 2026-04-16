#pragma once
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <share/base_macros.h>
#include <share/config.h>
#include <boost/interprocess/sync/lock_options.hpp>
namespace mplc { namespace lib {
    namespace this_thread {
        MPLCSHARE_API void set_name(const char* thread_name);
        using namespace boost::this_thread;
    }  // namespace this_thread
    using boost::adopt_lock;
    using boost::atomic;
    using boost::atomic_flag;
    using boost::condition_variable;
    using boost::defer_lock;
    using boost::lock;
    using boost::lock_guard;
    using boost::memory_order;
    using boost::memory_order_acquire;
    using boost::memory_order_relaxed;
    using boost::memory_order_release;
    using boost::mutex;
    using boost::recursive_mutex;
    using boost::shared_lock;
    using boost::shared_mutex;
    using boost::thread;
    using boost::unique_lock;
}}  // namespace mplc::lib

#pragma comment(lib, "boost_thread.lib")

namespace mplc { namespace lib {

    // Emplementation from https://rigtorp.se/spinlock/
    struct spinlock {
        lib::atomic<bool> lock_;
        spinlock() {
            lock_.exchange(false);
        }
        void lock() noexcept {
            for (;;) {
                // Optimistically assume the lock is free on the first try
                if (!lock_.exchange(true, lib::memory_order_acquire)) {
                    return;
                }
                // Wait for lock to be released without generating cache misses
                while (lock_.load(lib::memory_order_relaxed)) {
#if defined(BOOST_ARCH_X86_64_AVAILABLE) || defined(BOOST_ARCH_X86_32_AVAILABLE)
                    // Issue X86 PAUSE or ARM YIELD instruction to reduce contention between
                    // hyper-threads
#    ifdef _MSC_VER
                    _mm_pause();
#    else
                    __builtin_ia32_pause();
#    endif
#endif
                }
            }
        }

        bool try_lock() noexcept {
            // First do a relaxed load to check if lock is free in order to prevent
            // unnecessary cache misses if someone does while(!try_lock())
            return !lock_.load(lib::memory_order_relaxed) && !lock_.exchange(true, lib::memory_order_acquire);
        }

        void unlock() noexcept {
            lock_.store(false, lib::memory_order_release);
        }
        template<class T>
        void call(T&& func) {
            lib::unique_lock unique_lock(*this, lib::defer_lock);
            if (unique_lock.try_lock()) {
                func();
            } else {
                unique_lock.lock();
                func();
            }
        }
    };
}}  // namespace mplc::lib
