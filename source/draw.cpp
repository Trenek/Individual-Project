#include <stdio.h>
#include <math.h>

#include <thread>

#include "common.h"

static void draw1(struct commonData &data, bool end) {
    static FILE* gp{[]() {
        FILE *gp = popen("gnuplot -persist", "w");

        fprintf(gp, "set term qt 1 size 800,600\n");
        fprintf(gp, "set title 'Trajektoria'\n");
        fprintf(gp, "set xlabel 'x'\n");
        fprintf(gp, "set ylabel 'y'\n");
        fprintf(gp, "set grid\n");
        fflush(gp);

        return gp;
    }()};

    if (end) pclose(gp);
    else {
        fprintf(gp, "plot '-' with lines title 'orbita'\n");
        for (auto& p : data.array) {
            fprintf(gp, "%f %f\n", p.pos[0], p.pos[1]);
        }
        fprintf(gp, "e\n");
        fflush(gp);
    }
}

static void draw2(struct commonData &data, bool end) {
    static FILE* gp{[]() {
        FILE *gp = popen("gnuplot -persist", "w");

        fprintf(gp, "set term qt 2 size 800,600\n");
        fprintf(gp, "set title 'Wychylenie'\n");
        fprintf(gp, "set xlabel 'x'\n");
        fprintf(gp, "set ylabel 'y'\n");
        fprintf(gp, "set grid\n");
        fflush(gp);

        return gp;
    }()};

    if (end) pclose(gp);
    else {
        fprintf(gp, "plot '-' with lines title 'wychylenie'\n");
        for (auto& p : data.array) {
            double curr = sqrt(p.pos[0] * p.pos[0] + p.pos[1] * p.pos[1]);

            fprintf(gp, "%f %f\n", p.t, curr);
        }
        fprintf(gp, "e\n");
        fflush(gp);
    }
}

static void draw3(struct commonData &data, bool end) {
    static FILE* gp{[]() {
        FILE *gp = popen("gnuplot -persist", "w");

        fprintf(gp, "set term qt 3 size 800,600\n");
        fprintf(gp, "set title 'Wychylenie'\n");
        fprintf(gp, "set xlabel 'x'\n");
        fprintf(gp, "set ylabel 'y'\n");
        fprintf(gp, "set grid\n");
        fflush(gp);

        return gp;
    }()};

    if (end) pclose(gp);
    else {
        fprintf(gp, "plot '-' with lines title 'wychylenie'\n");
        for (auto& p : data.array) {
            double curr = p.pos[0] * p.pos[1];

            fprintf(gp, "%f %f\n", p.t, curr);
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

        std::thread t1(draw1, std::ref(data), false);
        std::thread t2(draw2, std::ref(data), false);
        std::thread t3(draw3, std::ref(data), false);

        t1.join();
        t2.join();
        t3.join();

        data.access.unlock();
    } while(data.isFinished == false);

    std::thread t1(draw1, std::ref(data), true);
    std::thread t2(draw2, std::ref(data), true);
    std::thread t3(draw3, std::ref(data), true);

    t1.join();
    t2.join();
    t3.join();
}
