#include "ProcessList.h"
#include <tlhelp32.h>   // для CreateToolhelp32Snapshot
#include <psapi.h>      // для GetProcessMemoryInfo

ProcessList::ProcessList() {
    // Ничего не делаем, список будет заполнен при первом Refresh()
}

const size_t ProcessList::sizeOfMemory() const {
    size_t size{};
    for (const auto& proc : m_processes) {
        size += proc.memoryMB;
    }
    return size;
}

void ProcessList::Refresh() {
    // 1. Очищаем старый список
    m_processes.clear();

    // 2. Создаем снимок (snapshot) всех процессов
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        // Ошибка - выходим
        return;
    }

    PROCESSENTRY32W pe32;  // W - широкая строка (Unicode)
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    // 3. Начинаем перебор
    if (!Process32FirstW(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return;
    }

    do {
        // Открываем процесс, чтобы узнать его память
        HANDLE hProcess = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE,
            pe32.th32ProcessID
        );

        if (hProcess != NULL) {
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {

                FILETIME createTime, exitTime, idleTime,kernelTime, userTime, systemKernel, systemUser;
                double cpuUsage{};
                if (GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime)) {
                    GetSystemTimes(&idleTime, &systemKernel, &systemUser);

                    uint64_t curKernel = FileTimeToMicroseconds(kernelTime);
                    uint64_t curUser = FileTimeToMicroseconds(userTime);
                    uint64_t curSysKernel = FileTimeToMicroseconds(systemKernel);
                    uint64_t curSysUser = FileTimeToMicroseconds(systemUser);

                    if (firstRun) {
                        m_prevTimes.emplace(pe32.th32ProcessID, ProcessTimeSnapshot(curKernel, curUser, curSysKernel, curSysUser));
                    }
                    else {
                        if (auto it = m_prevTimes.find(pe32.th32ProcessID); it == m_prevTimes.end()) {
                            m_prevTimes.emplace(pe32.th32ProcessID, ProcessTimeSnapshot(curKernel, curUser, curSysKernel, curSysUser));
                        }
                        else {
                            uint64_t diffKernel = curKernel - it->second.m_prevKernel;
                            uint64_t diffUser = curUser - it->second.m_prevUser;
                            uint64_t diffSysKernel = curSysKernel - it->second.m_prevSystemKernel;
                            uint64_t diffSysUser = curSysUser - it->second.m_prevSystemUser;
                            
                            uint64_t totalSys = diffSysKernel + diffSysUser;
                            if (totalSys > 0) {
                                cpuUsage = (static_cast<double>(diffKernel + diffUser) / totalSys) * 100.;
                            }

                            m_prevTimes.emplace(pe32.th32ProcessID, ProcessTimeSnapshot(curKernel, curUser, curSysKernel, curSysUser));
                        }
                    }
                }

                ProcessInfo info;
                info.pid = pe32.th32ProcessID;
                info.name = pe32.szExeFile;  // std::wstring
                info.memoryMB = pmc.WorkingSetSize / (1024 * 1024);
                info.cpuUsage = cpuUsage * 12.0;

                m_processes.push_back(info);

            }
            CloseHandle(hProcess);
        }

    } while (Process32NextW(hSnapshot, &pe32) && m_processes.size() != 20);

    CloseHandle(hSnapshot);

    if (firstRun) {
        firstRun = false;
    }
}

uint64_t ProcessList::FileTimeToMicroseconds(const FILETIME& ft) const {
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;

    return static_cast<uint64_t>(uli.QuadPart / 10);
}