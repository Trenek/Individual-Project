#include <stdio.h>

struct thing {
    FILE *fileDesc;
    const char *name;
    const char *file;
};

void createPlot(size_t qThing, struct thing thing[]);
void createPlotP(size_t qThing, struct thing thing[]);
void createPlotU(size_t qThing, struct thing thing[]);
void destroyPlot(size_t qThing, struct thing thing[]);

void cleanup(size_t qThing, struct thing thing[]);
