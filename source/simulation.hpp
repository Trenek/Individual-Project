#include <capd/capdlib.h>

#include "common.h"

typedef capd::vectalg::Matrix<num, 0, 0> Matrix;
typedef capd::map::Map<Matrix> Map;
typedef capd::dynsys::BasicOdeSolver<Map> Solver;
typedef capd::vectalg::Vector<num, 0> Vector;

struct timestamp {
    Vector pos;
    num r;
    num t;
};

struct simulation {
    Map *map;
    Solver *solver;
    Vector curr;
    num t;

    struct timestamp max;
    struct timestamp last[3];

    bool isMaximumFound;
    num angle;
};

struct simulation setupSim();
void initSim(struct simulation *sim, Vector init);
void stepSim(struct simulation *sim);

void tryOffload(struct commonData &data, auto &src, auto &dest) {
    if (data.isDrawOrdered == false && data.access.try_lock()) {
        data.isDrawOrdered = true;

        for(auto &e : src) {
            dest->push_back(e);
        }
        src.clear();
        data.access.unlock();

        data.drawOrderMessager.notify_all();
    }
}

void offload(struct commonData &data, auto &src, auto &dest) {
    data.access.lock();
    data.isFinished = true;
    data.isDrawOrdered = true;

    for(auto &e : src) {
        dest->push_back(e);
    }
    src.clear();

    data.access.unlock();

    data.drawOrderMessager.notify_all();
}
