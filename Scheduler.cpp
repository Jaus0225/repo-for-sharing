#include "Scheduler.h"

Scheduler::Scheduler(SchedulingPolicy policy, int tlbSize) {
    this->policy = policy;
    this->quantum = 4;
    this->currentQuantumUsed = 0;
    this->readyQueues.resize(3);
}

void Scheduler::addProcessIfNotExists(int pid) {
    if (processes.find(pid) == processes.end()) {
        Process p(pid, 0);
        processes.insert(std::make_pair(pid, p));
        standardQueue.push(pid);
        readyQueues[0].push(pid);
        waitCounters[pid] = 0;
    }
}

Process& Scheduler::getProcess(int pid) {
    return processes.find(pid)->second;
}

bool Scheduler::hasPendingWork() const {
    for (std::map<int, Process>::const_iterator it = processes.begin(); it != processes.end(); it++) {
        if (it->second.hasMoreAccesses()) {
            return true;
        }
    }
    return false;
}

int Scheduler::getNextProcessId() {
    if (policy == SchedulingPolicy::FCFS) {
        if (!standardQueue.empty()) {
            int current = standardQueue.front();
            if (processes.find(current)->second.hasMoreAccesses()) {
                return current;
            }
            standardQueue.pop();
            return getNextProcessId();
        }
    } else if (policy == SchedulingPolicy::RoundRobin) {
        if (!standardQueue.empty()) {
            int current = standardQueue.front();
            standardQueue.pop();
            standardQueue.push(current);
            return current;
        }
    } else if (policy == SchedulingPolicy::Priority) {
        int bestPid = -1;
        int bestPriority = 999999;
        for (std::map<int, Process>::iterator it = processes.begin(); it != processes.end(); it++) {
            if (it->second.hasMoreAccesses() && it->second.priority < bestPriority) {
                bestPriority = it->second.priority;
                bestPid = it->first;
            }
        }
        return bestPid;
    } else if (policy == SchedulingPolicy::MLFQ) {
        for (int q = 0; q < 3; q++) {
            if (!readyQueues[q].empty()) {
                int pid = readyQueues[q].front();
                readyQueues[q].pop();
                return pid;
            }
        }
    }
    return -1;
}

void Scheduler::tick() {
    currentQuantumUsed++;
    for (std::map<int, int>::iterator it = waitCounters.begin(); it != waitCounters.end(); it++) {
        it->second++;
        if (policy == SchedulingPolicy::MLFQ && it->second > 10) {
            it->second = 0;
            readyQueues[0].push(it->first);
        }
    }
}

void Scheduler::adjustPriority(int pid, int threadId) {
    if (policy == SchedulingPolicy::MLFQ) {
        readyQueues[2].push(pid);
    }
}