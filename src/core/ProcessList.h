#pragma once
#include <vector>
#include <unordered_map>
#include "ProcessInfo.h"

class ProcessList {
public:
    ProcessList();

    // Обновить список процессов
    void Refresh();

    const size_t sizeOfMemory() const;

    // Получить текущий список (константная ссылка, чтобы не копировать)
    const std::vector<ProcessInfo> GetProcesses() const { return m_processes; }

private:
    std::vector<ProcessInfo> m_processes;
    std::unordered_map<DWORD, ProcessTimeSnapshot> m_prevTimes;
    bool firstRun = true;

    uint64_t FileTimeToMicroseconds(const FILETIME& ft) const;
};