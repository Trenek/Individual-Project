#include <capd/capdlib.h>

using capd::autodiff::Node;

enum { R, DR, DT, Q };
enum { CE, CS };

static
void orbitkiAngle3D(Node &time, Node in[], int dimIn, Node out[], int dimOut, Node param[], int noParam) {
    Node &r  = in[R];
    Node &dr = in[DR];
    Node &dt = in[DT];

    Node &ce = param[CE];
    Node &cs = param[CS];

    out[R] = dr;
    out[DR] = r * (dt ^ 2.0) - (ce + cs * dt) / (r ^ 2.0);
    out[DT] = cs * dr / (r ^ 4.0) - 2.0 * dr * dt / r;
}
