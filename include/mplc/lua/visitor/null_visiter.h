#pragma once
#include <cstdint>
#include <mplc/libs/string_view.hpp>

namespace mplc { namespace lua {
    struct null_visitor {
        bool visit_nil() {
            return true;
        }
        bool visit_boolean(bool /*v*/) {
            return true;
        }
        bool visit_integer(int64_t /*v*/) {
            return true;
        }
        bool visit_float64(double /*v*/) {
            return true;
        }
        bool visit_str(lib::string_view /*str*/) {
            return true;
        }
        bool visit_ud(void* /*p*/, lib::string_view /*name*/) {
            return true;
        }
        bool start_array(uint32_t /*num_elements*/) {
            return true;
        }
        bool start_array_item() {
            return true;
        }
        bool end_array_item() {
            return true;
        }
        bool end_array() {
            return true;
        }
        bool start_map(uint32_t /*num_kv_pairs*/) {
            return true;
        }
        bool start_map_key() {
            return true;
        }
        bool end_map_key() {
            return true;
        }
        bool start_map_value() {
            return true;
        }
        bool end_map_value() {
            return true;
        }
        bool end_map() {
            return true;
        }
    };
}}  // namespace mplc::lua
