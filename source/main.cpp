#include <stdint.h>

#include <thread>

#include "common.h"

int main() {
    struct commonData data{};

    std::thread writer(simulate, std::ref(data));
    std::thread reader(draw, std::ref(data));

    writer.join();
    reader.join();

    return 0;
}
