#include <print>

#include "simulation.hpp"
#include "draw.hpp"

struct data {
    num t;
    num pos[2];
};

struct init {
    num y0;
};

static void simulation(struct commonData &data) {
    struct simulation sim = setupSim();

    std::vector<struct data> temp;
    std::vector<struct data> *array = (std::vector<struct data> *)data.array;

    struct init init = *(struct init *)data.data;

    initSim(&sim, { 1.0, 0.0, 0.0, init.y0 });

    do {
        stepSim(&sim);

        temp.emplace_back((struct data) {
            .t = sim.t,
            .pos = { sim.curr[0], sim.curr[1] }
        });
    
        tryOffload(data, temp, array);
#ifdef DEBUG
        if (sim.isMaximumFound) {
            sim.isMaximumFound = false;
            std::print("\33[2K\rMax Detected at {} - {:.6}\n", sim.last[1].t, sim.angle);
        }

        std::print("\r{:} {:.6f}: x={:.6f}, y={:.6f}, dx={:.6f}, dy={:.6f}, max={:.6f}, curr={:.6f}",
            sim.isMaximumFound,
            sim.t, 
            sim.curr[0], 
            sim.curr[1], 
            sim.curr[2], 
            sim.curr[3], 
            sim.max.r, 
            sim.last[0].r
        );
#endif
    } while(sim.t < 20'000);

    offload(data, temp, array);
}

static void drawTrajectory(struct commonData &data, bool end) {
    static FILE* gp{createPlot("Trajektoria")};
    auto array = (std::vector<struct data> *)data.array;

    if (end) pclose(gp);
    else {
        std::print(gp, "plot '-' with lines title 'orbita'\n");
        for (auto& p : *array) {
            std::print(gp, "{} {}\n", p.pos[0], p.pos[1]);
        }
        std::print(gp, "e\n");

        fflush(gp);
    }
}

static void drawDistance(struct commonData &data, bool end) {
    static FILE* gp{createPlot("Wychylenie")};
    auto array = (std::vector<struct data> *)data.array;

    if (end) pclose(gp);
    else {
        std::print(gp, "plot '-' with lines title 'wychylenie'\n");
        for (auto& p : *array) {
            num curr = sqrt(p.pos[0] * p.pos[0] + p.pos[1] * p.pos[1]);

            std::print(gp, "{} {}\n", p.t, curr);
        }
        std::print(gp, "e\n");
        fflush(gp);
    }
}

static void drawSomething(struct commonData &data, bool end) {
    static FILE* gp{createPlot("Coś")};
    auto array = (std::vector<struct data> *)data.array;

    if (end) pclose(gp);
    else {
        std::print(gp, "plot '-' with lines title 'śmieszne'\n");
        for (auto& p : *array) {
            num curr = p.pos[0] * p.pos[1];

            std::print(gp, "{} {}\n", p.t, curr);
        }
        std::print(gp, "e\n");
        fflush(gp);
    }
}

int main(int argc, char **argv) {
    struct init init {
        .y0 = argc > 1 ? atof(argv[1]) : 0.06
    };

    struct commonData data{
        .data = &init
    };

    std::vector<struct data> array;
    void (*drawA[])(struct commonData &, bool) = {
        drawTrajectory,
        drawDistance,
        // drawSomething
    };
    constexpr size_t qDraw = sizeof(drawA) / sizeof(void *);
    data.array = &array;

    std::thread writer(simulation, std::ref(data));
    std::thread reader(drawFun<qDraw>, std::ref(data), drawA);

    writer.join();
    reader.join();

    return 0;
}
