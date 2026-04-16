#pragma once
#include <boost/thread/concurrent_queues/sync_priority_queue.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/chrono/time_point.hpp>
#include <algorithm>  // std::min
#include <mplc/time_point.h>

namespace mplc { namespace lib { namespace cuncurent {

    namespace detail {
        // fixme: shouldn't the timepoint be configurable
        template<class T>
        struct scheduled_type {
            typedef T value_type;
            typedef TimePoint time_point;
            T data;
            time_point time;

            BOOST_THREAD_COPYABLE_AND_MOVABLE(scheduled_type)

            scheduled_type(T const& pdata, time_point tp): data(pdata), time(tp) {}
            scheduled_type(T&& pdata, time_point tp) noexcept: data(boost::move(pdata)), time(tp) {}

            scheduled_type(scheduled_type const& other): data(other.data), time(other.time) {}
            scheduled_type& operator=(BOOST_THREAD_COPY_ASSIGN_REF(scheduled_type) other) {
                data = other.data;
                time = other.time;
                return *this;
            }

            scheduled_type(scheduled_type&& other) noexcept: data(boost::move(other.data)), time(other.time) {}
            scheduled_type& operator=(scheduled_type&& other) noexcept {
                data = boost::move(other.data);
                time = other.time;
                return *this;
            }

            bool operator<(const scheduled_type& other) const {
                return this->time > other.time;
            }
        };  // end struct

        template<class Duration>
        boost::chrono::time_point<boost::chrono::steady_clock, Duration> limit_timepoint(
            boost::chrono::time_point<boost::chrono::steady_clock, Duration> const& tp) {
            // Clock == boost::chrono::steady_clock
            return tp;
        }

        template<class Clock, class Duration>
        boost::chrono::time_point<Clock, Duration> limit_timepoint(
            boost::chrono::time_point<Clock, Duration> const& tp) {
            // Clock != boost::chrono::steady_clock
            // The system time may jump while wait_until() is waiting. To compensate for this and time out near
            // the correct time, we limit how long wait_until() can wait before going around the loop again.
            const boost::chrono::time_point<Clock, Duration> tpmax(boost::chrono::time_point_cast<Duration>(
                Clock::now() + boost::chrono::milliseconds(BOOST_THREAD_POLL_INTERVAL_MILLISECONDS)));
            return (std::min)(tp, tpmax);
        }

        inline TimePoint limit_timepoint(TimePoint tp) {
            // The system time may jump while wait_until() is waiting. To compensate for this and time out near
            // the correct time, we limit how long wait_until() can wait before going around the loop again.
            const auto tpmax = TimePoint::now() + TimeSpan::Milliseconds(BOOST_THREAD_POLL_INTERVAL_MILLISECONDS);
            return (std::min)(tp, tpmax);
        }

        inline boost::chrono::steady_clock::time_point convert_to_steady_clock_timepoint(TimePoint tp) {
            return boost::chrono::steady_clock::time_point(boost::chrono::steady_clock::duration(tp.tiks() * 100));
        }

    }  // namespace detail

    template<class T>
    class sync_timed_queue : private boost::sync_priority_queue<detail::scheduled_type<T> > {
        typedef detail::scheduled_type<T> stype;
        typedef boost::sync_priority_queue<stype> super;

    public:
        typedef T value_type;
        typedef typename super::underlying_queue_type underlying_queue_type;
        typedef typename super::size_type size_type;
        typedef typename super::op_status op_status;

        sync_timed_queue(): super(){};
        ~sync_timed_queue() {}
        sync_timed_queue(const sync_timed_queue&) = delete;
        sync_timed_queue& operator=(const sync_timed_queue&) = delete;
        sync_timed_queue(sync_timed_queue&&) noexcept = delete;
        sync_timed_queue& operator=(sync_timed_queue&&) noexcept = delete;

        using super::close;
        using super::closed;
        using super::empty;
        using super::full;
        using super::size;
        using super::underlying_queue;

        T pull();
        void pull(T& elem);
        template<class Duration>
        boost::queue_op_status pull_until(TimePoint tp, T& elem);
        template<class Rep, class Period>
        boost::queue_op_status pull_for(TimeSpan dura, T& elem);

        boost::queue_op_status try_pull(T& elem);
        boost::queue_op_status wait_pull(T& elem);
        boost::queue_op_status nonblocking_pull(T& elem);

        void push(const T& elem, TimePoint tp);
        void push(const T& elem, TimeSpan dura);

        void push(T&& elem, TimePoint tp);
        void push(T&& elem, TimeSpan dura);

        boost::queue_op_status try_push(const T& elem, TimePoint tp);
        boost::queue_op_status try_push(const T& elem, TimeSpan dura);

        boost::queue_op_status try_push(T&& elem, TimePoint tp);

        boost::queue_op_status try_push(T&& elem, TimeSpan dura);
        void reset() {
            underlying_queue_type tmp(underlying_queue());
            boost::lock_guard<boost::mutex> lk(super::mtx_);
            super::closed_ = false;
        }

    private:
        inline bool not_empty_and_time_reached(boost::unique_lock<boost::mutex>& lk) const;
        inline bool not_empty_and_time_reached(boost::lock_guard<boost::mutex>& lk) const;

        bool wait_to_pull(boost::unique_lock<boost::mutex>&);
        boost::queue_op_status wait_to_pull_until(boost::unique_lock<boost::mutex>&, TimePoint tp);

        boost::queue_op_status wait_to_pull_for(boost::unique_lock<boost::mutex>& lk, TimeSpan dura);

        T pull(boost::unique_lock<boost::mutex>&);
        T pull(boost::lock_guard<boost::mutex>&);

        void pull(boost::unique_lock<boost::mutex>&, T& elem);
        void pull(boost::lock_guard<boost::mutex>&, T& elem);

        boost::queue_op_status try_pull(boost::unique_lock<boost::mutex>&, T& elem);
        boost::queue_op_status try_pull(boost::lock_guard<boost::mutex>&, T& elem);

        boost::queue_op_status wait_pull(boost::unique_lock<boost::mutex>& lk, T& elem);

    };  // end class

    template<class T>
    void sync_timed_queue<T>::push(const T& elem, TimePoint tp) {
        super::push(stype(elem, tp));
    }

    template<class T>
    void sync_timed_queue<T>::push(const T& elem, TimeSpan dura) {
        push(elem, TimePoint::now() + dura);
    }

    template<class T>
    void sync_timed_queue<T>::push(T&& elem, TimePoint tp) {
        super::push(stype(boost::move(elem), tp));
    }

    template<class T>
    void sync_timed_queue<T>::push(T&& elem, TimeSpan dura) {
        push(boost::move(elem), TimePoint::now() + dura);
    }

    template<class T>
    boost::queue_op_status sync_timed_queue<T>::try_push(const T& elem, TimePoint tp) {
        return super::try_push(stype(elem, tp));
    }

    template<class T>
    boost::queue_op_status sync_timed_queue<T>::try_push(const T& elem, TimeSpan dura) {
        return try_push(elem, TimePoint::now() + dura);
    }

    template<class T>
    boost::queue_op_status sync_timed_queue<T>::try_push(T&& elem, TimePoint tp) {
        return super::try_push(stype(boost::move(elem), tp));
    }

    template<class T>
    boost::queue_op_status sync_timed_queue<T>::try_push(T&& elem, TimeSpan dura) {
        return try_push(boost::move(elem), TimePoint::now() + dura);
    }

    ///////////////////////////
    template<class T>
    bool sync_timed_queue<T>::not_empty_and_time_reached(boost::unique_lock<boost::mutex>& lk) const {
        return !super::empty(lk) && TimePoint::now() >= super::data_.top().time;
    }

    template<class T>
    bool sync_timed_queue<T>::not_empty_and_time_reached(boost::lock_guard<boost::mutex>& lk) const {
        return !super::empty(lk) && TimePoint::now() >= super::data_.top().time;
    }

    ///////////////////////////
    template<class T>
    bool sync_timed_queue<T>::wait_to_pull(boost::unique_lock<boost::mutex>& lk) {
        for (;;) {
            if (not_empty_and_time_reached(lk))
                return false;  // success
            if (super::closed(lk))
                return true;  // closed

            super::wait_until_not_empty_or_closed(lk);

            if (not_empty_and_time_reached(lk))
                return false;  // success
            if (super::closed(lk))
                return true;  // closed

            const TimePoint tpmin(detail::limit_timepoint(super::data_.top().time));
            super::cond_.wait_until(lk, detail::convert_to_steady_clock_timepoint(tpmin));
        }
    }

    template<class T>
    boost::queue_op_status sync_timed_queue<T>::wait_to_pull_until(boost::unique_lock<boost::mutex>& lk, TimePoint tp) {
        for (;;) {
            if (not_empty_and_time_reached(lk))
                return boost::queue_op_status::success;
            if (super::closed(lk))
                return boost::queue_op_status::closed;
            if (TimePoint::now() >= tp)
                return super::empty(lk) ? boost::queue_op_status::timeout : boost::queue_op_status::not_ready;

            super::wait_until_not_empty_or_closed_until(lk, tp);

            if (not_empty_and_time_reached(lk))
                return boost::queue_op_status::success;
            if (super::closed(lk))
                return boost::queue_op_status::closed;
            if (TimePoint::now() >= tp)
                return super::empty(lk) ? boost::queue_op_status::timeout : boost::queue_op_status::not_ready;

            const TimePoint tpmin((std::min)(tp, detail::limit_timepoint(super::data_.top().time)));
            super::cond_.wait_until(lk, detail::convert_to_steady_clock_timepoint(tpmin));
        }
    }

    template<class T>
    boost::queue_op_status sync_timed_queue<T>::wait_to_pull_for(boost::unique_lock<boost::mutex>& lk, TimeSpan dura) {
        const TimePoint tp(TimePoint::now() + dura);
        for (;;) {
            if (not_empty_and_time_reached(lk))
                return boost::queue_op_status::success;
            if (super::closed(lk))
                return boost::queue_op_status::closed;
            if (TimePoint::now() >= tp)
                return super::empty(lk) ? boost::queue_op_status::timeout : boost::queue_op_status::not_ready;

            super::wait_until_not_empty_or_closed_until(lk, tp);

            if (not_empty_and_time_reached(lk))
                return boost::queue_op_status::success;
            if (super::closed(lk))
                return boost::queue_op_status::closed;
            if (TimePoint::now() >= tp)
                return super::empty(lk) ? boost::queue_op_status::timeout : boost::queue_op_status::not_ready;

            const TimePoint tpmin((std::min)(tp, super::data_.top().time));
            super::cond_.wait_until(lk, detail::convert_to_steady_clock_timepoint(tpmin));
        }
    }

    ///////////////////////////
    template<class T>
    T sync_timed_queue<T>::pull(boost::unique_lock<boost::mutex>&) {
        return boost::move(super::data_.pull().data);
    }

    template<class T>
    T sync_timed_queue<T>::pull(boost::lock_guard<boost::mutex>&) {
        return boost::move(super::data_.pull().data);
    }
    template<class T>
    T sync_timed_queue<T>::pull() {
        boost::unique_lock<boost::mutex> lk(super::mtx_);
        const bool has_been_closed = wait_to_pull(lk);
        if (has_been_closed)
            super::throw_if_closed(lk);
        return pull(lk);
    }

    ///////////////////////////
    template<class T>
    void sync_timed_queue<T>::pull(boost::unique_lock<boost::mutex>&, T& elem) {
        elem = boost::move(super::data_.pull().data);
    }

    template<class T>
    void sync_timed_queue<T>::pull(boost::lock_guard<boost::mutex>&, T& elem) {
        elem = boost::move(super::data_.pull().data);
    }

    template<class T>
    void sync_timed_queue<T>::pull(T& elem) {
        boost::unique_lock<boost::mutex> lk(super::mtx_);
        const bool has_been_closed = wait_to_pull(lk);
        if (has_been_closed)
            super::throw_if_closed(lk);
        pull(lk, elem);
    }

    //////////////////////
    template<class T>
    template<class Duration>
    boost::queue_op_status sync_timed_queue<T>::pull_until(TimePoint tp, T& elem) {
        boost::unique_lock<boost::mutex> lk(super::mtx_);
        const boost::queue_op_status rc = wait_to_pull_until(lk, tp);
        if (rc == boost::queue_op_status::success)
            pull(lk, elem);
        return rc;
    }

    //////////////////////
    template<class T>
    template<class Rep, class Period>
    boost::queue_op_status sync_timed_queue<T>::pull_for(TimeSpan dura, T& elem) {
        boost::unique_lock<boost::mutex> lk(super::mtx_);
        const boost::queue_op_status rc = wait_to_pull_for(lk, dura);
        if (rc == boost::queue_op_status::success)
            pull(lk, elem);
        return rc;
    }

    ///////////////////////////
    template<class T>
    boost::queue_op_status sync_timed_queue<T>::try_pull(boost::unique_lock<boost::mutex>& lk, T& elem) {
        if (not_empty_and_time_reached(lk)) {
            pull(lk, elem);
            return boost::queue_op_status::success;
        }
        if (super::closed(lk))
            return boost::queue_op_status::closed;
        if (super::empty(lk))
            return boost::queue_op_status::empty;
        return boost::queue_op_status::not_ready;
    }
    template<class T>
    boost::queue_op_status sync_timed_queue<T>::try_pull(boost::lock_guard<boost::mutex>& lk, T& elem) {
        if (not_empty_and_time_reached(lk)) {
            pull(lk, elem);
            return boost::queue_op_status::success;
        }
        if (super::closed(lk))
            return boost::queue_op_status::closed;
        if (super::empty(lk))
            return boost::queue_op_status::empty;
        return boost::queue_op_status::not_ready;
    }

    template<class T>
    boost::queue_op_status sync_timed_queue<T>::try_pull(T& elem) {
        boost::lock_guard<boost::mutex> lk(super::mtx_);
        return try_pull(lk, elem);
    }

    ///////////////////////////
    template<class T>
    boost::queue_op_status sync_timed_queue<T>::wait_pull(boost::unique_lock<boost::mutex>& lk, T& elem) {
        const bool has_been_closed = wait_to_pull(lk);
        if (has_been_closed)
            return boost::queue_op_status::closed;
        pull(lk, elem);
        return boost::queue_op_status::success;
    }

    template<class T>
    boost::queue_op_status sync_timed_queue<T>::wait_pull(T& elem) {
        boost::unique_lock<boost::mutex> lk(super::mtx_);
        return wait_pull(lk, elem);
    }

    ///////////////////////////
    template<class T>
    boost::queue_op_status sync_timed_queue<T>::nonblocking_pull(T& elem) {
        boost::unique_lock<boost::mutex> lk(super::mtx_, boost::try_to_lock);
        if (!lk.owns_lock())
            return boost::queue_op_status::busy;
        return try_pull(lk, elem);
    }
}}}  // namespace mplc::lib::cuncurent
