#include <print>

#include "simulation.hpp"
#include "draw.hpp"

struct data {
    num y0;
    num angle;
};

struct init {
    num start;
    num end;
    num inc;
};

static void simulation(struct commonData &data) {
    struct simulation sim = setupSim();

    std::vector<struct data> temp;
    std::vector<struct data> *array = (std::vector<struct data> *)data.array;

    struct init init = *(struct init *)data.data;

    for (num i = init.start; i < init.end; i += init.inc) {
        initSim(&sim, { 1.0, 0.0, 0.0, i });

        do {
            stepSim(&sim);
        } while (false == sim.isMaximumFound);

        temp.emplace_back((struct data) {
            .y0 = i,
            .angle = sim.angle
        });

        tryOffload(data, temp, array);
    }

    offload(data, temp, array);
}

static void draw(struct commonData &data, bool end) {
    static FILE* gp{createPlotP("Kąt między maksymalnymi wychyleniami")};
    auto array = (std::vector<struct data> *)data.array;

    if (end) pclose(gp);
    else {
        std::print(gp, "plot '-' with lines title 'kąt'\n");
        for (auto& p : *array) {
            std::print(gp, "{} {}\n", p.y0, p.angle);
        }
        std::print(gp, "e\n");

        fflush(gp);
    }
}

int main(int argc, char **argv) {
    struct init init = {
        .start = argc > 1 ? atof(argv[1]) : 0.06,
        .end = argc > 2 ? atof(argv[2]) : 1,
        .inc = argc > 3 ? atof(argv[3]) : 0.005
    };

    struct commonData data{
        .data = &init
    };

    std::vector<struct data> array;
    void (*drawA[])(struct commonData &, bool) = {
        draw,
    };
    constexpr size_t qDraw = sizeof(drawA) / sizeof(void *);

    data.array = &array;

    std::thread writer(simulation, std::ref(data));
    std::thread reader(drawFun<qDraw>, std::ref(data), drawA);

    writer.join();
    reader.join();

    return 0;
}
