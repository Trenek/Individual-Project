#include <print>
#include <math.h>
#include <thread>

#include "common.h"

FILE* createPlot(const char *name) {
    static int num = 1;
    FILE *gp = popen("gnuplot", "w");

    fprintf(gp, "set term qt %d size 800,600\n", num);
    fprintf(gp, "set title '%s'\n", name);
    fprintf(gp, "set xlabel 'x'\n");
    fprintf(gp, "set ylabel 'y'\n");
    fprintf(gp, "set grid\n");
    fflush(gp);

    num += 1;

    return gp;
};

static void draw1(struct commonData &data, bool end) {
    static FILE* gp{createPlot("Trajektoria")};

    if (end) pclose(gp);
    else {
        std::print(gp, "plot '-' with lines title 'orbita'\n");
        for (auto& p : data.array) {
            std::print(gp, "{} {}\n", p.pos[0], p.pos[1]);
        }
        std::print(gp, "e\n");

        fflush(gp);
    }
}

static void draw2(struct commonData &data, bool end) {
    static FILE* gp{createPlot("Wychylenie")};

    if (end) pclose(gp);
    else {
        std::print(gp, "plot '-' with lines title 'wychylenie'\n");
        for (auto& p : data.array) {
            num curr = sqrt(p.pos[0] * p.pos[0] + p.pos[1] * p.pos[1]);

            std::print(gp, "{} {}\n", p.t, curr);
        }
        std::print(gp, "e\n");
        fflush(gp);
    }
}

static void draw3(struct commonData &data, bool end) {
    static FILE* gp{createPlot("Cos")};

    if (end) pclose(gp);
    else {
        std::print(gp, "plot '-' with lines title 'wychylenie'\n");
        for (auto& p : data.array) {
            num curr = p.pos[0] * p.pos[1];

            std::print(gp, "{} {}\n", p.t, curr);
        }
        std::print(gp, "e\n");
        fflush(gp);
    }
}

void draw(struct commonData &data) {
    do {
        std::unique_lock<std::mutex> lk(data.drawMutex);
        data.drawOrderMessager.wait(lk, [&] { return data.isDrawOrdered == true; });

        data.access.lock();
        data.isDrawOrdered = false;
        
        std::thread t[] = {
            std::thread(draw1, std::ref(data), false),
            std::thread(draw2, std::ref(data), false),
            //std::thread(draw3, std::ref(data), false),
        };

        for (auto &e : t) {
            e.join();
        }

        data.access.unlock();
    } while(data.isFinished == false);

    std::thread t[] = {
        std::thread(draw1, std::ref(data), true),
        std::thread(draw2, std::ref(data), true),
        //std::thread(draw3, std::ref(data), true),
    };

    for (auto &e : t) {
        e.join();
    }
}
