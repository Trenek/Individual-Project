#include <print>

#include "simulation.hpp"
#include "draw.hpp"

struct data2 {
    num y0;
    num angle;
};

static void simulation(struct commonData &data) {
    struct simulation sim = setupSim();

    std::vector<struct data2> temp;
    std::vector<struct data2> *array = (std::vector<struct data2> *)data.array;

    for (num i = 0.0514; i < 1.0; i += 0.0002) {
        initSim(&sim, { 1.0, 0.0, 0.0, i });

        do {
            stepSim(&sim);

        } while (false == sim.isMaximumFound);

        temp.emplace_back((struct data2) {
            .y0 = i,
            .angle = sim.angle
        });

        tryOffload(data, temp, array);
    }

    offload(data, temp, array);
}

FILE* createPlotP(const char *name);

static void draw(struct commonData &data, bool end) {
    static FILE* gp{createPlotP("Trajektoria")};
    auto array = (std::vector<struct data2> *)data.array;

    if (end) pclose(gp);
    else {
        std::print(gp, "plot '-' with lines title 'orbita'\n");
        for (auto& p : *array) {
            std::print(gp, "{} {}\n", p.y0, p.angle);
        }
        std::print(gp, "e\n");

        fflush(gp);
    }
}

void amplitude(struct commonData &data) {
    std::vector<data2> array;
    void (*drawA[])(struct commonData &, bool) = {
        draw,
    };
    constexpr size_t qDraw = sizeof(drawA) / sizeof(void *);

    data.array = &array;

    std::thread writer(simulation, std::ref(data));
    std::thread reader(drawFun<qDraw>, std::ref(data), drawA);

    writer.join();
    reader.join();
}
