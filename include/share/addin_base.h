#pragma once
#include <share/config.h>
#include <opcua/opcua.h>

#if defined(WINCE)
MPLCSHARE_API std::string ws2s(const std::wstring& s);
#endif

class AddinBase {
public:
    virtual ~AddinBase() = default;

protected:
    MPLCSHARE_API OpcUa_StatusCode AttachConfigProcessor(const char* name, uint32_t callback_mode = 0xffffffffu);

    const char* GetName(void) {
        return _name;
    }
    virtual OpcUa_StatusCode OnConfigEvent(ConfigProcessorMode, ControllerConfig*) {
        return OpcUa_Good;
    }

private:
    const char* _name{};
    static int ConfigProcessorCallback(void* data, ConfigProcessorMode mode, ControllerConfig* config);
};
