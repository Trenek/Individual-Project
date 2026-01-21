#include <print>

#include "simulation.hpp"
#include "draw.hpp"

struct state {
    num angle;
    num dy0;
};

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

int getWhole(num curr, num prev) {
    int whole = 2;
    int maxWhole = 100;

    while (curr < 360.0 / whole && prev < 360.0 / whole && whole < maxWhole) whole += 1;
    if    (curr > 360.0 / whole && prev > 360.0 / whole) whole = -1;
    if    (whole >= maxWhole) whole = -1;

    return whole;
}

void printWait() {
    static int a = 0;
    const char *array[] = {
        ".",
        "..",
        "...",
    };

    std::print(stdout, "Szukam aproksymacji{}\r", array[a]);
    std::fflush(stdout);
    a += 1;
    a %= 3;
}

void findBetterApproximation(struct simulation *sim, struct state left, struct state right, int whole) {
    struct state inBetween = {
        .dy0 = (left.dy0 + right.dy0) / 2
    };

    if (fabs(360.0 / whole - left.angle) > 10e-5 && fabs(360.0 / whole - right.angle)) {
        printWait();
        findAngle(sim, inBetween.dy0);
        inBetween.angle = sim->angle;

        if (sim->angle < 360.0 / whole) {
            findBetterApproximation(sim, inBetween, right, whole);
        }
        else {
            findBetterApproximation(sim, left, inBetween, whole);
        }
    }
    else {
        if (fabs(360.0 / whole - left.angle) <= 10e-5) {
            std::print("Found {} velocity, {} angle\n", left.dy0, left.angle);
        }
        else {
            std::print("Found {} velocity, {} angle\n", right.dy0, right.angle);
        }
    }
}

void findCyclic(struct simulation *sim, struct state curr, struct state prev) {
    int whole = getWhole(curr.angle, prev.angle);
    num dy0 = (curr.dy0 + prev.dy0) / 2;

    if (whole > 0) {
        printWait();
        if (curr.angle < 360.0 / whole) {
            findBetterApproximation(sim, curr, prev, whole);
        }
        else {
            findBetterApproximation(sim, prev, curr, whole);
        }
    }
    else {
        std::print("Nie jestem wybrancem :((\n");
    }
}

static void simulation(struct commonData &data) {
    struct simulation sim = setupSim();

    std::vector<struct state> temp;
    std::vector<struct state> *array = (std::vector<struct state> *)data.array;

    struct init init = *(struct init *)data.data;
    num prevAngle = 0;
    num nextPrevAngle = 0;

    findAngle(&sim, init.start);
    temp.emplace_back((struct state) {
        .angle = sim.angle,
        .dy0 = init.start,
    });

    tryOffload(data, temp, array);
    nextPrevAngle = sim.angle;
    for (num i = init.start + init.inc; i < init.end; i += init.inc) {
        prevAngle = nextPrevAngle;
        findAngle(&sim, i);
        nextPrevAngle = sim.angle;
        temp.emplace_back((struct state) {
            .angle = sim.angle,
            .dy0 = i,
        });

        findCyclic(&sim,
            (struct state) { .angle = sim.angle, .dy0 = i }, 
            (struct state) { .angle = prevAngle, .dy0 = i - init.inc }
        );

        tryOffload(data, temp, array);
    }

    offload(data, temp, array);
}

static void draw(struct commonData &data, bool end) {
    static FILE* gp{createPlotP("Kąt między maksymalnymi wychyleniami")};
    auto array = (std::vector<struct state> *)data.array;

    if (end) pclose(gp);
    else {
        std::print(gp, "plot '-' with lines title 'kąt'\n");
        for (auto& p : *array) {
            std::print(gp, "{} {}\n", p.dy0, p.angle);
        }
        std::print(gp, "e\n");

        fflush(gp);
    }
}

int main(int argc, char **argv) {
    struct init init = {
        .start = argc > 1 ? atof(argv[1]) : 0.06,
        .end = argc > 2 ? atof(argv[2]) : 1.0,
        .inc = argc > 3 ? atof(argv[3]) : 0.005
    };

    struct commonData data{
        .data = &init
    };

    std::vector<struct state> array;
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
