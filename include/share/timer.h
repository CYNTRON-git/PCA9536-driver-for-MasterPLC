#pragma once

class Timer {
public:
    MPLCSHARE_API Timer();
    MPLCSHARE_API void Start();
    // MPLCSHARE_API void DurationFT(OpcUa_DateTime *tNDT);
    MPLCSHARE_API int64_t DurationFT() const;
    MPLCSHARE_API int64_t DurationMS() const;
    int64_t TimeStart() const {
        return _ft_timer;
    }

private:
    bool _Started;
    int64_t _ft_timer;
};
