#include <print>

#include "simulation.hpp"
#include "draw.hpp"

struct init {
    num y0;
};

static void simulation(struct commonData &data) {
    struct simulation sim = setupSim();
    struct init init = *(struct init *)data.data;

    struct thing thing[] = {
        {
            .name = "Trajektoria", 
            .file = "traj.dat"
        },
        { 
            .name = "Wychylenie", 
            .file = "wych.dat"
        },
        // { 
        //     .name = "Cos",
        //     .file = "cos.dat"
        // }
    };
    constexpr size_t qThing = sizeof(thing) / sizeof(struct thing);

    cleanup(qThing, thing);

    FILE *traj = fopen("traj.dat", "w");
    FILE *wych = fopen("wych.dat", "w");
    // FILE *cos = fopen("cos.dat", "w");
    
    initSim(&sim, { 1.0, 0.0, 0.0, init.y0 });

    stepSim(&sim);

    std::print(traj, "{} {}\n", sim.curr[0], sim.curr[1]);
    std::print(wych, "{} {}\n", sim.t, sqrt(sim.curr[0] * sim.curr[0] + sim.curr[1] * sim.curr[1]));
    std::fflush(traj);
    std::fflush(wych);
    // std::print(cos, "{} {}\n", sim.t, sim.curr[0] * sim.curr[1]);
    createPlot(qThing, thing);
    do {
        stepSim(&sim);

        std::print(traj, "{} {}\n", sim.curr[0], sim.curr[1]);
        std::print(wych, "{} {}\n", sim.t, sqrt(sim.curr[0] * sim.curr[0] + sim.curr[1] * sim.curr[1]));
        // std::print(cos, "{} {}\n", sim.t, sim.curr[0] * sim.curr[1]);
    } while(sim.t < 20'000);

    fclose(traj);
    fclose(wych);
    // fclose(cos);

    destroyPlot(qThing, thing);
}

int main(int argc, char **argv) {
    struct init init {
        .y0 = argc > 1 ? atof(argv[1]) : 0.06
    };

    struct commonData data{
        .data = &init
    };

    simulation(data);

    return 0;
}
