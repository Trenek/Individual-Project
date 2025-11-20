#include <cstdio>
#include <stdint.h>

#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>

#include "capd/capdlib.h"
#include "capd/mpcapdlib.h"

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

// x = r * sin(theta)
// y = r * cos(theta)
// x^2 + y^2 = r^2
double getAngle(double pos1[2], double pos2[2]) {
    double x1 = pos1[0];
    double x2 = pos2[0];
    double y1 = pos1[1];
    double y2 = pos2[1];

    double r1 = sqrt(x1 * x1 + y1 * y1);
    double r2 = sqrt(x2 * x2 + y2 * y2);

    return acos((x1 * x2 + y1 * y2) / (r1 * r2));
}

void simulate(struct commonData &data) {
    capd::DMap pendulum(
    "time:t;"
    "var:"
        "x,"
        "y,"
        "dx,"
        "dy;"
    "par:"
        "ce,"
        "cs;"
    "fun:"
        "dx,"
        "dy,"
        "-(ce*x+cs*dy)/((x^2+y^2)^1.5),"
        "(cs*dx-ce*y)/((x^2+y^2)^1.5);"
    );
    pendulum.setParameter("ce", 1.0);
    pendulum.setParameter("cs", 0.00001);

    uint32_t order = 100;
    capd::DOdeSolver solver{pendulum, order};

    solver.setStep(0.0001);

    capd::DVector u{1.0, 0.0, 0.0, 0.138};
    double t = 0.0;

    size_t cooldown = 0;
    double max = 0;
    double prev[2] = { u[0], u[1] };

    std::vector<struct data> temp;

    do {
        u = solver(t, u);

        // store point
        double curr = sqrt(u[0] * u[0] + u[1] * u[1]);
        temp.emplace_back((struct data) {
            .t = t,
            .pos = { u[0], u[1]}
        });

        if (cooldown > 0) cooldown -= 1;

        if (max < curr || abs(max - curr) < 10e-10) 
        if (cooldown == 0) {
            double uu[2] = { u[0], u[1] };

            printf("\n%f\n", getAngle(prev, uu));
            max = curr;
            prev[0] = u[0];
            prev[1] = u[1];
            cooldown = 50;
        }

        if (data.isDrawOrdered == false && data.access.try_lock()) {
            data.isDrawOrdered = true;

            for(auto &e : temp) {
                data.array.push_back(e);
            }
            temp.clear();
            data.access.unlock();

            data.drawOrderMessager.notify_all();
        }

        printf("\rt=%6f, x=%6f, y=%6f, dx=%6f, dy=%6f", t, u[0], u[1], u[2], u[3]);
    } while(t < 20'000);

    data.access.lock();
    data.isFinished = true;
    data.isDrawOrdered = true;

    for(auto &e : temp) {
        data.array.push_back(e);
    }

    temp.clear();
    data.access.unlock();

    data.drawOrderMessager.notify_all();
}

void draw1(struct commonData &data, bool end) {
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

void draw2(struct commonData &data, bool end) {
    static FILE* gp{[]() {
        FILE *gp = popen("gnuplot -persist", "w");

        fprintf(gp, "set term qt 1 size 800,600\n");
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

void draw(struct commonData &data) {
    do {
        std::unique_lock<std::mutex> lk(data.drawMutex);
        data.drawOrderMessager.wait(lk, [&] { return data.isDrawOrdered == true; });

        data.access.lock();
        data.isDrawOrdered = false;

        std::thread t1(draw1, std::ref(data), false);
        std::thread t2(draw2, std::ref(data), false);

        t1.join();
        t2.join();

        data.access.unlock();
    } while(data.isFinished == false);

    std::thread t1(draw1, std::ref(data), true);
    std::thread t2(draw2, std::ref(data), true);

    t1.join();
    t2.join();
}

int main() {
    struct commonData data{};

    std::thread writer(simulate, std::ref(data));
    std::thread reader(draw, std::ref(data));

    writer.join();
    reader.join();

    return 0;
}
