## clothsim

Simple mass-spring system simulation inspired by Baraff & Witkins paper "Large Steps in Cloth Simulation". Features forward and backward euler integrators, the Runge-Kutta method and the implicit midpoint integrator.

Built with Magnum + Corrade v2020.06, ImGui 1.73 and Eigen3.To build:

```
mkdir build
cd build
cmake -DImGui_INCLUDE_DIR=PATH/TO/IMGUI ..
make -j
```

