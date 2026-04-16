/*

The MIT License (MIT)

Copyright (c) 2015 Ievgen Polyvanyi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include <list>
// #include <memory>
#include <mutex>
#include <vector>
// #include <functional>
#include <forward_list>
#include <mplc/libs/threads.hpp>
#include <mplc/libs/bind.hpp>
#include <mplc/libs/smart_ptr.hpp>
namespace mplc { namespace signal {
    template<int>
    struct placeholder_lsignal {};
}}  // namespace mplc::signal

template<int N>
struct std::is_placeholder<mplc::signal::placeholder_lsignal<N>> : integral_constant<int, N + 1> {};
template<int N>
struct boost::is_placeholder<mplc::signal::placeholder_lsignal<N>> : integral_constant<int, N + 1> {};

namespace mplc { namespace signal {
    // std::integer_sequence for C++11

    template<int... Ns>
    struct int_sequence {};

    template<int N, int... Ns>
    struct make_int_sequence : make_int_sequence<N - 1, N - 1, Ns...> {};

    template<int... Ns>
    struct make_int_sequence<0, Ns...> : int_sequence<Ns...> {};

    // connection

    struct connection_data {
        lib::atomic_flag locked;

        typedef boost::intrusive_ptr<connection_data> ptr;
        static ptr make() {
            return boost::intrusive_ptr<connection_data>(new connection_data());
        }

    private:
        mutable lib::atomic<int> refcount_;
        friend void intrusive_ptr_add_ref(const connection_data* x) {
            x->refcount_.fetch_add(1, boost::memory_order_relaxed);
        }
        friend void intrusive_ptr_release(const connection_data* x) {
            if (x->refcount_.fetch_sub(1, boost::memory_order_release) == 1) {
                boost::atomic_thread_fence(boost::memory_order_acquire);
                delete x;
            }
        }
    };

    struct connection_cleaner {
        lib::function<void(connection_data::ptr)> deleter;
        connection_data::ptr data;
    };

    class connection {
        template<typename>
        friend class signal;

    public:
        connection(const connection& data): _data(data._data) {}
        connection& operator=(const connection& data) {
            _data = data._data;
            return *this;
        }
        connection(): _data(nullptr) {}
        connection(connection_data::ptr&& data);
        connection(connection&& data) noexcept;
        connection& operator=(connection&& data) noexcept;
        virtual ~connection();

        bool is_locked() const;
        void set_lock(bool lock);
        operator bool() const {
            return _data != nullptr;
        }
        // void disconnect();

        friend size_t hash_value(const connection& v) {
            return hash_value(v._data);
        }
        friend bool operator==(const connection& l, const connection& r) {
            return l._data == r._data;
        }

    private:
        connection_data::ptr _data;
        // std::vector<connection_cleaner> _cleaners;
    };
    inline connection::connection(connection&& con) noexcept
        : _data(std::move(con._data)) /*, _cleaners(std::move(con._cleaners))*/ {}
    inline connection& connection::operator=(connection&& con) noexcept {
        _data.swap(con._data);
        /*_cleaners.swap(con._cleaners);*/
        return *this;
    }
    inline connection::connection(connection_data::ptr&& data): _data(std::move(data)) {}

    inline connection::~connection() {}

    inline bool connection::is_locked() const {
        return _data && _data->locked.test();
    }

    inline void connection::set_lock(bool lock) {
        if (_data) {
            if (lock) {
                _data->locked.test_and_set();
            } else {
                _data->locked.clear();
            }
        }
    }

    /*inline void connection::disconnect() {
    decltype(_cleaners) cleaners = _cleaners;

    for (auto iter = cleaners.cbegin(); iter != cleaners.cend(); ++iter) {
        const connection_cleaner& cleaner = *iter;

        cleaner.deleter(cleaner.data);
    }
    }
    */
    // slot

    class slot : public connection {
        template<typename>
        friend class signal;
        std::vector<connection_cleaner> _cleaners;

    public:
        slot();
        slot(slot&& rv) noexcept;
        slot& operator=(slot&& rv) noexcept;
        ~slot() override;
        void disconnect();
    };

    inline slot::slot(): connection(connection_data::ptr()) {}

    inline slot::slot(slot&& rv) noexcept: connection(std::move(rv)) /*, _cleaners(std::move(con._cleaners))*/ {}

    inline slot& slot::operator=(slot&& rv) noexcept {
        connection::operator=(std::move(rv));
        return *this;
    }

    inline void slot::disconnect() {
        decltype(_cleaners) cleaners = _cleaners;

        for (auto iter = cleaners.cbegin(); iter != cleaners.cend(); ++iter) {
            const connection_cleaner& cleaner = *iter;

            cleaner.deleter(cleaner.data);
        }
    }

    inline slot::~slot() {
        disconnect();
    }

    // signal

    template<typename>
    class signal;

    template<typename R, typename... Args>
    class signal<R(Args...)> {
    public:
        using result_type = R;
        using callback_type = lib::function<R(Args...)>;

        signal();
        ~signal();

        signal(const signal& rhs);
        signal& operator=(const signal& rhs);

        signal(signal&& rhs) noexcept;
        signal& operator=(signal&& rhs) noexcept;

        bool is_locked() const;
        void set_lock(const bool lock);

        void connect(signal* sg);
        void disconnect(signal* sg);

        connection connect(const callback_type& fn, slot* owner = nullptr, bool locked = false);
        connection connect(callback_type&& fn, slot* owner = nullptr, bool locked = false);

        template<typename T, typename U>
        connection connect(T* p, const U& fn, slot* owner = nullptr, bool locked = false);

        void disconnect(const connection& connection);
        void disconnect(slot* owner);

        void disconnect_all();

        bool empty() const;

        void operator()(Args... args);

        template<typename T>
        R operator()(Args... args, const T& agg);

    private:
        struct joint {
            callback_type callback{};
            connection_data::ptr connection;
            slot* owner{};
        };

        mutable lib::mutex _mutex;
        std::forward_list<joint> _callbacks;
        std::forward_list<signal*> _children;
        signal* _parent;
        bool _locked;

        template<typename T, typename U, int... Ns>
        callback_type construct_mem_fn(const T& fn, U* p, int_sequence<Ns...>) const;

        void copy_callbacks(const std::forward_list<joint>& callbacks);

        connection_data::ptr create_connection(callback_type&& fn, slot* owner, bool locked = false);
        void destroy_connection(connection_data::ptr connection);

        // connection prepare_connection(connection&& conn);
    };

    template<typename R, typename... Args>
    signal<R(Args...)>::signal(): _locked(false), _parent(nullptr) {}

    template<typename R, typename... Args>
    signal<R(Args...)>::~signal() {
        lib::lock_guard<lib::mutex> locker(_mutex);

        for (const joint& jnt: _callbacks) {
            if (jnt.owner != nullptr) {
                jnt.owner->_data = nullptr;
                jnt.owner->_cleaners.clear();
            }
        }

        if (_parent != nullptr) {
            _parent->_children.remove(this);
        }

        for (signal* sig: _children) {
            sig->_parent = nullptr;
        }
    }

    template<typename R, typename... Args>
    signal<R(Args...)>::signal(const signal& rhs): _locked(rhs._locked), _parent(nullptr) {
        lib::lock(_mutex, rhs._mutex);
        lib::lock_guard<lib::mutex> locker_own(_mutex, lib::adopt_lock);
        lib::lock_guard<lib::mutex> locker_sg(rhs._mutex, lib::adopt_lock);

        copy_callbacks(rhs._callbacks);
    }

    template<typename R, typename... Args>
    signal<R(Args...)>& signal<R(Args...)>::operator=(const signal& rhs) {
        lib::lock(_mutex, rhs._mutex);
        lib::lock_guard<lib::mutex> locker_own(_mutex, lib::adopt_lock);
        lib::lock_guard<lib::mutex> locker_sg(rhs._mutex, lib::adopt_lock);

        _locked = rhs._locked;
        _parent = nullptr;

        copy_callbacks(rhs._callbacks);

        return *this;
    }

    template<typename R, typename... Args>
    signal<R(Args...)>::signal(signal&& rhs) noexcept: _locked(rhs._locked), _parent(rhs._parent) {
        lib::lock(_mutex, rhs._mutex);
        lib::lock_guard<lib::mutex> locker_own(_mutex, lib::adopt_lock);
        lib::lock_guard<lib::mutex> locker_sg(rhs._mutex, lib::adopt_lock);

        std::swap(_callbacks, rhs._callbacks);
    }

    template<typename R, typename... Args>
    signal<R(Args...)>& signal<R(Args...)>::operator=(signal&& rhs) noexcept {
        lib::lock(_mutex, rhs._mutex);
        lib::lock_guard<lib::mutex> locker_own(_mutex, lib::adopt_lock);
        lib::lock_guard<lib::mutex> locker_sg(rhs._mutex, lib::adopt_lock);

        _locked = rhs._locked;
        _parent = rhs._parent;

        std::swap(_callbacks, rhs._callbacks);

        return *this;
    }

    template<typename R, typename... Args>
    bool signal<R(Args...)>::is_locked() const {
        return _locked;
    }

    template<typename R, typename... Args>
    void signal<R(Args...)>::set_lock(const bool lock) {
        _locked = lock;
    }

    template<typename R, typename... Args>
    void signal<R(Args...)>::connect(signal* sg) {
        lib::lock(_mutex, sg->_mutex);
        lib::lock_guard<lib::mutex> locker_own(_mutex, lib::adopt_lock);
        lib::lock_guard<lib::mutex> locker_sg(sg->_mutex, lib::adopt_lock);

        if (_parent == sg) {
            return;
        }

        auto iter = std::find(_children.cbegin(), _children.cend(), sg);

        if (iter == _children.cend()) {
            sg->_parent = this;

            _children.emplace_front(std::move(sg));
        }
    }

    template<typename R, typename... Args>
    void signal<R(Args...)>::disconnect(signal* sg) {
        lib::lock_guard<lib::mutex> locker(_mutex);

        _children.remove(sg);
    }

    template<typename R, typename... Args>
    connection signal<R(Args...)>::connect(const callback_type& fn, slot* owner, bool locked) {
        return create_connection(static_cast<callback_type>(fn), owner, locked);
    }

    template<typename R, typename... Args>
    connection signal<R(Args...)>::connect(callback_type&& fn, slot* owner, bool locked) {
        return create_connection(std::move(fn), owner, locked);
    }

    template<typename R, typename... Args>
    template<typename T, typename U>
    connection signal<R(Args...)>::connect(T* p, const U& fn, slot* owner, bool locked) {
        auto mem_fn = std::move(construct_mem_fn(fn, p, make_int_sequence<sizeof...(Args)>{}));

        return create_connection(std::move(mem_fn), owner, locked);
    }

    template<typename R, typename... Args>
    void signal<R(Args...)>::disconnect(const connection& connection) {
        destroy_connection(connection._data);
    }

    template<typename R, typename... Args>
    void signal<R(Args...)>::disconnect(slot* owner) {
        if (owner != nullptr) {
            destroy_connection(owner->_data);
        }
    }
    template<typename R, typename... Args>
    bool signal<R(Args...)>::empty() const {
        lib::lock_guard<lib::mutex> locker(_mutex);
        return _callbacks.empty() && _children.empty();
    }
    template<typename R, typename... Args>
    void signal<R(Args...)>::disconnect_all() {
        lib::lock_guard<lib::mutex> locker(_mutex);

        for (const auto& jnt: _callbacks) {
            if (jnt.owner != nullptr) {
                jnt.owner->_data = nullptr;
                jnt.owner->_cleaners.clear();
            }
        }
        _callbacks.clear();
        for (auto sig: _children) {
            if (sig->_parent == this)  // should be an assert
            {
                sig->_parent = nullptr;
            }
        }
        _children.clear();
    }

    template<typename R, typename... Args>
    void signal<R(Args...)>::operator()(Args... args) {
        lib::lock_guard<lib::mutex> locker(_mutex);

        if (!_locked) {
            for (signal* sig: _children) {
                sig->operator()(std::forward<Args>(args)...);
            }

            for (const joint& jnt: _callbacks) {
                if (!jnt.connection->locked.test()) {
                    /*if (std::next(iter, 1) == _callbacks.cend()) {
                        return jnt.callback(std::forward<Args>(args)...);
                    }*/

                    jnt.callback(std::forward<Args>(args)...);
                }
            }
        }

        // return R();
    }

    template<typename R, typename... Args>
    template<typename T>
    R signal<R(Args...)>::operator()(Args... args, const T& agg) {
        std::vector<R> result;

        lib::lock_guard<lib::mutex> locker(_mutex);

        if (!_locked) {
            for (signal* sig: _children) {
                result.push_back(sig->operator()(std::forward<Args>(args)..., agg));
            }

            // result.reserve(_callbacks.size());

            for (auto&& jnt: _callbacks) {
                if (!jnt.connection->locked.test()) {
                    result.push_back(std::move(jnt.callback(std::forward<Args>(args)...)));
                }
            }
        }

        return agg(std::move(result));
    }

    template<typename R, typename... Args>
    template<typename T, typename U, int... Ns>
    typename signal<R(Args...)>::callback_type signal<R(Args...)>::construct_mem_fn(const T& fn,
                                                                                    U* p,
                                                                                    int_sequence<Ns...>) const {
        return lib::bind(fn, p, placeholder_lsignal<Ns>{}...);
    }

    template<typename R, typename... Args>
    void signal<R(Args...)>::copy_callbacks(const std::forward_list<joint>& callbacks) {
        for (const joint& jn: callbacks) {
            if (jn.owner == nullptr) {
                joint jnt;

                jnt.callback = jn.callback;
                jnt.connection = jn.connection;
                jnt.owner = nullptr;

                _callbacks.emplace_front(std::move(jnt));
            }
        }
    }

    template<typename R, typename... Args>
    connection_data::ptr signal<R(Args...)>::create_connection(callback_type&& fn, slot* owner, bool locked) {
        auto connection = connection_data::make();
        if (locked) {
            connection->locked.test_and_set();
        }
        if (owner != nullptr) {
            auto deleter = [this](connection_data::ptr connection) { destroy_connection(connection); };

            connection_cleaner cleaner;

            cleaner.deleter = deleter;
            cleaner.data = connection;

            owner->_data = connection;
            owner->_cleaners.emplace_back(cleaner);
        }

        joint jnt;

        jnt.callback = std::move(fn);
        jnt.connection = connection;
        jnt.owner = owner;

        lib::lock_guard<lib::mutex> locker(_mutex);

        _callbacks.emplace_front(std::move(jnt));

        return connection;
    }

    template<typename R, typename... Args>
    void signal<R(Args...)>::destroy_connection(connection_data::ptr connection) {
        lib::lock_guard<lib::mutex> locker(_mutex);
        _callbacks.remove_if([&connection](const joint& jnt) {
            if (jnt.connection == connection) {
                if (jnt.owner != nullptr) {
                    jnt.owner->_data = nullptr;
                    jnt.owner->_cleaners.clear();
                }
                return true;
            }
            return false;
        });
    }
}}  // namespace mplc::signal
