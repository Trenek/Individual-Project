#include "capd/capdlib.h"
#include "capd/mpcapdlib.h"

#include "common.h"

static double getAngle(double pos1[2], double pos2[2]) {
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
