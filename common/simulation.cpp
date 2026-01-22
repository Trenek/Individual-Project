#include "simulation.hpp"

static num toDeg(num rad) {
    return 180 * rad / M_PI;
}

static num getAngle(Vector pos1, Vector pos2) {
    num x1 = pos1[0];
    num x2 = pos2[0];
    num y1 = pos1[1];
    num y2 = pos2[1];

    num r1 = sqrt(x1 * x1 + y1 * y1);
    num r2 = sqrt(x2 * x2 + y2 * y2);

    num rad = acos((x1 * x2 + y1 * y2) / (r1 * r2));

    return toDeg(rad);
}

struct simulation setupSim() {
    struct simulation result = {
        .map = new Map(
            "time:t;"
            "var:"
                "x,"
                "y,"
                "dx,"
                "dy;"
            "par:"
                "ce,"
                "cs;"
            "fun:"
                "dx,"
                "dy,"
                "-(ce*x+cs*dy)/((x^2+y^2)^1.5),"
                "(cs*dx-ce*y)/((x^2+y^2)^1.5);"
        )
    };

    result.map->setParameter("ce", 1.0);
    result.map->setParameter("cs", 0.00001);

    uint32_t order = 10;

    result.solver = new Solver{*result.map, order};
    result.solver->setStep(0.0000001);

    return result;
}

void initSim(struct simulation *sim, Vector init) {
    struct timestamp initial = {
        .pos = init,
        .r = 1,
        .t = 0
    };

    sim->curr = init;
    sim->t = 0.0;
    sim->max = initial;
    sim->last[0] = initial;
    sim->last[1] = initial;
    sim->last[2] = initial;
    sim->isMaximumFound = 0;
    sim->angle = 0;
}

void stepSim(struct simulation *sim) {
    sim->curr = (*sim->solver)(sim->t, sim->curr);

    sim->last[2] = sim->last[1];
    sim->last[1] = sim->last[0];
    sim->last[0] = {
        .pos = sim->curr,
        .r = sqrt(pow(sim->curr[0], 2) + pow(sim->curr[1], 2)),
        .t = sim->t
    };

    if (sim->last[1].r > sim->last[2].r && sim->last[1].r >= sim->last[0].r) {
        sim->isMaximumFound = true;
        sim->angle = getAngle(sim->max.pos, sim->last[1].pos);

        sim->max = {
            .pos = sim->last[1].pos,
            .r = std::max(sim->last[1].r, sim->max.r),
            .t = sim->last[1].t
        };
    }
}
