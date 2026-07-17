#include "core/CpuSampler.h"
#include <thread>

#include <iostream>

int main() {
    CpuSampler sampler;

    while (true) {
        double load = sampler.GetCpuLoad();
        std::cout << "\rCPU Load: " << load << "%   " << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}