#pragma once
#include <share/config.h>
#include <mplc/libs/containers.hpp>
#include <mplc/libs/string_view.hpp>
#include <mplc/libs/variant.hpp>
#include <vector>

// #include <share/opcua_variant_hlp.h>

namespace mplc { namespace vm {
    struct VmPath {
        enum UnitType { tIdx, tField };
        struct Unit final {
            Unit(const Unit& lv) {
                *this = lv;
            }
            Unit& operator=(const Unit& lv) {
                if (lv.m_type == _type::tField) {
                    assign(lv.str(), false);
                } else {
                    m_type = lv.m_type;
                    memcpy(&m_data, &lv.m_data, sizeof(m_data));
                    m_len = lv.m_len;
                }
                return *this;
            }
            Unit(Unit&& rv) noexcept {
                std::swap(m_type, rv.m_type);
                std::swap(m_data, rv.m_data);
                std::swap(m_len, rv.m_len);
            }
            Unit& operator=(Unit&& rv) noexcept {
                std::swap(m_type, rv.m_type);
                std::swap(m_data, rv.m_data);
                std::swap(m_len, rv.m_len);
                return *this;
            }
            Unit(uint32_t idx): m_type(_type::tIdx) {
                m_data.m_idx = idx;
            }
            Unit(lib::string_view str, bool ref = false) {
                assign(str, ref);
            }

            ~Unit() {
                if (m_type == _type::tField) {
                    delete[] m_data.m_str;
                }
            }

            UnitType type() const {
                return m_type == _type::tIdx ? UnitType::tIdx : UnitType::tField;
            }

            bool operator==(const Unit& lv) const& {
                if (type() != lv.type()) {
                    return false;
                }
                if (type() == tIdx) {
                    return idx() == lv.idx();
                }
                return str() == lv.str();
            }

            bool operator!=(const Unit& lv) const& {
                return !(*this == lv);
            }
            friend size_t hash_value(const Unit& lv) {
                switch (lv.type()) {
                case tIdx:
                    return boost::hash_value(lv.idx());
                case tField:
                    return hash_value(lv.str());
                default:
                    return 0;
                }
            }
            uint32_t idx() const {
                if (type() == tIdx) {
                    return m_data.m_idx;
                }
                return 0;
            }
            lib::string_view str() const {
                if (type() != tIdx) {
                    return lib::string_view{m_data.m_str, m_len};
                }
                return {};
            }

        private:
            enum class _type : uint8_t { tNone, tIdx, tField, tFieldRef };
            friend VmPath;
            Unit(): m_data{} {}
            MPLCSHARE_API void assign(lib::string_view str, bool ref = false);

            union {
                const char* m_str;
                uint64_t m_idx;
            } m_data;
            uint32_t m_len{0};
            _type m_type{_type::tNone};
        };
        // using unit_t = lib::variant<int32_t, lib::string_view>;
    private:
        std::vector<Unit> m_path;

    public:
        auto begin() -> decltype(m_path.begin()) {
            return m_path.begin();
        }
        auto end() -> decltype(m_path.end()) {
            return m_path.end();
        }
        auto back() const -> decltype(m_path.back()) {
            return m_path.back();
        }
        const Unit& operator[](size_t pos) const {
            return m_path[pos];
        }
        size_t count() const {
            return m_path.size();
        }
        bool empty() const {
            return m_path.empty();
        }
        VmPath& operator/=(lib::string_view field) {
            m_path.emplace_back(Unit(field, true));
            return *this;
        }
        VmPath& operator=(lib::string_view path);

        VmPath& operator/=(int32_t idx) {
            m_path.push_back(idx);
            return *this;
        }
        MPLCSHARE_API std::string to_string() const;
        void clear() {
            m_path.clear();
        }
    };
}}  // namespace mplc::vm
