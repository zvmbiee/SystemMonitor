#include "CpuSampler.h"

CpuSampler::CpuSampler() : m_isFirstCall(true) {}

double CpuSampler::GetCpuLoad() {
    FILETIME idleTime, kernelTime, userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);

    uint64_t curIdle = FileTimeToMicroseconds(idleTime);
    uint64_t curKernel = FileTimeToMicroseconds(kernelTime);
    uint64_t curUser = FileTimeToMicroseconds(userTime);

    if (m_isFirstCall) {
        m_prevIdle = curIdle;
        m_prevKernel = curKernel;
        m_prevUser = curUser;
        m_isFirstCall = false;
        return 0.0;
    }
    
    uint64_t diffIdle = curIdle - m_prevIdle;
    uint64_t diffKernel = curKernel - m_prevKernel;
    uint64_t diffUser = curUser - m_prevUser;

    uint64_t total = diffKernel + diffUser;
    uint64_t busy = total - diffIdle;

    if (total == 0) return 0.0;

    double load = (static_cast<double>(busy) / total) * 100.0;

    m_prevIdle = curIdle;
    m_prevKernel = curKernel;
    m_prevUser = curUser;

    return load;
}

uint64_t CpuSampler::FileTimeToMicroseconds(const FILETIME& ft) const {
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    
    return static_cast<uint64_t>(uli.QuadPart / 10);
}