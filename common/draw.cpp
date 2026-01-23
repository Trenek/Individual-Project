#include <filesystem>
#include <unistd.h>

#include "draw.hpp"

static void setGNUPlotUnwrap(struct thing thing) {
    static int num = 1;

    fprintf(thing.fileDesc, "set term qt %d size 800,600\n", num);
    fprintf(thing.fileDesc, "set title '%s'\n", thing.name);
    fprintf(thing.fileDesc, "set xlabel 'x'\n");
    fprintf(thing.fileDesc, "set ylabel 'y'\n");
    fprintf(thing.fileDesc, "set grid\n");

    fprintf(thing.fileDesc, ""
        "prev = NaN\n"
        "wrap = 0\n"
        "unwrap(y) = ( "
            "(prev == prev) ? "
                "(wrap = wrap + round((y - prev)/360.0)) : "
                "(wrap = 0), "
            "prev = y, "
            "y - 360*wrap "
        ")\n"
    );

    fprintf(thing.fileDesc, ""
        "pause 1\n"
        "plot \"%s\" using 1:(unwrap($2)) with lines\n"
        "while (1) {\n"
        "    pause 1\n"
        "    prev = NaN\n"
        "    wrap = 0\n"
        "    replot\n"
        "}\n",
        thing.file
    );
    fflush(thing.fileDesc);

    num += 1;
}

static void setGNUPlot(struct thing thing) {
    static int num = 1;

    fprintf(thing.fileDesc, "set term qt %d size 800,600\n", num);
    fprintf(thing.fileDesc, "set title '%s'\n", thing.name);
    fprintf(thing.fileDesc, "set xlabel 'x'\n");
    fprintf(thing.fileDesc, "set ylabel 'y'\n");
    fprintf(thing.fileDesc, "set grid\n");

    fprintf(thing.fileDesc, "pause 1\n");
    fprintf(thing.fileDesc, "plot \"%s\" with lines\n", thing.file);
    fprintf(thing.fileDesc, "while (1) {\n");
    fprintf(thing.fileDesc, "    pause 1\n");
    fprintf(thing.fileDesc, "    replot\n");
    fprintf(thing.fileDesc, "}\n");
    fflush(thing.fileDesc);

    num += 1;
}

void cleanup(size_t qThing, struct thing thing[]) {
    for (size_t i = 0; i < qThing; i += 1) {
        std::remove(thing[i].file);
    }
    sleep(4);
}

void createPlot(size_t qThing, struct thing thing[]) {
    for (size_t i = 0; i < qThing; i += 1) {
        thing[i].fileDesc = popen("gnuplot", "w");

        setGNUPlot(thing[i]);
    }
};

void createPlotP(size_t qThing, struct thing thing[]) {
    for (size_t i = 0; i < qThing; i += 1) {
        thing[i].fileDesc = popen("gnuplot -persist", "w");

        setGNUPlot(thing[i]);
    }
};

void createPlotU(size_t qThing, struct thing thing[]) {
    for (size_t i = 0; i < qThing; i += 1) {
        thing[i].fileDesc = popen("gnuplot", "w");

        setGNUPlotUnwrap(thing[i]);
    }
}

void destroyPlot(size_t qThing, struct thing thing[]) {
    for (size_t i = 0; i < qThing; i += 1) {
        fclose(thing[i].fileDesc);
    }
}
