#include <stdint.h>
#include <math.h>

#include <thread>

#include "common.h"

int main(int argc, char **argv) {
    struct commonData data{
        .init = argc > 1 ? atof(argv[1]) : 0.06
    };

    std::thread writer(simulate, std::ref(data));
    std::thread reader(draw, std::ref(data));

    writer.join();
    reader.join();

    return 0;
}
