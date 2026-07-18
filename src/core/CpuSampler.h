#pragma once
#include <windows.h>
#include <cstdint>

class CpuSampler {
public:
    CpuSampler();

    double GetCpuLoad();

private:
    uint64_t m_prevIdle = 0;
    uint64_t m_prevKernel = 0;
    uint64_t m_prevUser = 0;
    bool m_isFirstCall;

    uint64_t FileTimeToMicroseconds(const FILETIME& ft) const;
};