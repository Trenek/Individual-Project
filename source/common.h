#pragma once

#include <vector>

#include <condition_variable>
#include <mutex>
#include <atomic>

typedef long double num;

struct commonData {
    std::atomic<bool> isFinished;
    std::atomic<bool> isDrawOrdered;
    std::condition_variable drawOrderMessager;
    std::mutex access;
    std::mutex drawMutex;

    num init;

    void *array;
};
