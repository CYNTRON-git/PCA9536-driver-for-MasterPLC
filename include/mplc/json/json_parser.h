#pragma once
#include <boost/container/small_vector.hpp>
#include <rapidjson/document.h>

namespace mplc { namespace json {

    struct json_parser {
    private:
        enum next_ret { cont, finish, abort };
        struct elem {
            elem(rapidjson::Value const* p, std::size_t r): rest(r), is_map(false), is_key(false) {
                as.obj_ptr = p;
            }

            elem(rapidjson::Value::ConstMemberIterator p, std::size_t r): rest(r), is_map(true), is_key(true) {
                as.kv_it = p;
            }

            rapidjson::Value const& get() const {
                if (is_map) {
                    if (is_key) {
                        return as.kv_it->name;
                    } else {
                        return as.kv_it->value;
                    }
                } else {
                    return *as.obj_ptr;
                }
            }

            template<typename Visitor>
            next_ret next(Visitor& v) {
                if (rest == 0) {
                    if (is_map) {
                        if (!v.end_map())
                            return abort;
                    } else {
                        if (!v.end_array())
                            return abort;
                    }
                    return finish;
                } else {
                    if (is_map) {
                        if (is_key) {
                            if (!v.end_map_key())
                                return abort;
                            if (!v.start_map_value())
                                return abort;
                            is_key = false;
                        } else {
                            if (!v.end_map_value())
                                return abort;
                            --rest;
                            if (rest == 0) {
                                if (!v.end_map())
                                    return abort;
                                return finish;
                            }
                            if (!v.start_map_key())
                                return abort;
                            ++as.kv_it;
                            is_key = true;
                        }
                    } else {
                        if (!v.end_array_item())
                            return abort;
                        --rest;
                        if (rest == 0) {
                            if (!v.end_array())
                                return abort;
                            return finish;
                        }
                        if (!v.start_array_item())
                            return abort;
                        ++as.obj_ptr;
                    }
                    return cont;
                }
            }

            union {
                rapidjson::Value const* obj_ptr;
                rapidjson::Value::ConstMemberIterator kv_it;
            } as;
            std::size_t rest;
            bool is_map;
            bool is_key;
        };

    public:
        json_parser(const rapidjson::Value& json): m_current(&json) {}
        template<class Visitor>
        void parse(Visitor& v) {
            while (true) {
                bool start_collection = false;
                switch (m_current->GetType()) {
                case rapidjson::kNullType:
                    if (!v.visit_nil())
                        return;
                    break;
                case rapidjson::kFalseType:
                    if (!v.visit_boolean(false))
                        return;
                    break;
                case rapidjson::kTrueType:
                    if (!v.visit_boolean(true))
                        return;
                    break;
                case rapidjson::kObjectType:
                    if (!v.start_map(m_current->MemberCount()))
                        return;
                    if (m_current->MemberCount() != 0) {
                        m_ctx.push_back(elem(m_current->MemberBegin(), m_current->MemberCount()));
                        if (!v.start_map_key())
                            return;
                    } else {
                        if (!v.end_map())
                            return;
                    }
                    break;
                case rapidjson::kArrayType: {
                    if (!v.start_array(m_current->Size()))
                        return;
                    if (m_current->Size() != 0) {
                        m_ctx.push_back(elem(m_current->Begin(), m_current->Size()));
                        if (!v.start_array_item())
                            return;
                    } else {
                        if (!v.end_array())
                            return;
                    }
                    break;
                }
                case rapidjson::kStringType:
                    if (!v.visit_str(m_current->GetString(), m_current->GetStringLength()))
                        return;
                    break;
                case rapidjson::kNumberType:
                    if (m_current->IsDouble()) {
                        if (!v.visit_float64(m_current->GetDouble()))
                            return;
                    } else if (m_current->IsUint64()) {
                        if (!v.visit_uint64(m_current->GetUint64()))
                            return;
                    } else {
                        if (!v.visit_int64(m_current->GetInt64()))
                            return;
                    }
                    break;
                default:;
                }
                if (m_ctx.empty())
                    return;
                if (!start_collection) {
                    while (true) {
                        next_ret r = m_ctx.back().next(v);
                        if (r == finish) {
                            m_ctx.pop_back();
                            if (m_ctx.empty())
                                return;
                        } else if (r == cont) {
                            break;
                        } else {
                            // abort
                            return;
                        }
                    }
                }
                m_current = &m_ctx.back().get();
            }
        }

    private:
        boost::container::small_vector<elem, 8> m_ctx;
        const rapidjson::Value* m_current;
    };

}}  // namespace mplc::json
