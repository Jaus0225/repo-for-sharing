#include "TLB.h"
#include <iostream>

TLB::TLB(int size) {
    this->maxSize = size;
    this->currentTime = 0;
    this->hits = 0;
    this->misses = 0;
}

bool TLB::lookup(int pid, int pageNumber, int& frameNumber) {
    currentTime++;
    for (size_t i = 0; i < tlbEntries.size(); i++) {
        if (tlbEntries[i].pid == pid && tlbEntries[i].pageNumber == pageNumber) {
            tlbEntries[i].lastUsed = currentTime;
            frameNumber = tlbEntries[i].frameNumber;
            hits++;
            return true;
        }
    }
    misses++;
    return false;
}

void TLB::insert(int pid, int pageNumber, int frameNumber) {
    currentTime++;
    for (size_t i = 0; i < tlbEntries.size(); i++) {
        if (tlbEntries[i].pid == pid && tlbEntries[i].pageNumber == pageNumber) {
            tlbEntries[i].frameNumber = frameNumber;
            tlbEntries[i].lastUsed = currentTime;
            return;
        }
    }

    if ((int)tlbEntries.size() < maxSize) {
        TLBEntry entry;
        entry.pid = pid;
        entry.pageNumber = pageNumber;
        entry.frameNumber = frameNumber;
        entry.lastUsed = currentTime;
        tlbEntries.push_back(entry);
    } else {
        int lruIndex = 0;
        int minTime = tlbEntries[0].lastUsed;
        for (size_t i = 1; i < tlbEntries.size(); i++) {
            if (tlbEntries[i].lastUsed < minTime) {
                minTime = tlbEntries[i].lastUsed;
                lruIndex = i;
            }
        }
        tlbEntries[lruIndex].pid = pid;
        tlbEntries[lruIndex].pageNumber = pageNumber;
        tlbEntries[lruIndex].frameNumber = frameNumber;
        tlbEntries[lruIndex].lastUsed = currentTime;
    }
}

void TLB::printStats() const {
    std::cout << "TLB Hits: " << hits << std::endl;
    std::cout << "TLB Misses: " << misses << std::endl;
    if (hits + misses > 0) {
        double rate = (double)hits / (hits + misses) * 100.0;
        std::cout << "TLB Hit Rate: " << rate << "%" << std::endl;
    }
}

int TLB::getHits() const { return hits; }
int TLB::getMisses() const { return misses; }