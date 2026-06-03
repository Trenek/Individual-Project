#include <capd/capdlib.h>

#include "fun.hpp"
#include "draw.hpp"

constexpr long double inc = 0.0005;

float getAngle(capd::LDPoincareMap map, capd::LDVector v0) {
    capd::LDVector v1 = map(v0);
    capd::LDVector v2 = map(v1);
    return 180.0 * (v2[T] - v1[T]) / std::numbers::pi;
}

float findFirst(capd::LDPoincareMap &map) {
    float first = 0;

    try {
        while (true) {
            capd::LDVector u{0.0, 0.0, 0.0, 0.0};
            u[R] = 1.0;
            u[DT] = first;

            map(u);

            first -= 0.10;
        }
    }
    catch (std::exception &e) {
        
    }

    return first + 0.10;
}

int main() {
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

    // capd::LDOdeSolver::StepControlType s{2, 1.0 / 0x4000000000};
    capd::LDOdeSolver::StepControlType s{2, 0};

    capd::LDOdeSolver solver{f, order, s};

    capd::LDCoordinateSection section{4, DR, 0.0};
    capd::LDPoincareMap map{solver, section, capd::poincare::PlusMinus};

    capd::LDVector u{0.0, 0.0, 0.0, 0.0};
    u[R] = 1.0;
    u[DT] = findFirst(map);

    manager.print(0, "{} {}\n", u[DT], getAngle(map, u));
    manager.fflush();
    manager.initGNUPlot();

    while (true) {
        u[DT] += inc;
        manager.print(0, "{} {}\n", u[DT], getAngle(map, u));
    }
    manager.fflush();

    return 0;
}
