#pragma once

#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/enable_if.hpp>

namespace mplc { namespace api {

    template<class T, class V = void>
    class bit_type {
    public:
    };
    template<class T>
    class bit_type<T, boost::enable_if_t<boost::is_unsigned<T>::value>> {
        T data{};

    public:
        bit_type() = default;
        bit_type(const bit_type& val): data(val.data) {}
        bit_type& operator=(const bit_type& val) {
            data = val.data;
            return *this;
        }
        bit_type(const bit_type&& val) noexcept: data(val.data) {}
        bit_type& operator=(bit_type&& val) noexcept {
            data = val.data;
            return *this;
        }
        bit_type& operator=(T val) {
            data = val;
            return *this;
        }
        bit_type(T val): data(val) {}
        template<size_t N>
        bool get() const {
            static_assert(N < sizeof(T) * 8, "Out of bounds");
            return (data >> N) & 1;
        }
        template<size_t N>
        void set() {
            static_assert(N < sizeof(T) * 8, "Out of bounds");
            data |= 1 << N;
        }
        template<size_t N>
        void unset() {
            static_assert(N < sizeof(T) * 8, "Out of bounds");
            data &= ~(1 << N);
        }
        template<size_t N>
        void toggle() {
            static_assert(N < sizeof(T) * 8, "Out of bounds");
            data ^= 1 << N;
        }

        bool get(size_t N) const {
            return (data >> N) & 1;
        }
        void set(size_t N) {
            data |= 1 << N;
        }
        void unset(size_t N) {
            static_assert(N < sizeof(T) * 8, "Out of bounds");
            data &= ~(1 << N);
        }
        void toggle(size_t N) {
            static_assert(N < sizeof(T) * 8, "Out of bounds");
            data ^= 1 << N;
        }
        static size_t count() {
            return sizeof(T) * 8;
        }
        T raw() const {
            return data;
        }
        operator T() const {
            return data;
        }
    };
}}  // namespace mplc::api
