#include "CPU.h"
#include <iostream>

CPU::CPU() {
    cyclesExecuted = 0;
    contextSwitches = 0;
    threadSwitches = 0;
    lastPid = -1;
    lastThreadId = -1;
}

void CPU::execute(Process& p, int threadId) {
    if (lastPid != -1 && lastPid != p.pid) {
        contextSwitches++;
    } else if (lastPid == p.pid && lastThreadId != -1 && lastThreadId != threadId) {
        threadSwitches++;
    }

    lastPid = p.pid;
    lastThreadId = threadId;

    p.tick(threadId);
    cyclesExecuted++;
}

void CPU::printStats() const {
    std::cout << "CPU Execution Cycles: " << cyclesExecuted << std::endl;
    std::cout << "Context Switches: " << contextSwitches << std::endl;
    std::cout << "Thread Switches: " << threadSwitches << std::endl;
}

int CPU::getCycles() const { return cyclesExecuted; }
int CPU::getContextSwitches() const { return contextSwitches; }
int CPU::getThreadSwitches() const { return threadSwitches; }