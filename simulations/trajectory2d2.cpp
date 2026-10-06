#include <capd/capdlib.h>

#include "fun2d.hpp"
#include "draw.hpp"

int main() {
    class gnuPlotManager manager{{
        {
            .name = "Trajektoria",
            .file = "traj1.dat",
            .xName = "r",
            .yName = "dr",
        },
    }};

    struct InitData init = {
        .ce = 1.0,
        .cs = 0.00001,

        .r = 1.0,
        .dr = 0.0,
        .t = 0.0,
        .dt = 0.0,
    };

    constexpr uint32_t order = 20;
    capd::LDMap f{orbitkiAngle2D, 2, 2, 3}; {
        f.setParameter(CE, init.ce);
        f.setParameter(CS, init.cs);
        f.setParameter(KSI, calcKsi(init));
    }

    capd::LDOdeSolver::StepControlType s{2, 1.0 / 0x400000};
    capd::LDOdeSolver solver{f, order, s};

    capd::LDVector u{0.0, 0.0};
    u[R] = 1.0;

    long double t = 0.0;

    manager.print(0, "{} {}\n", u[R], u[DR]);
    manager.fflush();
    manager.initGNUPlot();

    while (t < 20) {
        std::print("{} {}\n", u[R], u[DR]);
        u = solver(t, u);

        manager.print(0, "{} {}\n", u[R], u[DR]);
    }

    return 0;
}
