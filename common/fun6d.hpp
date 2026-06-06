#include <capd/capdlib.h>

using capd::autodiff::Node;

enum { X, Y, Z, DX, DY, DZ };
enum { CE, CS };

static
void orbitkiAngle6D(Node &time, Node in[], int dimIn, Node out[], int dimOut, Node param[], int noParam) {
    Node &x = in[X];
    Node &y = in[Y];
    Node &z = in[Z];
    Node &dx = in[DX];
    Node &dy = in[DY];
    Node &dz = in[DZ];

    Node &ce = param[CE];
    Node &cs = param[CS];

    Node r2 = (x ^ 2) + (y ^ 2) + (z ^ 2);
    Node r3 = (r2 ^ 1.5);
    Node r5 = (r2 ^ 2.5);

    out[X] = dx;
    out[Y] = dy;
    out[Z] = dz;
    out[DX] = 3 * cs * z * (dy * z - dz * y) / r5 - (cs * dy + ce * x) / r3;
    out[DY] = 3 * cs * z * (dz * x - dx * z) / r5 + (cs * dx - ce * y) / r3;
    out[DZ] = 3 * cs * z * (dx * y - dy * x) / r5 - ce * z / r3;
}
