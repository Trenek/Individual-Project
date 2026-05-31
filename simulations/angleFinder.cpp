#include <capd/capdlib.h>

#include "fun.hpp"
#include "draw.hpp"

long double toDeg(long double rad) {
    return 180.0 * rad / std::numbers::pi;
}

capd::LDVector Newton(capd::LDVector u, capd::LDPoincareMap map, long double angle) {
    capd::vectalg::MaxNorm<capd::LDVector, capd::LDMatrix> maxNorm;

    capd::LDMatrix Dphi(4, 4);
    capd::LDVector P = map(u, Dphi);
    capd::LDMatrix DP = map.computeDP(P, Dphi);

    long double curr = toDeg(P[T]);
    long double error = std::remainder(curr - angle, 360.0);

    size_t i = 0;

    while (std::abs(error) >= 1e-14) {
        std::print("Iter {}: DT = {:.5e} Curr {:.2e}° Error {:.2e}°\n", i++, u[DT], std::remainder(curr, 360.0), error);
        std::fflush(stdout);
        
        u[DT] -= std::clamp(error / toDeg(DP[T][DT]), -0.01l, 0.01l);
        COUT(u);

        P = map(u, Dphi);
        DP = map.computeDP(P, Dphi);
        curr = toDeg(P[T]);
        error = std::remainder(curr - angle, 360.0);
    }

    return u;
}

int main() {
    class gnuPlotManager manager{{
        {
            .name = "Trajektoria",
            .file = "foundTraj.dat",

            .xName = "x",
            .yName = "y",
        },
    }};

    constexpr uint32_t order = 20;
    capd::LDMap f{orbitkiAngle, 4, 4, 2}; {
        f.setParameter(CE, 1.0);
        f.setParameter(CS, 0.00001);
    }

    capd::LDOdeSolver solver{f, order}; {
        solver.setStep(0.000001);
    }

    capd::LDCoordinateSection section{4, DR, 0.0};
    capd::LDPoincareMap map{solver, section, capd::poincare::PlusMinus};
    map.setMaxReturnTime(10);

    capd::LDVector u{0.0, 0.0, 0.0, 0.0};
    u[R] = 1.0;
    u[DT] = -0.03;

    u = Newton(u, map, 2 * 360 / 7.0);

    long double t = 0.0;

    manager.print(0, "{} {}\n", u[0] * cos(u[1]), u[0] * sin(u[1]));
    manager.fflush();
    manager.initGNUPlot();

    while (t < 200) {
        u = solver(t, u);
        manager.print(0, "{} {}\n", u[0] * cos(u[1]), u[0] * sin(u[1]));
    }

    return 0;
}
