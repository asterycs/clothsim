#include "Integrators.h"

#include <iostream>
#include <numeric>
#include <unordered_set>

namespace clothsim {
void backwardMidpointStep(System &system, const Float dt) {
  const System::Vector xInitial{system.getState()};

  // Initial guess using forward Euler
  System::Vector x{xInitial + dt * system.evalDerivative(xInitial)};

  System::Vector dx{xInitial.size()};
  dx.setOnes();

  // Newton's method
  for (int i = 0; i < 10 && dx.norm() > 1e-12f; ++i) {
    const System::SparseMatrix dfdx{system.evalJacobian(0.5f * (x + xInitial))};
    const System::Vector dxdt{system.evalDerivative(0.5f * (x + xInitial))};

    System::SparseMatrix J{dfdx.rows(), dfdx.cols()};
    {
      J.setIdentity();
      J = J - dt * dfdx * 0.5f * J;
    }

    const System::Vector b{-(x - xInitial - dt * dxdt)};

    Eigen::SparseLU<System::SparseMatrix> solver;
    solver.compute(J);

    if (solver.info() != Eigen::Success) {
      throw std::runtime_error("Solver failed");
    }

    dx = solver.solve(b);

    if (solver.info() != Eigen::Success) {
      throw std::runtime_error("Solver failed");
    }

    x += dx;
  }

  system.setState(std::move(x));
}

void backwardEulerStep(System &system, const Float dt) {
  const System::Vector xInitial{system.getState()};

  // Initial guess using forward Euler
  System::Vector x{xInitial + dt * system.evalDerivative(xInitial)};

  System::Vector dx{xInitial.size()};
  dx.setOnes();

  // Newton's method
  for (int i = 0; i < 10 && dx.norm() > 1e-12f; ++i) {
    const System::SparseMatrix dfdx{system.evalJacobian(x)};
    const System::Vector dxdt{system.evalDerivative(x)};

    System::SparseMatrix J{dfdx.rows(), dfdx.cols()};
    {
      J.setIdentity();
      J = J - dt * dfdx;
    }

    const System::Vector b{-(x - xInitial - dt * dxdt)};

    Eigen::SparseLU<System::SparseMatrix> solver;
    solver.compute(J);

    if (solver.info() != Eigen::Success) {
      throw std::runtime_error("Solver failed");
    }

    dx = solver.solve(b);

    if (solver.info() != Eigen::Success) {
      throw std::runtime_error("Solver failed");
    }

    x += dx;
  }

  system.setState(std::move(x));
}

void forwardEulerStep(System &system, const Float dt) {
  const auto &x0 = system.getState();
  const auto dxdt = system.evalDerivative(x0);
  System::Vector x1 = x0 + dt * dxdt;

  system.setState(std::move(x1));
}

void rk4Step(System &system, const Float dt) {
  const auto &x0{system.getState()};
  const auto k1{system.evalDerivative(x0)};

  System::Vector xT{x0 + (0.5f * dt) * k1};

  const auto k2{system.evalDerivative(xT)};

  xT = x0 + (0.5f * dt) * k2;

  const auto k3{system.evalDerivative(xT)};

  xT = x0 + dt * k3;

  const auto k4{system.evalDerivative(xT)};

  const System::Vector x1{x0 + dt / 6.0f * (k1 + 2.0f * k2 + 2.0f * k3 + k4)};

  system.setState(std::move(x1));
}
} // namespace clothsim