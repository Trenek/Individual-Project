#include <cstdio>
#include <stdint.h>

#include "capd/capdlib.h"
#include "capd/mpcapdlib.h"

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

    capd::DVector u{1.0, 0.0, 0.0, 0.001};
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
    size_t cooldown = 0;
    double max = 0;
    double prev[2] = { u[0], u[1] };

    do {
        u = solver(t, u);

        // store point
        double curr = sqrt(u[0] * u[0] + u[1] * u[1]);
        //pts.emplace_back(step, curr);
        pts.emplace_back(u[0], u[1]);

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

        step += 1;
        if (step % 5'000 == 0) {
            fprintf(gp, "plot '-' with lines title 'orbit'\n");
            for (auto& p : pts)
                fprintf(gp, "%f %f\n", p.first, p.second);
            fprintf(gp, "e\n");
            fflush(gp);
        }
        printf("\rt=%6f, x=%6f, y=%6f, dx=%6f, dy=%6f", t, u[0], u[1], u[2], u[3]);

    } while(t < 200'000.0);

    pclose(gp);

    return 0;
}
