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

float getMax(float cs, float r) {
    float max = 0;

    try {
        constexpr uint32_t order = 20;
        capd::LDMap f{orbitkiAngle, 4, 4, 2}; {
            f.setParameter(CE, 1.0);
            f.setParameter(CS, cs);
        }

        // capd::LDOdeSolver::StepControlType s{2, 1.0 / 0x4000000000};
        capd::LDOdeSolver::StepControlType s{2, 0};

        capd::LDOdeSolver solver{f, order, s};

        capd::LDCoordinateSection section{4, DR, 0.0};

        capd::LDPoincareMap map{solver, section, capd::poincare::PlusMinus};

        capd::LDVector u{0.0, 0.0, 0.0, 0.0};
        u[R] = r;
        u[DT] = findFirst(map);

        max = getAngle(map, u);

        while (true) {
            u[DT] += inc;
            max = std::max(max, getAngle(map, u));
        }
    } catch (std::exception &e) {}

    printf("We Alive\n");

    return max;
}

int main() {
    class gnuPlotManager manager{{
        {
            .name = "Change In Angle",
            .file = "angle.dat",

            .xName = "r0",
            .yName = "max Δθ",
        },
    }};

    float cs = 0.00001;
    float r = 0.5;

    manager.print(0, "{} {}\n", r, getMax(cs, r));
    manager.fflush();
    manager.initGNUPlot();

    while (true) {
        r += 0.0001;
        manager.print(0, "{} {}\n", r, getMax(cs, r));
        manager.fflush();
    }

    return 0;
}
// #include <capd/capdlib.h>
//
// #include "fun.hpp"
// #include "draw.hpp"
//
// constexpr long double inc = 0.0005;
//
// float getAngle(capd::LDPoincareMap map, capd::LDVector v0) {
//     capd::LDVector v1 = map(v0);
//     capd::LDVector v2 = map(v1);
//     return 180.0 * (v2[T] - v1[T]) / std::numbers::pi;
// }
//
// float findFirst(capd::LDPoincareMap &map) {
//     float first = 0;
//
//     try {
//         while (true) {
//             capd::LDVector u{0.0, 0.0, 0.0, 0.0};
//             u[R] = 1.0;
//             u[DT] = first;
//
//             map(u);
//
//             first -= 0.10;
//         }
//     }
//     catch (std::exception &e) {
//
//     }
//
//     return first + 0.10;
// }
//
// int main() {
//     class gnuPlotManager manager{{
//         {
//             .name = "Radious",
//             .file = "angle.dat",
//
//             .xName = "dθ",
//             .yName = "r",
//         },
//     }};
//
//     constexpr uint32_t order = 20;
//     capd::LDMap f{orbitkiAngle, 4, 4, 2}; {
//         f.setParameter(CE, 1.0);
//         f.setParameter(CS, 0.00001);
//     }
//
//     // capd::LDOdeSolver::StepControlType s{2, 1.0 / 0x4000000000};
//     capd::LDOdeSolver::StepControlType s{2, 0};
//
//     capd::LDOdeSolver solver{f, order, s};
//
//     capd::LDCoordinateSection section{4, DR, 0.0};
//     capd::LDPoincareMap map{solver, section, capd::poincare::MinusPlus};
//
//     capd::LDVector u{0.0, 0.0, 0.0, 0.0};
//     float dt = -0.1;
//     u[R] = 1.0;
//     u[DR] = 0;
//     u[T] = 0;
//     u[DT] = dt;
//
//     u = map(u);
//
//     manager.print(0, "{} {}\n", dt, u[R]);
//     manager.fflush();
//     manager.initGNUPlot();
//
//     while (dt <= 0.1) {
//         dt += 0.001;
//
//         u[R] = 1.0;
//         u[DR] = 0;
//         u[T] = 0;
//         u[DT] = dt;
//
//         u = map(u);
//
//         manager.print(0, "{} {}\n", dt, u[R]);
//     }
//     manager.fflush();
//
//     return 0;
// }
//
//
//






