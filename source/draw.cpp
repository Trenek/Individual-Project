#include <stdio.h>
#include <math.h>

#include <thread>

#include "common.h"

FILE* createPlot(const char *name, int num) {
    FILE *gp = popen("gnuplot -persist", "w");

    fprintf(gp, "set term qt %d size 800,600\n", num);
    fprintf(gp, "set title '%s'\n", name);
    fprintf(gp, "set xlabel 'x'\n");
    fprintf(gp, "set ylabel 'y'\n");
    fprintf(gp, "set grid\n");
    fflush(gp);

    return gp;
};

static void draw1(struct commonData &data, bool end) {
    static FILE* gp{createPlot("Trajektoria", 1)};

    if (end) pclose(gp);
    else {
        fprintf(gp, "plot '-' with lines title 'orbita'\n");
        for (auto& p : data.array) {
            fprintf(gp, "%Lf %Lf\n", p.pos[0], p.pos[1]);
        }
        fprintf(gp, "e\n");
        fflush(gp);
    }
}

static void draw2(struct commonData &data, bool end) {
    static FILE* gp{createPlot("Wychylenie", 2)};

    if (end) pclose(gp);
    else {
        fprintf(gp, "plot '-' with lines title 'wychylenie'\n");
        for (auto& p : data.array) {
            long double curr = sqrt(p.pos[0] * p.pos[0] + p.pos[1] * p.pos[1]);

            fprintf(gp, "%Lf %Lf\n", p.t, curr);
        }
        fprintf(gp, "e\n");
        fflush(gp);
    }
}

static void draw3(struct commonData &data, bool end) {
    static FILE* gp{createPlot("Cos", 3)};

    if (end) pclose(gp);
    else {
        fprintf(gp, "plot '-' with lines title 'wychylenie'\n");
        for (auto& p : data.array) {
            double curr = p.pos[0] * p.pos[1];

            fprintf(gp, "%Lf %f\n", p.t, curr);
        }
        fprintf(gp, "e\n");
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
            //std::thread(draw2, std::ref(data), false),
            //std::thread(draw3, std::ref(data), false),
        };

        for (auto &e : t) {
            e.join();
        }

        data.access.unlock();
    } while(data.isFinished == false);

    std::thread t[] = {
        std::thread(draw1, std::ref(data), true),
        //std::thread(draw2, std::ref(data), true),
        //std::thread(draw3, std::ref(data), true),
    };

    for (auto &e : t) {
        e.join();
    }
}
