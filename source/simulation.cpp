#include "capd/capdlib.h"

#include "common.h"

static long double getAngle(long double pos1[2], long double pos2[2]) {
    long double x1 = pos1[0];
    long double x2 = pos2[0];
    long double y1 = pos1[1];
    long double y2 = pos2[1];

    long double r1 = sqrt(x1 * x1 + y1 * y1);
    long double r2 = sqrt(x2 * x2 + y2 * y2);

    return acos((x1 * x2 + y1 * y2) / (r1 * r2));
}

void simulate(struct commonData &data) {
    capd::LDMap pendulum(
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
    capd::LDOdeSolver solver{pendulum, order};

    solver.setStep(0.00001);

    capd::LDVector u{1.0, 0.0, 0.0, 0.06};
    long double t = 0.0;

    size_t cooldown = 0;
    long double max = 0;
    long double prev[2] = { u[0], u[1] };

    std::vector<struct data> temp;

    do {
        u = solver(t, u);

        long double curr = sqrt(u[0] * u[0] + u[1] * u[1]);
        temp.emplace_back((struct data) {
            .t = t,
            .pos = { u[0], u[1] }
        });

        if (cooldown > 0) cooldown -= 1;

        if (max < curr || abs(max - curr) < 10e-10) 
        if (cooldown == 0) {
            long double uu[2] = { u[0], u[1] };

            printf("\n%Lf\n", getAngle(prev, uu));
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

        printf("\rt=%6Lf, x=%6Lf, y=%6Lf, dx=%6Lf, dy=%6Lf", t, u[0], u[1], u[2], u[3]);
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
