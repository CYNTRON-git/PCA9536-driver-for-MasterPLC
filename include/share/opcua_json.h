#pragma once
#include <map>
#include <list>
#include <boost/core/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/make_void.hpp>
#include <rapidjson/document.h>
#include <share/filetime.h>
#include <mplc/libs/containers.hpp>
#include <mplc/libs/string_view.hpp>
#include <msgpack/object_fwd.hpp>

namespace GetValueFlags {
    enum GetValueFlagsEnum { None = 0, ReadTimeAsLinuxTime = 0x1 };
}
class BaseLuaObj;
namespace mplc {

    struct JsonWrapper {
        rapidjson::Value& value;
        rapidjson::Value::AllocatorType& allocator;
        GetValueFlags::GetValueFlagsEnum flags;
        JsonWrapper(rapidjson::Value& v,
                    rapidjson::Value::AllocatorType& a,
                    GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::None)
            : value(v), allocator(a), flags(flags) {}
    };

    using rapidjson::Value;
    inline Value& operator<<(Value& os, int64_t v) {
        os.SetInt64(v);
        return os;
    }

    inline Value& operator<<(Value& os, int v) {
        os.SetInt(v);
        return os;
    }

    inline Value& operator<<(Value& os, double v) {
        os.SetDouble(v);
        return os;
    }

    inline Value& operator<<(Value& os, bool v) {
        os.SetBool(v);
        return os;
    }

    inline Value& operator<<(Value& os, unsigned v) {
        os.SetUint(v);
        return os;
    }

    inline Value& operator<<(Value& os, uint64_t v) {
        os.SetUint64(v);
        return os;
    }

    namespace __detail {
        template<typename, typename = void>
        struct is_disable_json : boost::false_type {};
        template<typename T>
        struct is_disable_json<T, typename boost::make_void<typename T::is_disable_json>::type> : boost::true_type {};

    }  // namespace __detail

    template<class T>
    typename boost::enable_if<__detail::is_disable_json<T>, void>::type set_json_data(JsonWrapper& json, const T& v) {}

    template<class T>
    typename boost::enable_if<boost::is_same<FileTime, T>, void>::type set_json_data(JsonWrapper& json, T v) {
        if (json.flags & GetValueFlags::ReadTimeAsLinuxTime) {
            json.value.SetInt64(v.unix_ms());
        } else {
            json.value.SetInt64(v.dt());
        }
    }

    template<class T>
    typename boost::enable_if<boost::is_pointer<T>, void>::type set_json_data(JsonWrapper& json, T v) {
        if (v == nullptr) {
            json.value.SetNull();
        } else {
            set_json_data(json, *v);
        }
    }
    template<class T>
    typename boost::enable_if<boost::is_integral<T>, void>::type set_json_data(JsonWrapper& json, T v) {
        json.value << v;
    }
    template<class T>
    typename boost::enable_if<boost::is_floating_point<T>, void>::type set_json_data(JsonWrapper& json, T v) {
        json.value << v;
    }
    template<class T>
    typename boost::enable_if<boost::is_enum<T>, void>::type set_json_data(JsonWrapper& json, T v) {
        json.value.SetInt(v);
    }
    MPLCSHARE_API void set_json_data(JsonWrapper& json, const OpcUa_VariantHlp& v);

    template<class T>
    void set_json_kv(JsonWrapper& json, const std::string& key, const T& val) {
        Value js_key, js_val;
        js_key.SetString(key, json.allocator);
        JsonWrapper wrapper(js_val, json.allocator);
        set_json_data(wrapper, val);
        json.value.AddMember(js_key, js_val, json.allocator);
    }

    template<class T>
    void set_json_data(JsonWrapper& json, const std::map<std::string, T>& val) {
        json.value.SetObject();
        for (typename std::map<std::string, T>::const_iterator it = val.begin(); it != val.end(); ++it) {
            set_json_kv(json, it->first, it->second);
        }
    }

    inline void set_json_data(JsonWrapper& json, const std::string& v) {
        json.value.SetString(v.c_str(), v.size(), json.allocator);
    }
    inline void set_json_data(JsonWrapper& json, const char* v, int size) {
        json.value.SetString(v ? v : "", size, json.allocator);
    }

    template<class It>
    void set_json_array(JsonWrapper& json, It begin, It end) {
        if (!json.value.IsArray())
            json.value.SetArray();
        for (; begin != end; ++begin) {
            Value val;
            JsonWrapper wrapper(val, json.allocator);
            set_json_data(wrapper, *begin);
            json.value.PushBack(val, json.allocator);
        }
    }
    inline void set_json_data(JsonWrapper& json, const std::vector<bool>& v) {
        if (!json.value.IsArray())
            json.value.SetArray();
        for (size_t i = 0; i != v.size(); ++i) {
            Value val;
            JsonWrapper wrapper(val, json.allocator);
            set_json_data(wrapper, (bool)v[i]);
            json.value.PushBack(val, json.allocator);
        }
    }
    template<class T>
    void set_json_data(JsonWrapper& json, const std::vector<T>& v) {
        set_json_array(json, v.begin(), v.end());
    }
    template<class T, class Alloc>
    void set_json_data(JsonWrapper& json, const std::list<T>& v) {
        set_json_array(json, v.begin(), v.end());
    }

    template<class Type>
    class Cpp2CProxy {
        Cpp2CProxy();
        ~Cpp2CProxy();

    public:
        static OpcUa_Void Initialize(OpcUa_Void** value) {
            *value = new Type();
        }

        static OpcUa_Void Clear(OpcUa_Void* value) {
            delete static_cast<Type*>(value);
        }

        static OpcUa_Int Compare(const OpcUa_Void* value1, const OpcUa_Void* value2) {
            const Type* left = static_cast<const Type*>(value1);
            const Type* right = static_cast<const Type*>(value2);
            if (left == right) {
                return OPCUA_EQUAL;
            }
            if (left == nullptr || right == nullptr) {
                return 1;
            }
            return *left == *right ? 0 : 1;
        }

        static OpcUa_StatusCode CopyTo(const OpcUa_Void* source, OpcUa_Void** destination) {
            const Type* src = static_cast<const Type*>(source);
            if (*destination == nullptr) {
                Initialize(destination);
            }
            Type* dst = static_cast<Type*>(*destination);
            if (src == dst) {
                return OpcUa_Good;
            }
            *dst = *src;
            return OpcUa_Good;
        }

        static OpcUa_StatusCode Concatenate(const OpcUa_Void* source, OpcUa_Void** destination) {
            const Type* src = static_cast<const Type*>(source);
            if (*destination == nullptr) {
                Initialize(destination);
            }
            Type* dst = static_cast<Type*>(*destination);
            if (src == dst) {
                return OpcUa_Good;
            }
            *dst += *src;
            return OpcUa_Good;
        }
        static OpcUa_CppObject create() {
            OpcUa_CppObject obj;
            obj.meta = meta();
            Initialize(&obj.object);
            return obj;
        }
        static Type& get(OpcUa_CppObject& obj) {
            return *static_cast<Type*>(obj.object);
        }
        static const Type& get(const OpcUa_CppObject& obj) {
            return *static_cast<const Type*>(obj.object);
        }
        static const OpcUa_CppProxy* meta() {
            static OpcUa_CppProxy proxy;
            static bool inited = false;
            if (!inited) {
                inited = true;
                proxy.Clear = &Clear;
                proxy.Initialize = &Initialize;
                proxy.Compare = &Compare;
                proxy.CopyTo = &CopyTo;
                proxy.Concatenate = &Concatenate;
            }
            return &proxy;
        }
    };
    class OpcUa_Json {
    public:
        typedef rapidjson::MemoryPoolAllocator<> AllocatorType;
        MPLCSHARE_API OpcUa_Json();
        MPLCSHARE_API ~OpcUa_Json();
        MPLCSHARE_API void Clear();
        rapidjson::Value& get() {
            return data;
        }
        const Value& get() const {
            return data;
        }
        AllocatorType& GetAllocator() {
            return allocator;
        }
        MPLCSHARE_API OpcUa_Int operator==(const OpcUa_Json& val) const;
        MPLCSHARE_API OpcUa_Json& operator=(const OpcUa_Json& val);
        MPLCSHARE_API OpcUa_Json& operator+=(const OpcUa_Json& val);
        MPLCSHARE_API void swap(OpcUa_Json& rv_data);
        MPLCSHARE_API OpcUa_Int operator==(const Value& val) const;
        MPLCSHARE_API OpcUa_Json& operator=(const Value& val);
        MPLCSHARE_API OpcUa_Json& operator+=(const Value& val);
        // MPLCSHARE_API static bool equal(const rapidjson::Value& src, const rapidjson::Value&
        // dst);
        template<class Alloc>
        static void merge(const Value& src, Value& dst, Alloc& allocator) {
            using namespace rapidjson;
            switch (src.GetType()) {
            case kArrayType:
                if (!dst.IsArray())
                    dst.SetArray();
                for (size_t i = 0; i < src.Size(); ++i) {
                    Value val;
                    merge(src[i], val, allocator);
                    dst.PushBack(val, allocator);
                }
                break;
            case kObjectType: {
                bool created = !dst.IsObject();
                if (!dst.IsObject())
                    dst.SetObject();
                for (Value::ConstMemberIterator it = src.MemberBegin(); it != src.MemberEnd(); ++it) {
                    if (!created) {
                        Value::MemberIterator member = dst.FindMember(it->name);
                        if (member != dst.MemberEnd()) {
                            merge(it->value, member->value, allocator);
                            continue;
                        }
                    }
                    Value key, value;
                    merge(it->value, value, allocator);
                    merge(it->name, key, allocator);
                    dst.AddMember(key, value, allocator);
                }
            } break;
            case kStringType:
            case kNullType:
            case kFalseType:
            case kTrueType:
            case kNumberType:
                dst.CopyFrom(src, allocator);
                break;
            }
        }
        MPLCSHARE_API static OpcUa_Json& ref(OpcUa_CppObject& obj);
        MPLCSHARE_API static const OpcUa_Json& ref(const OpcUa_CppObject& obj);
        MPLCSHARE_API void from_string(lib::string_view str);
        template<class T>
        OpcUa_Json& operator<<(const T& val) {
            JsonWrapper json(data, allocator);
            set_json_data(json, val);
            return *this;
        }
        MPLCSHARE_API void from_msgpack(const msgpack::object& o, GetValueFlags::GetValueFlagsEnum flags = GetValueFlags::None);
    private:
        Value data;
        AllocatorType allocator;
    };

}  // namespace mplc
inline OpcUa_Void OpcUa_Json_Initialize(OpcUa_Variant* a_pValue) {
    a_pValue->Datatype = OpcUaType_JsonElement;
    a_pValue->Value.JsonElement = mplc::Cpp2CProxy<mplc::OpcUa_Json>::create();
}
