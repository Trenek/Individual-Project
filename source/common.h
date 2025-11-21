#include <vector>

#include <condition_variable>
#include <mutex>
#include <atomic>

struct data {
    double t;
    double pos[2];
};

struct commonData {
    std::atomic<bool> isFinished;
    std::atomic<bool> isDrawOrdered;
    std::condition_variable drawOrderMessager;
    std::mutex access;
    std::mutex drawMutex;

    std::vector<data> array;
};

void draw(struct commonData &data);
void simulate(struct commonData &data);
