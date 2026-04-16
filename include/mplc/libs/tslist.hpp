#pragma once
#include "threads.hpp"
#include <list>
#include <deque>

namespace mplc { namespace lib {
    namespace threadsafe {
        template<template<class, class> class _Container, typename _Ty, typename _Alloc = std::allocator<_Ty>>
        class container {
            // typedef lib::unique_lock<lib::spinlock> m_lock_guard;
            //  using base = tslist<_Ty, _Alloc>;

        public:
            using value_type = _Ty;
            using allocator_type = _Alloc;
            using reference = _Ty&;
            using const_reference = const _Ty&;
            using size_type = size_t;
            using container_type = _Container<_Ty, _Alloc>;

        public:
            container() = default;
            // cv используется не под mutex-ом. TODO потом переделать. 
            container(lib::condition_variable& cv): cv(&cv) {}
            container(const container&) = delete;
            container(container&& lst) noexcept {
                lst.mtx.call([this, &lst] { storage.swap(lst.storage); });
            }
            container& operator=(const container& lst) = delete;
            container& operator=(container&& lst) noexcept {
                auto f_mtx = std::min(&mtx, &lst.mtx);
                auto s_mtx = std::max(&mtx, &lst.mtx);
                lib::unique_lock<spinlock> lock(*f_mtx);
                s_mtx->call([this, &lst] { storage.swap(lst.storage); });
                return *this;
            }
            container(const container_type& lst) {
                storage = lst;
            }
            container(const container_type& lst, lib::condition_variable& cv): cv(&cv) {
                storage = lst;
            }
            container(container_type&& lst) noexcept {
                storage.swap(lst);
            }

            container& operator=(const container_type& lst) {
                mtx.call([this, &lst] { storage = lst; });
                return *this;
            }
            container& operator=(container_type&& lst) noexcept {
                mtx.call([this, &lst] { storage.swap(lst.storage); });
                return *this;
            }

            virtual ~container() {
                // notify_all();
                clear();
            }

        public:
            // Removes and returns item from front of Queue
            bool pop_front(_Ty& item) {
                bool has = false;
                mtx.call([this, &item, &has] {
                    if (!storage.empty()) {
                        item = std::move(storage.front());
                        storage.pop_front();
                        has = true;
                    }
                });
                return has;
            }

            // Removes and returns item from back of Queue
            bool pop_back(_Ty& item) {
                bool has = false;
                mtx.call([this, &item, &has] {
                    if (!storage.empty()) {
                        item = std::move(storage.back());
                        storage.pop_back();
                        has = true;
                    }
                });
                return has;
                // m_lock_guard lock(mtx);
            }
            // Removes and returns item from front of Queue
            _Ty pop_front() {
                _Ty item;
                mtx.call([this, &item] {
                    if (!storage.empty()) {
                        item = std::move(storage.front());
                        storage.pop_front();
                    }
                });
                return item;
            }

            // Removes and returns item from back of Queue
            _Ty pop_back() {
                // m_lock_guard lock(mtx);
                _Ty item;
                mtx.call([this, &item] {
                    if (!storage.empty()) {
                        item = std::move(storage.back());
                        storage.pop_back();
                    }
                });
                return item;
            }

            // Adds an item to back of Queue

            void push_back(const _Ty& item) {
                mtx.call([this, &item] { storage.push_back(item); });
                if (cv) {
                    cv->notify_one();
                }
            }

            // Adds an item to front of Queue

            void push_front(const _Ty& item) {
                mtx.call([this, &item] { storage.push_front(item); });
                if (cv) {
                    cv->notify_one();
                }
            }
            void emplace_back(_Ty&& item) {
                mtx.call([this, &item] { storage.emplace_back(std::move(item)); });
                if (cv) {
                    cv->notify_one();
                }
            }

            // Adds an item to front of Queue

            void emplace_front(_Ty&& item) {
                mtx.call([this, &item] { storage.emplace_front(std::move(item)); });
                if (cv) {
                    cv->notify_one();
                }
            }
            //// Adds an item to back of Queue
            // void emplace_back(_Ty&& item) {
            //     m_lock_guard lock(mtx);
            //     storage.emplace_back(std::move(item));
            //     if (cv) {
            //         cv->notify_one();
            //     }
            // }

            //// Adds an item to front of Queue
            // void emplace_front(_Ty&& item) {
            //     m_lock_guard lock(mtx);
            //     storage.emplace_front(std::move(item));
            //     if (cv) {
            //         cv->notify_one();
            //     }
            // }

            // Returns true if Queue has no items
            bool empty() const {
                bool is_empty{true};
                mtx.call([this, &is_empty] { is_empty = storage.empty(); });
                return is_empty;
            }

            // Returns number of items in Queue
            size_t count() const {
                return size();
            }

            // Returns number of items in Queue
            size_t size() const {
                size_t _size;
                mtx.call([this, &_size] { _size = storage.size(); });
                return _size;
            }

            // Clears Queue
            void clear() {
                container_type lst;
                swap(lst);
            }

            void swap(container_type& lst) {
                mtx.call([this, &lst] { storage.swap(lst); });
            }

            //// Exit when pred() return true;
            // template<typename predicate_type>
            // void wait(predicate_type pred) {
            //     m_lock_guard lock(mtx);
            //     cv.wait(lock, [this, pred] { return m_stop || !storage.empty() || pred(); });
            // }
            // template<typename predicate_type>
            // bool timed_wait(int64_t ft_timeout, predicate_type pred) {
            //     m_lock_guard lock(mtx);
            //     return !m_stop && cv.timed_wait(lock,
            //                                     boost::posix_time::microsec(ft_timeout / FileTime::Microsecond),
            //                                     [this, pred] { return m_stop || !storage.empty() && !pred(); });
            // }
            // bool timed_wait(int64_t ft_timeout) {
            //     m_lock_guard lock(mtx);
            //     return cv.timed_wait(lock, boost::posix_time::microsec(ft_timeout / FileTime::Microsecond), [this] {
            //         return m_stop || !storage.empty();
            //     }) && !m_stop;
            // }
            // void notify_all() noexcept {
            //     m_stop = true;
            //     cv.notify_all();
            // }

        protected:
            container_type storage;
            lib::condition_variable* cv{};
            mutable lib::spinlock mtx;
            // bool m_stop{false};
        };

    }  // namespace threadsafe
    template<class _Ty, typename _Alloc = std::allocator<_Ty>>
    using tslist = threadsafe::container<std::list, _Ty, _Alloc>;

    template<class _Ty, typename _Alloc = std::allocator<_Ty>>
    using ts_vector = threadsafe::container<std::vector, _Ty, _Alloc>;

    template<class _Ty, typename _Alloc = std::allocator<_Ty>>
    using ts_deque = threadsafe::container<std::deque, _Ty, _Alloc>;
}}  // namespace mplc::lib
