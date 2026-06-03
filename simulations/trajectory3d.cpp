#include <capd/capdlib.h>

#include "fun3d.hpp"
#include "draw.hpp"

static void setGNUPlot(int id, struct thing &drawer) {
    fprintf(drawer.gnuplot, "set term qt %d size 800,600\n", id);
    fprintf(drawer.gnuplot, "set title '%s'\n", drawer.name);
    fprintf(drawer.gnuplot, "set view 60, 30, 1, 1\n");
    fprintf(drawer.gnuplot, "set xlabel 'R'\n");
    fprintf(drawer.gnuplot, "set ylabel 'DR'\n");
    fprintf(drawer.gnuplot, "set zlabel 'DT'\n");
    fprintf(drawer.gnuplot, "set pointsize 1.5\n");
    fprintf(drawer.gnuplot, "set grid\n");

    fprintf(drawer.gnuplot, "pause 1\n");
    fprintf(drawer.gnuplot, "splot \"%s\" with lines lc rgb '#000000'\n", drawer.file);
    fprintf(drawer.gnuplot, "bind 'q' 'exit'\n");
    fprintf(drawer.gnuplot, "while (1) {\n");
    fprintf(drawer.gnuplot, "    pause 1\n");
    fprintf(drawer.gnuplot, "    replot\n");
    fprintf(drawer.gnuplot, "}\n");
    fflush(drawer.gnuplot);
}

int main() {
    class gnuPlotManager manager{{
        {
            .name = "Trajektoria",
            .file = "traj1.dat",
            .setGNUPlot = setGNUPlot,
        },
    }};

    constexpr uint32_t order = 20;
    capd::LDMap f{orbitkiAngle3D, 3, 3, 2}; {
        f.setParameter(CE, 1.0);
        f.setParameter(CS, 0.00001);
    }

    capd::LDOdeSolver::StepControlType s{2, 1.0 / 0x400000};
    capd::LDOdeSolver solver{f, order, s};

    capd::LDVector u{0.0, 0.0, 0.0};
    u[R] = 1.0;
    u[DT] = -0.1;

    long double t = 0.0;

    manager.print(0, "{} {} {}\n", u[R], u[DR], u[DT]);
    manager.fflush();
    manager.initGNUPlot();

    while (t < 20) {
        u = solver(t, u);

        manager.print(0, "{} {} {}\n", u[R], u[DR], u[DT]);
    }

    return 0;
}
