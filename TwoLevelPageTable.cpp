#include "TwoLevelPageTable.h"
#include <iostream>

TwoLevelPageTable::TwoLevelPageTable(int pageCount) {
    this->totalPages = pageCount;
    this->pageFaults = 0;
}

bool TwoLevelPageTable::isInMemory(int pid, int pageNum) const {
    int outer = (pageNum >> 6) & 0x3F;
    int inner = pageNum & 0x3F;

    std::pair<int, int> outerKey = std::make_pair(pid, outer);
    if (firstLevel.find(outerKey) == firstLevel.end()) {
        return false;
    }

    std::pair<int, int> innerKey = std::make_pair(pid, inner);
    if (secondLevel.find(innerKey) == secondLevel.end()) {
        return false;
    }

    std::pair<int, int> memKey = std::make_pair(pid, pageNum);
    return memoryPages.find(memKey) != memoryPages.end();
}

int TwoLevelPageTable::getFrame(int pid, int pageNum) const {
    int inner = pageNum & 0x3F;
    std::pair<int, int> innerKey = std::make_pair(pid, inner);
    std::map<std::pair<int, int>, int>::const_iterator it = secondLevel.find(innerKey);
    if (it != secondLevel.end()) {
        return it->second;
    }
    return -1;
}

void TwoLevelPageTable::loadPage(int pid, int pageNum, int frameNum) {
    int outer = (pageNum >> 6) & 0x3F;
    int inner = pageNum & 0x3F;

    std::pair<int, int> outerKey = std::make_pair(pid, outer);
    firstLevel[outerKey] = outer;

    std::pair<int, int> innerKey = std::make_pair(pid, inner);
    secondLevel[innerKey] = frameNum;

    memoryPages.insert(std::make_pair(pid, pageNum));
    pageFaults++;
}

void TwoLevelPageTable::invalidatePage(int pid, int pageNum) {
    int inner = pageNum & 0x3F;
    std::pair<int, int> innerKey = std::make_pair(pid, inner);
    secondLevel.erase(innerKey);
    memoryPages.erase(std::make_pair(pid, pageNum));
}

void TwoLevelPageTable::printStats() const {
    std::cout << "Page Faults: " << pageFaults << std::endl;
}

int TwoLevelPageTable::getPageFaults() const {
    return pageFaults;
}