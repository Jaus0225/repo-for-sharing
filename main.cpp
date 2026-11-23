#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Process.h"
#include "CPU.h"
#include "Scheduler.h"
#include "TLB.h"
#include "TwoLevelPageTable.h"
#include "Clock.h"

struct TraceRecord {
    int pid;
    int threadId;
    unsigned int address;
    char accessType;
};

int main(int argc, char* argv[]) {
    std::string traceFile = "input/trace.txt";
    int tlbSize = 16;
    int frameCount = 32;

    if (argc > 1) {
        tlbSize = std::stoi(argv[1]);
    }

    std::ifstream inFile(traceFile);
    if (!inFile.is_open()) {
        std::cerr << "Could not open trace file: " << traceFile << std::endl;
        return 1;
    }

    std::vector<TraceRecord> records;
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::istringstream iss(line);
        TraceRecord rec;
        std::string addrStr;
        if (iss >> rec.pid >> rec.threadId >> addrStr >> rec.accessType) {
            rec.address = std::stoul(addrStr, nullptr, 16);
            records.push_back(rec);
        }
    }
    inFile.close();

    TLB tlb(tlbSize);
    TwoLevelPageTable pt(1024);
    Clock clock(frameCount);
    CPU cpu;
    Scheduler scheduler(SchedulingPolicy::MLFQ, tlbSize);

    for (size_t i = 0; i < records.size(); i++) {
        scheduler.addProcessIfNotExists(records[i].pid);
        scheduler.getProcess(records[i].pid).memoryAccesses.push_back(
            std::make_pair(records[i].threadId, records[i].address)
        );
        scheduler.getProcess(records[i].pid).addThread(records[i].threadId, 1);
    }

    std::ofstream logFile("simulation.log");

    for (size_t i = 0; i < records.size(); i++) {
        TraceRecord rec = records[i];
        int pageNum = (rec.address >> 8) & 0xFFF;
        bool isWrite = (rec.accessType == 'W' || rec.accessType == 'w');

        Process& proc = scheduler.getProcess(rec.pid);
        cpu.execute(proc, rec.threadId);
        scheduler.tick();

        int frame = -1;
        bool hit = tlb.lookup(rec.pid, pageNum, frame);

        if (!hit) {
            if (pt.isInMemory(rec.pid, pageNum)) {
                frame = pt.getFrame(rec.pid, pageNum);
            } else {
                int evictedPage = -1;
                frame = clock.evictAndReplace(rec.pid, pageNum, evictedPage, isWrite);
                if (evictedPage != -1) {
                    pt.invalidatePage(clock.getPidAt(frame), evictedPage);
                }
                pt.loadPage(rec.pid, pageNum, frame);
            }
            tlb.insert(rec.pid, pageNum, frame);
        }

        clock.markReferenced(frame, isWrite);

        logFile << "Step: " << i 
                << " | PID: " << rec.pid 
                << " | TID: " << rec.threadId 
                << " | Addr: 0x" << std::hex << rec.address << std::dec
                << " | Page: " << pageNum 
                << " | Frame: " << frame 
                << " | TLB: " << (hit ? "HIT" : "MISS") 
                << std::endl;
    }

    logFile.close();

    std::cout << "--- Simulation Results ---" << std::endl;
    cpu.printStats();
    tlb.printStats();
    pt.printStats();

    return 0;
}