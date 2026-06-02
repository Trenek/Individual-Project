#include <capd/capdlib.h>

#include "fun.hpp"
#include "draw.hpp"

int main() {
    class gnuPlotManager manager{{
        {
            .name = "Trajektoria",
            .file = "traj1.dat",

            .xName = "x",
            .yName = "y",
        },
        {
            .name = "Promień vs Prędkość Promienia",
            .file = "traj2.dat",

            .xName = "r",
            .yName = "dr",
        },
        {
            .name = "Promień vs Prędkość Kątowa",
            .file = "traj3.dat",

            .xName = "r",
            .yName = "dθ",
        },
        {
            .name = "Prędkość Promienia vs Prędkość Kątowa",
            .file = "traj4.dat",

            .xName = "dr",
            .yName = "dθ",
        },
    }, true};

    constexpr uint32_t order = 20;
    capd::LDMap f{orbitkiAngle, 4, 4, 2}; {
        f.setParameter(CE, 1.0);
        f.setParameter(CS, 0.00001);
    }

    capd::LDOdeSolver::StepControlType s{2, 1.0 / 0x400000};
    capd::LDOdeSolver solver{f, order, s};

    capd::LDVector u{0.0, 0.0, 0.0, 0.0};
    u[R] = 1.0;
    u[DT] = 0.2;

    long double t = 0.0;

    manager.print(0, "{} {}\n", u[0] * cos(u[1]), u[0] * sin(u[1]));
    manager.print(1, "{} {}\n", u[R], u[DR]);
    manager.print(2, "{} {}\n", u[R], u[DT]);
    manager.print(3, "{} {}\n", u[DR], u[DT]);
    manager.fflush();

    while (t < 200) {
        u = solver(t, u);
        manager.print(0, "{} {}\n", u[0] * cos(u[1]), u[0] * sin(u[1]));
        manager.print(1, "{} {}\n", u[R], u[DR]);
        manager.print(2, "{} {}\n", u[R], u[DT]);
        manager.print(3, "{} {}\n", u[DR], u[DT]);
    }

    return 0;
}
