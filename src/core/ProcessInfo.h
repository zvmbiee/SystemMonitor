#pragma once
#include <string>
#include <windows.h>
#include <cstdint>

struct ProcessInfo {
    DWORD pid = 0;                  // ID процесса
    std::wstring name;          // Имя (в Unicode, т.к. Windows)
    size_t memoryMB = 0;            // Память в мегабайтах (рабочий набор)
    double cpuUsage = 0.0;            // Использование CPU
};

struct ProcessTimeSnapshot
{
    uint64_t m_prevKernel = { 0 };
    uint64_t m_prevUser = { 0 };
    uint64_t m_prevSystemKernel = { 0 };
    uint64_t m_prevSystemUser = { 0 };
};