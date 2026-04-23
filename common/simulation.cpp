#include "simulation.hpp"

static num toDeg(num rad) {
    return 180 * rad / M_PI;
}

static num getAngle(Vector pos1, Vector pos2) {
    num x1 = pos1[0];
    num y1 = pos1[1];
    num x2 = pos2[0];
    num y2 = pos2[1];

    num dot = x1 * x2 + y1 * y2;
    num cross = x1 * y2 - y1 * x2;

    return(toDeg(atan2(cross, dot)));
}

#define x 0
#define y 1
#define dx 2
#define dy 3
#define ce 0
#define cs 1

void orbitki(
    capd::autodiff::Node &time, 
    capd::autodiff::Node in[],
    int dimIn,
    capd::autodiff::Node out[], 
    int dimOut, 
    capd::autodiff::Node param[],
    int noParam) {
    out[x] = in[dx];
    out[y] = in[dy];
    out[dx] = - (param[ce] * in[x] + param[cs] * in[dy]) / (((in[x] ^ 2.0) + (in[y] ^ 2.0)) ^ 1.5);
    out[dy] =   (param[cs] * in[dx] - param[ce] * in[y]) / (((in[x] ^ 2.0) + (in[y] ^ 2.0)) ^ 1.5);
}

struct simulation setupSim(double step) {
    struct simulation result = {
        .map = new Map(orbitki, 4, 4, 2)
    };

    result.map->setParameter(ce, 1.0);
    result.map->setParameter(cs, 0.00001);

    uint32_t order = 10;

    result.solver = new Solver{*result.map, order};
    result.solver->setStep(step);

    return result;
}

#undef ce
#undef cs
#undef x
#undef y
#undef dx
#undef dy

void initSim(struct simulation *sim, Vector init) {
    struct timestamp initial = {
        .pos = init,
        .r = 1,
        .t = 0
    };
    struct timestamp nan = {
        .pos = {
            NAN, NAN
        },
        .r = NAN,
        .t = NAN
    };

    sim->curr = init;
    sim->t = 0.0;
    sim->last[0] = nan;
    sim->last[1] = nan;
    sim->last[2] = nan;
    sim->firstMax = nan;
    sim->secondMax = nan;
    sim->isSecondMaxFound = false;
    sim->isFirstMaxFound = false;
    sim->isAngleFound = false;
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
        if (sim->isFirstMaxFound) {
            sim->isSecondMaxFound = true;
            sim->secondMax = sim->last[1];

            sim->isAngleFound = true;
            sim->angle = getAngle(sim->firstMax.pos, sim->secondMax.pos);
        }
        else {
            sim->isFirstMaxFound = true;
            sim->firstMax = sim->last[1];
        }
    }

}
