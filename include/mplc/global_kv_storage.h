#pragma once
#include <msgpack.hpp>
#include <share/config.h>
#include <mplc/libs/threads.hpp>
#include <mplc/libs/smart_ptr.hpp>
#include <mplc/libs/unordered_map.hpp>

namespace mplc { namespace kvb {

    using Value = lib::shared_ptr<const msgpack::object_handle>;
    struct Storage {
    private:
        lib::unordered_map<std::string, Value> stats;
        boost::shared_mutex writeMutex;

    public:
        Storage() {}
        Value Get(const std::string& key);
        std::vector<std::string> GetKeys();
        void Set(const std::string& key, Value val);
    };

    MPLCSHARE_API std::vector<std::string> GetKeys();
    MPLCSHARE_API Value GetValue(const std::string& key);
    MPLCSHARE_API void AddValue(const std::string& key, Value val);
    // MPLCSHARE_API void AddValue(const std::string& key, msgpack::object& obj);

    template<class T>
    boost::enable_if_t<!boost::is_arithmetic<T>::value, void> AddValue(const std::string& key, const T& val) {
        auto ptr = lib::make_shared<msgpack::object_handle>();
        auto z = new msgpack::zone(256);
        msgpack::object::with_zone oz(*z);
        oz << val;
        ptr->set(oz);
        ptr->zone().reset(z);
        return AddValue(key, std::move(Value(ptr)));
    }
    template<class T>
    boost::enable_if_t<boost::is_arithmetic<T>::value, void> AddValue(const std::string& key, T val) {
        auto ptr = lib::make_shared<msgpack::object_handle>();
        ptr->set(msgpack::object(val));
        return AddValue(key, std::move(Value(ptr)));
    }
}}  // namespace mplc::kvb
