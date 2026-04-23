#include "simulation.hpp"
#include "draw.hpp"

int main(int argc, char **argv) {
    num y0 = argc > 1 ? atof(argv[1]) : 0.06;

    struct simulation sim = setupSim(0.00001);

    class gnuPlotManager manager{{
        {
            .name = "Trajektoria", 
            .file = "traj.dat"
        },
        { 
            .name = "Wychylenie", 
            .file = "wych.dat"
        },
    }, true};
    
    initSim(&sim, { 1.0, 0.0, 0.0, y0 });

    stepSim(&sim);

    manager.print(0, "{} {}\n", sim.curr[0], sim.curr[1]);
    manager.print(1, "{} {}\n", sim.t, sqrt(sim.curr[0] * sim.curr[0] + sim.curr[1] * sim.curr[1]));
    manager.fflush();

    do {
        stepSim(&sim);

        manager.print(0, "{} {}\n", sim.curr[0], sim.curr[1]);
        manager.print(1, "{} {}\n", sim.t, sqrt(sim.curr[0] * sim.curr[0] + sim.curr[1] * sim.curr[1]));
    } while(sim.t < 20'000);

    return 0;
}
