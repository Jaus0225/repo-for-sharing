#include "Process.h"

Process::Process(int pid, int priority) {
    this->pid = pid;
    this->priority = priority;
    this->cpuBurstRemaining = 0;
    this->accessIndex = 0;
}

bool Process::hasMoreAccesses() const {
    return accessIndex < memoryAccesses.size();
}

std::pair<int, unsigned int> Process::nextAccess() {
    if (accessIndex < memoryAccesses.size()) {
        std::pair<int, unsigned int> val = memoryAccesses[accessIndex];
        accessIndex++;
        return val;
    }
    return std::make_pair(-1, 0);
}

void Process::tick(int threadId) {
    if (cpuBurstRemaining > 0) {
        cpuBurstRemaining--;
    }
}

void Process::addThread(int threadId, int threadPriority) {
    threadPriorities[threadId] = threadPriority;
}