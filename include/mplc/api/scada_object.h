#pragma once
#include <boost/array.hpp>
#include <mplc/date_time.h>
#include <mplc/meta_fwd.h>
#include <mplc/api/macros_fwd.h>
#include "bit_types.h"
#include "system_params.h"
#include "ref_to.h"

namespace mplc { namespace api {

    struct ScadaObject {
    private:
        template<class T>
        struct system_param final {
            DateTime SourceTime;
            OpcUa_StatusCode StatusCode{};
            T Value;
        };

    public:
        // Typedefs for SCADA Type ALIASING
        using BOOL = bool;
        using DATE = DateTime;
        using DT = DateTime;
        using TIME = TimeSpan;
        using TOD = TimeSpan;
        using REAL = float;
        using LREAL = double;
        using USINT = uint8_t;
        using UINT = uint16_t;
        using UDINT = uint32_t;
        using ULINT = uint64_t;
        using SINT = int8_t;
        using INT = int16_t;
        using DINT = int32_t;
        using LINT = int64_t;
        using BYTE = bit_type<uint8_t>;
        using WORD = bit_type<uint16_t>;
        using DWORD = bit_type<uint32_t>;
        using LWORD = bit_type<uint64_t>;
        using STRING = std::string;
        using VARIANT = OpcUa_VariantHlp;
        using REF_TO = RefTo;
        using WSTRING = std::wstring;
        /*using SYSTEM_BOOL_PARAM = system_param<BOOL>;
        using SYSTEM_BYTE_PARAM = system_param<BYTE>;
        using SYSTEM_DT_PARAM = system_param<DT>;
        using SYSTEM_REAL_PARAM = system_param<REAL>;
        using SYSTEM_LREAL_PARAM = system_param<LREAL>;
        using SYSTEM_INT_PARAM = system_param<INT>;
        using SYSTEM_DINT_PARAM = system_param<DINT>;
        using SYSTEM_LINT_PARAM = system_param<LINT>;
        using SYSTEM_WORD_PARAM = system_param<WORD>;
        using SYSTEM_DWORD_PARAM = system_param<DWORD>;
        using SYSTEM_LWORD_PARAM = system_param<LWORD>;
        using SYSTEM_STRING_PARAM = system_param<STRING>;*/

        template<class T, size_t N>
        struct ARRAY : private boost::array<T, N> {
            using _meta_type = meta::tag::array_t;
            using _base = boost::array<T, N>;
            using value_type = typename _base::value_type;
            using const_iterator = typename _base::const_iterator;
            using const_reference = typename _base::const_reference;
            using difference_type = typename _base::difference_type;
            using iterator = typename _base::iterator;
            using reference = typename _base::reference;
            using size_type = typename _base::size_type;

            // iterator support
            iterator begin() {
                return _base::begin();
            }
            const_iterator begin() const {
                return _base::begin();
            }
            const_iterator cbegin() const {
                return _base::cbegin();
            }

            iterator end() {
                return _base::end();
            }
            const_iterator end() const {
                return _base::end();
            }
            const_iterator cend() const {
                return _base::cend();
            }

            reference operator[](size_type i) {
                return _base::operator[](i);
            }

            const_reference operator[](size_type i) const {
                return _base::operator[](i);
            }

            reference front() {
                return _base::front();
            }

            BOOST_CONSTEXPR const_reference front() const {
                return _base::front();
            }

            reference back() {
                return _base::back();
            }

            BOOST_CONSTEXPR const_reference back() const {
                return _base::back();
            }

            // size is constant
            static BOOST_CONSTEXPR size_type size() {
                return _base::size();
            }
            static BOOST_CONSTEXPR bool empty() {
                return _base::empty();
            }
            static BOOST_CONSTEXPR size_type max_size() {
                return _base::max_size();
            }

            // assign one value to all elements
            void assign(const T& value) {
                fill(value);
            }  // A synonym for fill
            void fill(const T& value) {
                std::fill_n(begin(), size(), value);
            }
        };

        friend struct ApiStorage;
    };

    struct ScadaStruct : ScadaObject {};
}}  // namespace mplc::api
//
//template<class T>
//struct mplc::meta::adapter::Array<std::vector<T>, std::enable_if_t<!boost::is_same<bool, T>::value>> {
//    using type = std::vector<T>;
//    using value_type = T;
//    static type& from(const void* ptr) {
//        return *static_cast<type*>(const_cast<void*>(ptr));
//    }
//    static const void* get(const void* obj, size_t pos) {
//        if (!obj)
//            return nullptr;
//        auto& arr = from(obj);
//        if (arr.size() > pos) {
//            return &arr[pos];
//        }
//        return nullptr;
//    }
//    static size_t size(const void* obj) {
//        if (!obj)
//            return 0;
//        return from(obj).size();
//    }
//    static bool resize(const void* obj, size_t new_size) {
//        if (!obj)
//            return false;
//        from(obj).resize(new_size);
//        return true;
//    }
//};
