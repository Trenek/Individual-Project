#include <unistd.h>

#include "draw.hpp"

void setGNUPlotUnwrap(int num, struct thing &thing) {
    fprintf(thing.gnuplot, "set term qt %d size 800,600\n", num);
    fprintf(thing.gnuplot, "set title '%s'\n", thing.name);
    fprintf(thing.gnuplot, "set xlabel 'x'\n");
    fprintf(thing.gnuplot, "set ylabel 'y'\n");
    fprintf(thing.gnuplot, "bind 'q' 'exit'\n");
    fprintf(thing.gnuplot, "set grid\n");

    fprintf(thing.gnuplot, ""
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

    fprintf(thing.gnuplot, ""
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
    fflush(thing.gnuplot);
}

static void setGNUPlot(int num, struct thing &thing) {
    fprintf(thing.gnuplot, "set term qt %d size 800,600\n", num);
    fprintf(thing.gnuplot, "set title '%s'\n", thing.name);
    fprintf(thing.gnuplot, "set xlabel 'x'\n");
    fprintf(thing.gnuplot, "set ylabel 'y'\n");
    fprintf(thing.gnuplot, "set grid\n");

    fprintf(thing.gnuplot, "bind 'q' 'exit'\n");
    fprintf(thing.gnuplot, "pause 1\n");
    fprintf(thing.gnuplot, "plot \"%s\" with lines\n", thing.file);
    fprintf(thing.gnuplot, "while (1) {\n");
    fprintf(thing.gnuplot, "    pause 1\n");
    fprintf(thing.gnuplot, "    replot\n");
    fprintf(thing.gnuplot, "}\n");
    fflush(thing.gnuplot);
}

void gnuPlotManager::fflush() {
    for (const auto &drawer : this->drawers) {
        std::fflush(drawer.dataFile);
    }
}

void gnuPlotManager::removeData() {
    for (const auto &drawer : this->drawers) {
        std::remove(drawer.file);
    }
}

void gnuPlotManager::initGNUPlot() {
    size_t id = 0;

    if (this->isEnabled == false) {
        for (auto &drawer : this->drawers) {
            if (drawer.setGNUPlot) {
                drawer.setGNUPlot(id++, drawer);
            }
            else {
                setGNUPlot(id++, drawer);
            }
        }
    }

    this->isEnabled = true;
}

gnuPlotManager::gnuPlotManager(std::vector<struct thing> &&array, bool init) : drawers(std::move(array)) {
    removeData();

    sleep(4);

    for (auto &drawer : this->drawers) {
        drawer.dataFile = fopen(drawer.file, "w");
        drawer.gnuplot = popen("gnuplot", "w");
    }

    if (init) initGNUPlot();
}

gnuPlotManager::~gnuPlotManager() {
    for (auto &drawer : this->drawers) {
        fclose(drawer.dataFile);
        pclose(drawer.gnuplot);
    }
}
