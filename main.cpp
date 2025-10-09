#include <cstdio>
#include "capd/capdlib.h"

int main() {
  // define an instance of class DMap that describes the vector field
  capd::DMap pendulum(
    "time:t;"
    "var:"
        "x,"
        "y,"
        "dx,"
        "dy;"
    "par:"
        "ce,"
        "cs;"
    "fun:"
        "dx,"
        "dy,"
        "-(ce*x+cs*dy)/((x^2+y^2)^1.5),"
        "(cs*dx-ce*y)/((x^2+y^2)^1.5);"
  );
  pendulum.setParameter("ce", 1.0);
  pendulum.setParameter("cs", 0.00001);

  // define an instance of ODE solver
  uint32_t order = 100;
  capd::DOdeSolver solver{pendulum, order};

  // We set a fixed time step - this disables automatic step control.
  // If one really wants to use fixed time step, it is recommended to put a floating point number with many tailing zeros in the mantissa.
  solver.setStep(0.00001);

  // specify initial condition
  capd::DVector u{1000.0, 1000.0, 10.0, 0.0};
  double t = 0.0;

  // use one step Taylor method to integrate
  do {
    u = solver(t, u);
    printf("\rt=%6f, x=%6f, y=%6f, dx=%6f, dy=%6f", t, u[0], u[1], u[2], u[3]);
  } while(t < 20.0);
  printf("\n");

  return 0;
}
