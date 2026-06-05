#include <capd/capdlib.h>

using capd::autodiff::Node;

enum { R, DR, DT, Q };
enum { CE, CS, KSI, L };

struct InitData {
    double ce;
    double cs;

    double r;
    double dr;
    double t;
    double dt;
};

static double calcKsi(struct InitData init) {
    return init.r * init.r * init.dt + init.cs / init.r;
}

static
void orbitkiAngle2D(Node &time, Node in[], int dimIn, Node out[], int dimOut, Node param[], int noParam) {
    Node &r = in[R];
    Node &dr = in[DR];

    Node &ce = param[CE];
    Node &cs = param[CS];
    Node &ksi = param[KSI];

    Node phi = (ksi - cs / r) / (r ^ 2);

    out[R] = dr;
    out[DR] = r * (phi ^ 2) - (cs * phi + ce) / (r ^ 2);
}

static
void orbitkiAngle3D(Node &time, Node in[], int dimIn, Node out[], int dimOut, Node param[], int noParam) {
    Node &r = in[R];
    Node &dr = in[DR];

    Node &ce = param[CE];
    Node &cs = param[CS];
    Node &ksi = param[KSI];

    Node phi = (ksi - cs / r) / (r ^ 2);

    out[R] = dr;
    out[DR] = r * (phi ^ 2) - (cs * phi + ce) / (r ^ 2);
    out[DT] = (cs * dr) / (r ^ 4) - 2 * dr * phi / r;
}
