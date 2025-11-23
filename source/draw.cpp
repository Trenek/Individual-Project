#include <stdio.h>
#include <math.h>

FILE* createPlot(const char *name) {
    static int num = 1;
    FILE *gp = popen("gnuplot", "w");

    fprintf(gp, "set term qt %d size 800,600\n", num);
    fprintf(gp, "set title '%s'\n", name);
    fprintf(gp, "set xlabel 'x'\n");
    fprintf(gp, "set ylabel 'y'\n");
    fprintf(gp, "set grid\n");
    fflush(gp);

    num += 1;

    return gp;
};

FILE* createPlotP(const char *name) {
    static int num = 1;
    FILE *gp = popen("gnuplot -persist", "w");

    fprintf(gp, "set term qt %d size 800,600\n", num);
    fprintf(gp, "set title '%s'\n", name);
    fprintf(gp, "set xlabel 'x'\n");
    fprintf(gp, "set ylabel 'y'\n");
    fprintf(gp, "set grid\n");
    fflush(gp);

    num += 1;

    return gp;
};

