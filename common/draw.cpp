#include <stdio.h>

static void setGNUPlot(FILE *gp, const char *name) {
    static int num = 1;

    fprintf(gp, "set term qt %d size 800,600\n", num);
    fprintf(gp, "set title '%s'\n", name);
    fprintf(gp, "set xlabel 'x'\n");
    fprintf(gp, "set ylabel 'y'\n");
    fprintf(gp, "set grid\n");
    fflush(gp);

    num += 1;
}

FILE* createPlot(const char *name) {
    FILE *gp = popen("gnuplot", "w");

    setGNUPlot(gp, name);

    return gp;
};

FILE* createPlotP(const char *name) {
    FILE *gp = popen("gnuplot -persist", "w");

    setGNUPlot(gp, name);

    return gp;
};

