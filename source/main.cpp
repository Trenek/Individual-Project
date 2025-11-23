#include <math.h>

#include "common.h"
#include "sims.hpp"

int main(int argc, char **argv) {
    struct commonData data{
        .init = argc > 1 ? atof(argv[1]) : 0.06
    };

    trajectory(data);

    return 0;
}
