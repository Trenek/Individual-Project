#include <print>

#include "simulation.hpp"
#include "draw.hpp"

struct init {
    num start;
    num end;
    num inc;
};

void findAngle(struct simulation *sim, num dy0) {
    initSim(sim, { 1.0, 0.0, 0.0, dy0 });

    do {
        stepSim(sim);
    } while (false == sim->isMaximumFound);
}

static void simulation(struct commonData &data) {
    struct simulation sim = setupSim();
    struct init init = *(struct init *)data.data;

    struct thing thing[] = {
        {
            .name = "Kąt między maksymalnymi wychyleniami",
            .file = "amp.dat"
        },
    };
    constexpr size_t qThing = sizeof(thing) / sizeof(struct thing);

    cleanup(qThing, thing);

    FILE *amp = fopen("amp.dat", "w");

    findAngle(&sim, init.start);

    std::print(amp, "{} {}\n", init.start, sim.angle);
    std::fflush(amp);
    createPlot(qThing, thing);
    for (num i = init.start + init.inc; i < init.end; i += init.inc) {
        findAngle(&sim, i);

        std::print(amp, "{} {}\n", i, sim.angle);
        std::print(stdout, "{} {}\n", i, sim.angle);

        std::fflush(amp);
        std::fflush(stdout);
    }

    destroyPlot(qThing, thing);
}

int main(int argc, char **argv) {
    struct init init = {
        .start = argc > 1 ? atof(argv[1]) : -0.10,
        .end =   argc > 2 ? atof(argv[2]) :  0.10,
        .inc =   argc > 3 ? atof(argv[3]) :  0.0005
    };

    struct commonData data{
        .data = &init
    };

    simulation(data);

    return 0;
}
