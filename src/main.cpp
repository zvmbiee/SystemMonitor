#include "core/CpuSampler.h"
#include "core/ProcessList.h"
#include <iostream>
#include <thread>
#include <algorithm>

int main() {
	CpuSampler sampler;
	ProcessList process;

	while (true) {
		system("cls");

		process.Refresh();

		double load = sampler.GetCpuLoad();
		std::cout << "Memory load: " << process.sizeOfMemory() << " MB    " << "CPU load: " << load << "%\n\n";

		auto sortProcess = process.GetProcesses();
		std::sort(sortProcess.begin(), sortProcess.end(), 
			[](const ProcessInfo& left, const ProcessInfo& right) 
			{ return left.memoryMB > right.memoryMB; });

		std::cout << "Top 20 process of memory size: " << std::endl;

		int counter{1};
		for (const auto& proc : sortProcess) {
			int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, proc.name.data(), (int)proc.name.size(), NULL, 0, NULL, NULL);
			std::string name(sizeNeeded, 0);
			WideCharToMultiByte(CP_UTF8, 0, proc.name.data(), (int)proc.name.size(), name.data(), sizeNeeded, NULL, NULL);

			std::cout << counter << ": " << name << "(" << proc.pid << ")  " << "Memory usage: " << proc.memoryMB << " MB      " << "CPU usage: " << proc.cpuUsage << "%\n";
			++counter;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}