#include <math.h>

#include "common.h"

FILE* createPlot(const char *name);

template <std::size_t N>
void runThreads(struct commonData &data, void (*draw[N])(struct commonData &, bool)) {
    std::thread *t[N];

    for (size_t i = 0; i < N; i += 1) {
        t[i] = new std::thread(draw[i], std::ref(data), false);
    }

    for (auto &e : t) {
        e->join();
    }

    for (size_t i = 0; i < N; i += 1) {
        delete t[i];
    }
}

template <std::size_t N>
void drawFun(struct commonData &data, void (*draw[N])(struct commonData &, bool)) {
    do {
        std::unique_lock<std::mutex> lk(data.drawMutex);
        data.drawOrderMessager.wait(lk, [&] { return data.isDrawOrdered == true; });

        data.access.lock();
        data.isDrawOrdered = false;

        runThreads<N>(data, draw);

        data.access.unlock();
    } while(data.isFinished == false);

    runThreads<N>(data, draw);
}
