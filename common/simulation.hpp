#include <capd/capdlib.h>

typedef long double num;

typedef capd::vectalg::Matrix<num, 0, 0> Matrix;
typedef capd::map::Map<Matrix> Map;
typedef capd::dynsys::BasicOdeSolver<Map> Solver;
typedef capd::vectalg::Vector<num, 0> Vector;

struct commonData {
    void *data;
    void *array;
};

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

    bool isAngleFound;
    bool isSecondMaxFound;
    struct timestamp secondMax;

    bool isFirstMaxFound;
    struct timestamp firstMax;

    struct timestamp last[3];

    num angle;
};

struct simulation setupSim();
void initSim(struct simulation *sim, Vector init);
void stepSim(struct simulation *sim);
