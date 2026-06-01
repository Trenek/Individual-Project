#include <capd/capdlib.h>

#include "fun.hpp"
#include "draw.hpp"

struct init {
    long double start;
    long double end;
    long double inc;
};

float getAngle(capd::LDVector v0) {
    return 180.0 * v0[T] / std::numbers::pi;
}

int main(int argc, char **argv) {
    struct init init = {
        .start = argc > 1 ? atof(argv[1]) : -0.10,
        .end =   argc > 2 ? atof(argv[2]) :  0.10,
        .inc =   argc > 3 ? atof(argv[3]) :  0.00005
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
        f.setParameter(CS, 0.00001);
    }

    capd::LDOdeSolver solver{f, order};

    capd::LDOdeSolver::StepControlType s{2, 1.0 / 0x400000};
    solver.setStepControl(s);

    capd::LDCoordinateSection section{4, DR, 0.0};
    capd::LDPoincareMap map{solver, section, capd::poincare::PlusMinus};

    capd::LDVector u{0.0, 0.0, 0.0, 0.0};
    u[R] = 1.0;
    u[DT] = init.start;

    manager.print(0, "{} {}\n", init.start, getAngle(map(u)));
    manager.fflush();
    manager.initGNUPlot();
    for (long double i = init.start + init.inc; true; i += init.inc) {
        u[DT] = i;
        manager.print(0, "{} {}\n", i, getAngle(map(u)));
    }
    manager.fflush();

    return 0;
}
