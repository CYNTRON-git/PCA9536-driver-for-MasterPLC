#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <mplc/libs/json.hpp>
#include "item_id.h"

namespace mplc { namespace vm {

    struct RemoteMplc {
        std::string address;
        int32_t exemplar{};
        int reserv_num{};
        bool operator==(const RemoteMplc& rv) const {
            return address == rv.address && exemplar == rv.exemplar && reserv_num == rv.reserv_num;
        }
    };
    struct RemotePLC {
        using Connections = std::vector<RemoteMplc>;
        Connections connections;
        int64_t ID;
        TimeSpan Timeout;
        RemotePLC(const rapidjson::Value& json);
    };
    struct VMTask {
        enum Type {
            T_LUA,
            T_NETCORE,
            T_REMOTE,
        };
        int64_t id{};
        Type type{T_LUA};
        int index{};
        VMTask(const json::Value& json);
        virtual ~VMTask() = default;
    };
    struct VMLuaTask : VMTask {
        std::string lua_name;
        VMLuaTask(const json::Value& json);
    };
    struct VMRemoteTask : VMTask {
        struct Link {
            int64_t link_id{0};
            ItemID item_id;
            Link(int64_t link_id, ItemID id): link_id(link_id), item_id(std::move(id)) {}
        };
        VMRemoteTask(const json::Value& json);
        std::vector<Link> links;
    };
    struct TaskFactory {
        static VMTask* create(const json::Value& json);
    };
}}  // namespace mplc::vm
