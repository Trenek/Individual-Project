#include <capd/capdlib.h>

#include "fun.hpp"
#include "draw.hpp"

struct init {
    long double start;
    long double end;
    long double inc;
};

float findAngle(capd::LDVector v0, capd::LDPoincareMap &map) {
    capd::LDVector v1 = map(v0);
    COUT(v1);

    return 180.0 * v1[T] / std::numbers::pi;
}

int main(int argc, char **argv) {
    struct init init = {
        .start = argc > 1 ? atof(argv[1]) : -0.10,
        .end =   argc > 2 ? atof(argv[2]) :  0.10,
        .inc =   argc > 3 ? atof(argv[3]) :  0.005
    };
    class gnuPlotManager manager{{
        {
            .name = "Change In Angle",
            .file = "angle.dat",

            .xName = "dθ",
            .yName = "Δθ",
        },
    }};

    constexpr uint32_t order = 20;
    capd::LDMap f{orbitkiAngle, 4, 4, 2}; {
        f.setParameter(CE, 1.0);
        f.setParameter(CS, 0.000'01);
    }

    capd::LDOdeSolver solver{f, order}; {
        solver.setStep(0.000'01);
    }

    capd::LDCoordinateSection section{4, DR, 0.0};
    capd::LDPoincareMap map{solver, section, capd::poincare::PlusMinus};
    map.setMaxReturnTime(10);

    capd::LDVector u(4); u[0] = 0; u[1] = 0; u[2] = 0; u[3] = 0;
    u[R] = 1.0; u[DT] = init.start;

    u[DT] = init.start;
    manager.print(0, "{} {}\n", init.start, findAngle(u, map));
    manager.fflush();
    manager.initGNUPlot();
    for (long double i = init.start + init.inc; i < init.end; i += init.inc) {
        if (i < -0.025 || i > 0.5) solver.setStep(0.000'01);
        else if (i < 0.02) solver.setStep(0.000'001);
        else solver.setStep(0.000'000'1);
        u[DT] = i;
        manager.print(0, "{} {}\n", i, findAngle(u, map));
        manager.fflush();
    }

    return 0;
}
