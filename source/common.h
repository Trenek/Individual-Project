#include <vector>

#include <condition_variable>
#include <mutex>
#include <atomic>

typedef long double num;

struct data {
    num t;
    num pos[2];
};

struct commonData {
    std::atomic<bool> isFinished;
    std::atomic<bool> isDrawOrdered;
    std::condition_variable drawOrderMessager;
    std::mutex access;
    std::mutex drawMutex;

    num init;

    std::vector<data> array;
};

void draw(struct commonData &data);
void simulate(struct commonData &data);
