#pragma once
#include <cstdint>
#include <lua.hpp>
#include <boost/container/small_vector.hpp>
#include <share/config.h>
namespace msgpack {
    struct MPLCSHARE_API lua_wirite_visitor /*: private msgpack::null_visitor */ {
        lua_wirite_visitor(lua_State* L);

        bool visit_nil() const;

        bool visit_boolean(bool v) const;

        bool visit_positive_integer(uint64_t v) const;

        bool visit_negative_integer(int64_t v) const;

        bool visit_float32(float v) const;

        bool visit_float64(double v) const;

        bool visit_str(const char* v, uint32_t size) const;

        bool visit_bin(const char* v, uint32_t size) const;

        bool visit_ext(const char* v, uint32_t size);

        bool start_array(uint32_t num_elements);

        bool start_array_item();

        bool end_array_item();

        bool end_array();

        bool start_map(uint32_t num_kv_pairs);

        bool start_map_key();

        bool end_map_key();

        bool start_map_value();

        bool end_map_value();

        bool end_map();

        bool is_ok() const;

    private:
        boost::container::small_vector<int32_t, 4>& m_stack();
        bool m_is_ok{true};
        lua_State* L;
        char buf[sizeof(boost::container::small_vector<int32_t, 4>)];
    };

}  // namespace msgpack
