#include <capd/capdlib.h>

using capd::autodiff::Node;

enum { X, Y, DX, DY, };
enum { R, T, DR, DT, };
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

static
void orbitkiAngle(Node &time, Node in[], int dimIn, Node out[], int dimOut, Node param[], int noParam) {
    Node &r  = in[R];
    Node &t  = in[T];
    Node &dr = in[DR];
    Node &dt = in[DT];

    Node &ce = param[CE];
    Node &cs = param[CS];

    out[R] = dr;
    out[T] = dt;
    out[DR] = r * (dt ^ 2.0) - (ce + cs * dt) / (r ^ 2.0);
    out[DT] = cs * dr / (r ^ 4.0) - 2.0 * dr * dt / r;
}
