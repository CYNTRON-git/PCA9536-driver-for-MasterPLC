#pragma once
// Minimal host-side channel storage. The ARM build separately uses the real SDK.
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <cstdint>

#define MPLC_OBJECT(type) using FixtureObject = type
#define MPLC_PROTOCOL_TYPE(name, type)
constexpr int OpcUa_BadCommunicationError = 1;
struct CpuLoadState {};
inline CpuLoadState* InitGetCPULoad() { return new CpuLoadState; }
inline void DeleteCpuLoadState(CpuLoadState* state) { delete state; }
inline double GetCPULoad(CpuLoadState*) { return 42.0; }

namespace mplc {
struct DateTime {
    DateTime() = default;
    DateTime(int, int, int, int, int, int) {}
    static DateTime from_unix(long) { return {}; }
};
namespace vm {
struct Channel {
    std::string name;
    std::string getTranslitedName() const { return name; }
};
}
namespace api {
struct Variable {
    std::variant<std::monostate, bool, int16_t, float, std::string, DateTime> value;
    int quality = -1;
    template<class T> void Update(T v) { value = v; quality = 0; }
    template<class T> T Get() const { return std::get<T>(value); }
    void SetQuality(int q) { quality = q; }
};
struct ScadaChannel {
    Variable InVar, OutVar;
    void BaseInit(const vm::Channel*) {}
};
struct ScadaProtocol {
    virtual ~ScadaProtocol() = default;
    virtual void Init() = 0;
    virtual void Execute() = 0;
    virtual ScadaChannel* Create(const vm::Channel*) = 0;
    bool connected = true, executing = true, fault = false;
    int acks = 0;
    bool isConnect() const { return connected; }
    bool isExecute() const { return executing; }
    void SetFaultState(bool current, const std::string&) { fault = current; }
    void AcknowledgeAllChanges() { ++acks; }
    void Register(ScadaChannel* ch) { channels.emplace_back(ch); }
private:
    std::vector<std::unique_ptr<ScadaChannel>> channels;
};
}
}
