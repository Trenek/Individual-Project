#include <print>

#include <capd/capdlib.h>

#include "common.h"

num toDeg(num rad) {
    return 180 * rad / M_PI;
}

static num getAngle(num pos1[2], num pos2[2]) {
    num x1 = pos1[0];
    num x2 = pos2[0];
    num y1 = pos1[1];
    num y2 = pos2[1];

    num r1 = sqrt(x1 * x1 + y1 * y1);
    num r2 = sqrt(x2 * x2 + y2 * y2);

    num rad = acos((x1 * x2 + y1 * y2) / (r1 * r2));

    return toDeg(rad);
}

typedef capd::vectalg::Matrix<num, 0, 0> Matrix;
typedef capd::map::Map<Matrix> Map;
typedef capd::dynsys::BasicOdeSolver<Map> Solver;
typedef capd::vectalg::Vector<num, 0> Vector;

struct timestamp {
    Vector pos;
    num r;
    num t;
};

void simulate(struct commonData &data) {
    Map pendulum(
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
    );
    pendulum.setParameter("ce", 1.0);
    pendulum.setParameter("cs", 0.00001);

    uint32_t order = 100;

    Solver solver{pendulum, order};

    solver.setStep(0.00001);

    Vector curr{1.0, 0.0, 0.0, data.init};
    num t = 0.0;

    struct timestamp max = {
        .pos = curr,
        .r = 1
    };

    struct timestamp last[3] = { 
        { curr, 1, 0 }, 
        { curr, 1, 0 }, 
        { curr, 1, 0 }, 
    };

    std::vector<struct data> temp;

    do {
        curr = solver(t, curr);

        last[2] = last[1];
        last[1] = last[0];
        last[0] = {
            .pos = curr,
            .r = sqrt(curr[0] * curr[0] + curr[1] * curr[1]),
            .t = t
        };

        temp.emplace_back((struct data) {
            .t = t,
            .pos = { curr[0], curr[1] }
        });
 
        if (last[1].r >= last[2].r && last[1].r >= last[0].r) {
            std::print("\33[2K\rMax Detected at {} - {:.6}\n", last[1].t, getAngle(
                (num[]) { max.pos[0], max.pos[1] }, 
                (num[]) { last[1].pos[0], last[1].pos[1] }
            ));

            max = {
                .pos = last[1].pos,
                .r = std::max(last[1].r, max.r)
            };
        }

        if (data.isDrawOrdered == false && data.access.try_lock()) {
            data.isDrawOrdered = true;

            for(auto &e : temp) {
                data.array.push_back(e);
            }
            temp.clear();
            data.access.unlock();

            data.drawOrderMessager.notify_all();
        }
        std::print("\r{:.6f}: x={:.6f}, y={:.6f}, dx={:.6f}, dy={:.6f}, max={:.6f}, curr={:.6f}", t, curr[0], curr[1], curr[2], curr[3], max.r, last[0].r);
    } while(t < 20'000);

    data.access.lock();
    data.isFinished = true;
    data.isDrawOrdered = true;

    for(auto &e : temp) {
        data.array.push_back(e);
    }

    temp.clear();
    data.access.unlock();

    data.drawOrderMessager.notify_all();
}
