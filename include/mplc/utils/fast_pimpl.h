#pragma once
#include <utility>
#include <mplc_stdint.h>
//#define DllExport __declspec(dllexport)

namespace mplc { namespace utils {
    template<class T, std::size_t Size, std::size_t Alignment, bool Strict = false>
    class fast_pimpl_t final {
    public:
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,performance-noexcept-move-constructor)
        fast_pimpl_t(fast_pimpl_t&& v) noexcept(noexcept(T(std::declval<T>()))): fast_pimpl_t(std::move(*v)) {}

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
        fast_pimpl_t(const fast_pimpl_t& v) noexcept(noexcept(T(std::declval<const T&>()))): fast_pimpl_t(*v) {}

        // NOLINTNEXTLINE(bugprone-unhandled-self-assignment,cert-oop54-cpp)
        fast_pimpl_t& operator=(const fast_pimpl_t& rhs) noexcept(
            noexcept(std::declval<T&>() = std::declval<const T&>())) {
            *AsHeld() = *rhs;
            return *this;
        }

        fast_pimpl_t& operator=(fast_pimpl_t&& rhs) noexcept(
            // NOLINTNEXTLINE(performance-noexcept-move-constructor)
            noexcept(std::declval<T&>() = std::declval<T>())) {
            *AsHeld() = std::move(*rhs);
            return *this;
        }

        template<typename... Args>
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
        explicit fast_pimpl_t(Args&&... args) noexcept(noexcept(T(std::declval<Args>()...))) {
            ::new (AsHeld()) T(std::forward<Args>(args)...);
        }

        T* operator->() noexcept {
            return AsHeld();
        }

        const T* operator->() const noexcept {
            return AsHeld();
        }

        T& operator*() noexcept {
            return *AsHeld();
        }

        const T& operator*() const noexcept {
            return *AsHeld();
        }

        ~fast_pimpl_t() noexcept {
            Validate<sizeof(T), alignof(T)>();
            AsHeld()->~T();
        }

    private:
        // Use a template to make actual sizes visible in the compiler error message.
        template<std::size_t ActualSize, std::size_t ActualAlignment>
        static void Validate() noexcept {
            static_assert(Size >= ActualSize, "invalid Size: Size >= sizeof(T) failed");
            static_assert(!Strict || Size == ActualSize, "invalid Size: Size == sizeof(T) failed");

            static_assert(Alignment % ActualAlignment == 0, "invalid Alignment: Alignment % alignof(T) == 0 failed");
            static_assert(!Strict || Alignment == ActualAlignment, "invalid Alignment: Alignment == alignof(T) failed");
        }

        alignas(Alignment) uint8_t storage_[Size];

        T* AsHeld() noexcept {
            return reinterpret_cast<T*>(&storage_);
        }

        const T* AsHeld() const noexcept {
            return reinterpret_cast<const T*>(&storage_);
        }
    };
}}  // namespace mplc::utils
