#pragma once

template<typename T>
using CMonitoredItemClientMap = mplc::lib::unordered_map<OpcUa_UInt32, T>;

template<typename T>
class OPCMapedClt {
private:
    OpcUa_UInt32 _ClientHandle;
    CMonitoredItemClientMap<T> _ClientHandleMap;
    mplc::lib::mutex mtx_handle_map;

public:
    OPCMapedClt() { _ClientHandle = 0; }
    ~OPCMapedClt() { Clear(); }
    OpcUa_UInt32 AddDsc(T dsc) { 
        mplc::lib::unique_lock<mplc::lib::mutex> lock(mtx_handle_map);
        auto handle = _ClientHandle++;
        _ClientHandleMap[handle] = dsc;
        return handle;
    }
    T FindDsc(OpcUa_UInt32 handle) {
        mplc::lib::unique_lock<mplc::lib::mutex> lock(mtx_handle_map);
        auto it = _ClientHandleMap.find(handle);
        if (it != _ClientHandleMap.end())
            return it->second;
        return nullptr;
    }
    void Clear(void) 
    {
        mplc::lib::unique_lock<mplc::lib::mutex> lock(mtx_handle_map);
         _ClientHandleMap.clear(); 
         _ClientHandle = 0; 
    }
};
