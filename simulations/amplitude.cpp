#include "simulation.hpp"
#include "draw.hpp"

struct init {
    num start;
    num end;
    num inc;
};

static void findAngle(struct simulation *sim, num dy0) {
    initSim(sim, { 1.0, 0.0, 0.0, dy0 });

    do {
        stepSim(sim);
    } while (false == sim->isAngleFound);
}

int main(int argc, char **argv) {
    struct init init = {
        .start = argc > 1 ? atof(argv[1]) : -0.10,
        .end =   argc > 2 ? atof(argv[2]) :  0.10,
        .inc =   argc > 3 ? atof(argv[3]) :  0.005
    };

    struct simulation sim = setupSim(0.0000001);
    class gnuPlotManager manager{{
        {
            .name = "Kąt między maksymalnymi wychyleniami",
            .file = "amp.dat",
            .setGNUPlot = setGNUPlotUnwrap,
        },
    }};

    findAngle(&sim, init.start);

    manager.print(0, "{} {}\n", init.start, sim.angle);
    manager.fflush();
    manager.initGNUPlot();
    for (num i = init.start + init.inc; i < init.end; i += init.inc) {
        findAngle(&sim, i);

        manager.print(0, "{} {}\n", i, sim.angle);
        manager.fflush();
    }

    return 0;
}
