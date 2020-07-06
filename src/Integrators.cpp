#include "Integrators.h"

#include <ranges>
#include <numeric>
#include <unordered_set>
#include <iostream>

namespace clothsim
{
    void backwardEulerStep(System &system, const Float dt)
    {
        Eigen::VectorXd xInitial{system.getState()};

        // Initial guess
        Eigen::VectorXd x{xInitial + dt * system.evalDerivative(xInitial)};

        Eigen::VectorXd dx{xInitial.size()};
        dx.setOnes();

        // Newton's method
        for (int i = 0; i < 10 && dx.norm() > 1e-12; ++i)
        {
            const Eigen::SparseMatrix<double> dfdx{system.evalJacobian(x)};
            const Eigen::VectorXd dxdt{system.evalDerivative(x)};

            Eigen::SparseMatrix<double> J{dfdx.rows(), dfdx.cols()};
            {
                J.setIdentity();
                J = J - dt * dfdx;
            }

            const Eigen::VectorXd b{-(x - xInitial - dt * dxdt)};

            Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
            //Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
            //Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> solver;
            solver.compute(J);

            if (solver.info() != Eigen::Success)
            {
                throw std::runtime_error("Solver failed");
            }

            dx = solver.solve(b);

            if (solver.info() != Eigen::Success)
            {
                throw std::runtime_error("Solver failed");
            }

            x += dx;
        }

        system.setState(std::move(x));
    }

    void forwardEulerStep(System &system, const Float dt)
    {
        const auto &x0 = system.getState();
        const auto dxdt = system.evalDerivative(x0);
        Eigen::VectorXd x1 = x0 + dt * dxdt;

        system.setState(std::move(x1));
    }

    void rk4Step(System &system, const Float dt)
    {
        const auto &x0 = system.getState();
        const auto k1 = system.evalDerivative(x0);

        Eigen::VectorXd xT{x0 + (0.5f * dt) * k1};

        const auto k2 = system.evalDerivative(xT);

        xT = x0 + (0.5f * dt) * k2;

        const auto k3 = system.evalDerivative(xT);

        xT = x0 + dt * k3;

        const auto k4 = system.evalDerivative(xT);

        const Eigen::VectorXd x1{x0 + dt / 6.0f * (k1 + 2.0f * k2 + 2.0f * k3 + k4)};

        system.setState(std::move(x1));
    }
} // namespace clothsim