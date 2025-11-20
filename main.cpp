#include <cstdio>
#include <stdint.h>

#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>

#include "capd/capdlib.h"
#include "capd/mpcapdlib.h"

struct commonData {
    std::atomic<bool> isFinished;
    std::atomic<bool> isWritten;
    std::mutex access;
    std::mutex dataWrittenMutex;
    std::condition_variable isDataWritten;

    std::vector<std::pair<double,double>> array;
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

    capd::DVector u{1.0, 0.0, 0.0, 0.2};
    double t = 0.0;

    size_t step = 0;
    size_t cooldown = 0;
    double max = 0;
    double prev[2] = { u[0], u[1] };

    std::vector<std::pair<double,double>> temp;

    do {
        u = solver(t, u);

        // store point
        double curr = sqrt(u[0] * u[0] + u[1] * u[1]);
        //pts.emplace_back(step, curr);
        temp.emplace_back(u[0], u[1]);

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

        if (data.access.try_lock()) {
            data.isWritten = true;
            data.isDataWritten.notify_all();

            for(auto &e : temp) {
                data.array.push_back(e);
            }
            temp.clear();
            data.access.unlock();
        }

        printf("\rt=%6f, x=%6f, y=%6f, dx=%6f, dy=%6f", t, u[0], u[1], u[2], u[3]);
    } while(t < 200);

    data.access.lock();
    data.isWritten = true;
    data.isFinished = true;
    data.isDataWritten.notify_all();

    for(auto &e : temp) {
        data.array.push_back(e);
    }

    temp.clear();
    data.access.unlock();
}

void write(struct commonData &data) {
    FILE* gp = popen("gnuplot -persist", "w");

    fprintf(gp, "set term qt size 800,600\n");
    fprintf(gp, "set title 'Live Trajectory'\n");
    fprintf(gp, "set xlabel 'x'\n");
    fprintf(gp, "set ylabel 'y'\n");
    fprintf(gp, "set grid\n");
    fflush(gp);

    do {
        std::unique_lock<std::mutex> lk(data.dataWrittenMutex);
        data.isDataWritten.wait(lk, [&] { return data.isWritten == true; });

        data.access.lock();
        data.isWritten = false;

        for (auto& p : data.array) {
            fprintf(gp, "%f %f\n", p.first, p.second);
        }
        data.access.unlock();

        fflush(gp);
        fprintf(gp, "e\n");
        fprintf(gp, "plot '-' with lines title 'orbit'\n");
    } while(data.isFinished == false);

    pclose(gp);
}

int main() {
    struct commonData data{
        .isFinished = false,
        .isWritten = false
    };

    std::thread reader(simulate, std::ref(data));
    std::thread writer(write, std::ref(data));

    reader.join();
    writer.join();

    return 0;
}
