#include "Planet.h"

#include <Corrade/Containers/Array.h>

#include <Magnum/EigenIntegration/Integration.h>

#include <cassert>
#include <cmath>
#include <iostream>

namespace clothsim
{
    Planet::Planet(PhongIdShader &phongShader,
                   VertexMarkerShader &vertexShader,
                   Object3D &parent,
                   Magnum::SceneGraph::DrawableGroup3D &drawableGroup) : System(phongShader,
                                                                                vertexShader,
                                                                                parent,
                                                                                drawableGroup)
    {
        reset();
    }

    Planet::~Planet()
    {
    }

    void Planet::reset()
    {
        Eigen::VectorXd state{6};
        state.setZero();

        state(2) = m_radius;
        state(3) = -1.0f / Math::sqrt(m_radius);

        setState(std::move(state));
        clearPinnedVertices();
    }

    Eigen::SparseMatrix<double> Planet::evalJacobian(const Eigen::VectorXd &state) const
    {
        Eigen::SparseMatrix<double> j(6, 6);
        const Eigen::Vector3d X{state.head<3>()};
        const Eigen::Vector3d Xn{X.normalized()};
        const auto r2{X.squaredNorm()};
        const auto m{getParticleMass()};

        const Eigen::Matrix3d cross{Xn * Xn.transpose()};
        const Eigen::Matrix3d I{Eigen::Matrix3d::Identity()};

        const Eigen::Matrix3d jPart{2 * m / (r2 * r2) * cross - I * m / r2};

        j.coeffRef(0, 3) = 1.0f;
        j.coeffRef(1, 4) = 1.0f;
        j.coeffRef(2, 5) = 1.0f;

        j.coeffRef(3, 0) = jPart(0, 0);
        j.coeffRef(4, 0) = jPart(1, 0);
        j.coeffRef(5, 0) = jPart(2, 0);

        j.coeffRef(3, 1) = jPart(0, 1);
        j.coeffRef(4, 1) = jPart(1, 1);
        j.coeffRef(5, 1) = jPart(2, 1);

        j.coeffRef(3, 2) = jPart(0, 2);
        j.coeffRef(4, 2) = jPart(1, 2);
        j.coeffRef(5, 2) = jPart(2, 2);

        return j;
    }

    Eigen::VectorXd Planet::evalDerivative(const Eigen::VectorXd &state) const
    {
        Eigen::VectorXd d(6);
        d.segment(0, 3) = state.segment(3, 3);

        const auto pos = state.head(3);
        const double r = pos.norm();

        if (r > 0)
            d.segment(3, 3) = -state.segment(0, 3) / (r * r);

        for (const auto pinnedIdx : getPinnedVertexIds())
        {
            d(pinnedIdx * 3) = 0.0f;
            d(pinnedIdx * 3 + 1) = 0.0f;
            d(pinnedIdx * 3 + 2) = 0.0f;

            d(pinnedIdx * 3 + 3) = 0.0f;
            d(pinnedIdx * 3 + 3 + 1) = 0.0f;
            d(pinnedIdx * 3 + 3 + 2) = 0.0f;
        }

        return d;
    }

    Corrade::Containers::Array<Vector3> Planet::getParticlePositions(const Eigen::VectorXd &state) const
    {
        Corrade::Containers::Array<Vector3> vertices{1};

        vertices[0].x() = static_cast<Float>(state(0));
        vertices[0].y() = static_cast<Float>(state(1));
        vertices[0].z() = static_cast<Float>(state(2));

        return vertices;
    }

} // namespace clothsim