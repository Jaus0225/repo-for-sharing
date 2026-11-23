#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Process.h"
#include <map>
#include <vector>
#include <queue>

enum class SchedulingPolicy { FCFS, RoundRobin, Priority, MLFQ };

class Scheduler {
private:
    SchedulingPolicy policy;
    std::map<int, Process> processes;
    std::vector<std::queue<int>> readyQueues;
    std::queue<int> standardQueue;
    std::map<int, int> waitCounters;
    int quantum;
    int currentQuantumUsed;

public:
    Scheduler(SchedulingPolicy policy, int tlbSize);
    void addProcessIfNotExists(int pid);
    Process& getProcess(int pid);
    void tick();
    void adjustPriority(int pid, int threadId);
    bool hasPendingWork() const;
    int getNextProcessId();
};

#endif