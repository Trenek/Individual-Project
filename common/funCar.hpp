#include <capd/capdlib.h>

using capd::autodiff::Node;

enum { X, Y, DX, DY, };
enum { CE, CS };

static
void orbitki(Node &time, Node in[], int dimIn, Node out[], int dimOut, Node param[], int noParam) {
    Node &x = in[X];
    Node &y = in[Y];
    Node &dx = in[DX];
    Node &dy = in[DY];

    Node &ce = param[CE];
    Node &cs = param[CS];

    out[X] = dx;
    out[Y] = dy;
    out[DX] = - (ce * x + cs * dy) / (((x ^ 2.0) + (y ^ 2.0)) ^ 1.5);
    out[DY] =   (cs * dx - ce * y) / (((x ^ 2.0) + (y ^ 2.0)) ^ 1.5);
}
