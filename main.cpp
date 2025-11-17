#include <cstdio>
#include <stdint.h>

#include "capd/capdlib.h"

int main() {
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

    capd::DVector u{1.0, 1.0, 0.0, 1.1};
    double t = 0.0;

    FILE* gp = popen("gnuplot -persist", "w");
    if (!gp) {
        fprintf(stderr, "Failed to start gnuplot.\n");
        return 1;
    }

    fprintf(gp, "set term qt size 800,600\n");
    fprintf(gp, "set title 'Live Trajectory'\n");
    fprintf(gp, "set xlabel 'x'\n");
    fprintf(gp, "set ylabel 'y'\n");
    fprintf(gp, "set grid\n");
    fflush(gp);

    std::vector<std::pair<double,double>> pts;

    size_t step = 0;

    do {
        u = solver(t, u);

        // store point
        pts.emplace_back(u[0], u[1]);

        step += 1;
        if (step % 50'000 == 0) {
            fprintf(gp, "plot '-' with lines title 'orbit'\n");
            for (auto& p : pts)
                fprintf(gp, "%f %f\n", p.first, p.second);
            fprintf(gp, "e\n");
            fflush(gp);
        }
        printf("\rt=%6f, x=%6f, y=%6f, dx=%6f, dy=%6f", t, u[0], u[1], u[2], u[3]);

    } while(t < 200.0);

    pclose(gp);

    return 0;
}
