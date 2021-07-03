#ifndef CLOTHSIM_INTEGREATORS_H
#define CLOTHSIM_INTEGREATORS_H

#include "System.h"

namespace clothsim {
void backwardMidpointStep(System &system, const Float dt);
void forwardEulerStep(System &system, const Float dt);
void backwardEulerStep(System &system, const Float dt);
void rk4Step(System &system, const Float dt);
} // namespace clothsim

#endif