#include "Oscillator.h"

#include <Corrade/Containers/Array.h>

#include <Magnum/EigenIntegration/Integration.h>

#include <cassert>
#include <iostream>

namespace clothsim
{
    Oscillator::Oscillator(PhongIdShader &phongShader,
                           VertexMarkerShader &vertexShader,
                           Object3D &parent,
                           Magnum::SceneGraph::DrawableGroup3D &drawableGroup) : System(phongShader,
                                                                                        vertexShader,
                                                                                        parent,
                                                                                        drawableGroup)
    {
        reset();
    }

    Oscillator::~Oscillator()
    {
    }

    void Oscillator::reset()
    {
        Eigen::VectorXd state(3);
        state(0) = 0.0;
        state(1) = 0.0;
        state(2) = m_radius;

        setState(std::move(state));
        clearPinnedVertices();
    }

    Eigen::SparseMatrix<double> Oscillator::evalJacobian(const Eigen::VectorXd & /*state*/) const
    {
        Eigen::SparseMatrix<double> j(3, 3);

        j.coeffRef(0, 2) = -1.0f;
        j.coeffRef(1, 1) = 1.0f;
        j.coeffRef(2, 0) = 1.0f;

        return j;
    }

    Eigen::VectorXd Oscillator::evalDerivative(const Eigen::VectorXd &state) const
    {
        Eigen::VectorXd d{3};
        d(0) = -state(2);
        d(1) = 0.0f;
        d(2) = state(0);

        return d;
    }

    Corrade::Containers::Array<Vector3> Oscillator::getParticlePositions(const Eigen::VectorXd &state) const
    {
        Corrade::Containers::Array<Vector3> vertices{1};

        vertices[0].x() = static_cast<Float>(state(0));
        vertices[0].y() = static_cast<Float>(state(1));
        vertices[0].z() = static_cast<Float>(state(2));

        return vertices;
    }

} // namespace clothsim