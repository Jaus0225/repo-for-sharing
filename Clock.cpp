#include "Clock.h"

Clock::Clock(int frameCount) {
    this->clockHand = 0;
    for (int i = 0; i < frameCount; i++) {
        FrameEntry f;
        f.pid = -1;
        f.pageNumber = -1;
        f.referenced = false;
        f.modified = false;
        frames.push_back(f);
    }
}

int Clock::evictAndReplace(int pid, int pageNum, int& evictedPage, bool isModified) {
    for (size_t i = 0; i < frames.size(); i++) {
        if (frames[i].pid == -1) {
            frames[i].pid = pid;
            frames[i].pageNumber = pageNum;
            frames[i].referenced = true;
            frames[i].modified = isModified;
            evictedPage = -1;
            return i;
        }
    }

    int chosenFrame = -1;
    for (int pass = 0; pass < 4; pass++) {
        for (size_t i = 0; i < frames.size(); i++) {
            size_t idx = (clockHand + i) % frames.size();
            bool ref = frames[idx].referenced;
            bool mod = frames[idx].modified;

            if (pass == 0 && !ref && !mod) {
                chosenFrame = idx;
                clockHand = (idx + 1) % frames.size();
                break;
            } else if (pass == 1 && !ref && mod) {
                chosenFrame = idx;
                clockHand = (idx + 1) % frames.size();
                break;
            } else if (pass == 2) {
                frames[idx].referenced = false;
            }
        }
        if (chosenFrame != -1) {
            break;
        }
    }

    if (chosenFrame == -1) {
        chosenFrame = clockHand;
        clockHand = (clockHand + 1) % frames.size();
    }

    evictedPage = frames[chosenFrame].pageNumber;
    frames[chosenFrame].pid = pid;
    frames[chosenFrame].pageNumber = pageNum;
    frames[chosenFrame].referenced = true;
    frames[chosenFrame].modified = isModified;

    return chosenFrame;
}

void Clock::markReferenced(int frameNum, bool isWrite) {
    if (frameNum >= 0 && frameNum < (int)frames.size()) {
        frames[frameNum].referenced = true;
        if (isWrite) {
            frames[frameNum].modified = true;
        }
    }
}

int Clock::getPidAt(int frameNum) const {
    if (frameNum >= 0 && frameNum < (int)frames.size()) {
        return frames[frameNum].pid;
    }
    return -1;
}