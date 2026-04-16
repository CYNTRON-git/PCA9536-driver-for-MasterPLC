#pragma once
#include <vector>
#include <msgpack.hpp>
#include <boost/container/small_vector.hpp>
#include <mplc/libs/string_view.hpp>
#include <share/config.h>

namespace mplc { namespace lua { namespace detail {
    struct create_msgpack_visitor {
        create_msgpack_visitor(msgpack::object::with_zone& oz, bool use_ref = false)
            : z(oz.zone), m_ref(use_ref) {
            m_stack.push_back(&oz);
        }
        /*create_msgpack_visitor(msgpack::object_handle& oh, bool use_ref = false, size_t zone_chank = 1024)
            : oh(oh), m_zone_chank(zone_chank), m_ref(use_ref) {
            m_stack.push_back(&const_cast<msgpack::object&>(oh.get()));
        }*/
        bool visit_nil() {
            msgpack::object* obj = m_stack.back();
            obj->type = msgpack::type::NIL;
            // msgpack::detail::create_object_visitor create(&msgpack::unpacker::default_reference_func);
            return true;
        }
        bool visit_boolean(bool v) {
            msgpack::object* obj = m_stack.back();
            obj->type = msgpack::type::BOOLEAN;
            obj->via.boolean = v;
            return true;
        }
        bool visit_integer(int64_t v) {
            msgpack::object* obj = m_stack.back();
            obj->type = v >= 0 ? msgpack::type::POSITIVE_INTEGER : msgpack::type::NEGATIVE_INTEGER;
            obj->via.u64 = v;
            return true;
        }
        bool visit_float64(double v) {
            msgpack::object* obj = m_stack.back();
            obj->type = msgpack::type::FLOAT64;
            obj->via.f64 = v;
            return true;
        }
        bool visit_str(lib::string_view str) {
            /*if (size > m_limit.str())
                throw msgpack::str_size_overflow("str size overflow");*/
            msgpack::object* obj = m_stack.back();
            obj->type = msgpack::type::STR;
            if (m_ref) {
                obj->via.str.ptr = str.data();
                obj->via.str.size = str.size();
            } else {
                if (!str.empty()) {
                    char* tmp = static_cast<char*>(zone().allocate_align(str.size(), MSGPACK_ZONE_ALIGNOF(char)));
                    str.copy(tmp, str.size());
                    obj->via.str.ptr = tmp;
                    obj->via.str.size = str.size();
                } else {
                    obj->via.str.ptr = MSGPACK_NULLPTR;
                    obj->via.str.size = 0;
                }
            }
            return true;
        }
        MPLCSHARE_API bool visit_ud(void* p, lib::string_view name);

        bool start_array(uint32_t num_elements) {
            msgpack::object* obj = m_stack.back();
            obj->type = msgpack::type::ARRAY;
            obj->via.array.size = num_elements;
            if (num_elements == 0) {
                obj->via.array.ptr = MSGPACK_NULLPTR;
            } else {
                size_t size = num_elements * sizeof(msgpack::object);
                obj->via.array.ptr = static_cast<msgpack::object*>(
                    zone().allocate_align(size, MSGPACK_ZONE_ALIGNOF(msgpack::object)));
            }
            m_stack.push_back(obj->via.array.ptr);
            return true;
        }
        bool start_array_item() {
            return true;
        }
        bool end_array_item() {
            ++m_stack.back();
            return true;
        }
        bool end_array() {
            m_stack.pop_back();
            return true;
        }
        bool start_map(uint32_t num_kv_pairs) {
            msgpack::object* obj = m_stack.back();
            obj->type = msgpack::type::MAP;
            obj->via.map.size = num_kv_pairs;
            if (num_kv_pairs == 0) {
                obj->via.map.ptr = MSGPACK_NULLPTR;
            } else {
                size_t size = num_kv_pairs * sizeof(msgpack::object_kv);
                obj->via.map.ptr = static_cast<msgpack::object_kv*>(
                    zone().allocate_align(size, MSGPACK_ZONE_ALIGNOF(msgpack::object_kv)));
            }
            m_stack.push_back(reinterpret_cast<msgpack::object*>(obj->via.map.ptr));
            return true;
        }
        bool start_map_key() {
            return true;
        }
        bool end_map_key() {
            ++m_stack.back();
            return true;
        }
        bool start_map_value() {
            return true;
        }
        bool end_map_value() {
            ++m_stack.back();
            return true;
        }
        bool end_map() {
            m_stack.pop_back();
            return true;
        }

    private:
        msgpack::zone& zone() {
            /*if (!oh.zone()) {
                oh.zone().reset(new msgpack::zone(m_zone_chank));
            }*/
            return z;
        }
        boost::container::small_vector<msgpack::object*, 4> m_stack;
        msgpack::zone& z;
        bool m_ref{};
    };
}}}  // namespace mplc::lua::detail
