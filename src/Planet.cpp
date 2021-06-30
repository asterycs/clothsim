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
        Vector state{6};
        state.setZero();

        state(2) = m_radius;
        state(3) = -1.0f / Math::sqrt(m_radius);

        setState(std::move(state));
        clearPinnedParticles();
    }

    System::SparseMatrix Planet::evalJacobian(const Vector &state) const
    {
        SparseMatrix j{6, 6};
        const Vector3 X{state.head<3>()};
        const Vector3 Xn{X.normalized()};
        const auto r2{X.squaredNorm()};

        const auto cross{Xn * Xn.transpose()};
        const auto I{Matrix3::Identity()};

        const Matrix3 jPart{2.0f / (r2 * r2) * cross - I / r2};

        j.reserve(12);

        j.insert(0, 3) = 1.0f;
        j.insert(1, 4) = 1.0f;
        j.insert(2, 5) = 1.0f;

        j.insert(3, 0) = jPart(0, 0);
        j.insert(4, 0) = jPart(1, 0);
        j.insert(5, 0) = jPart(2, 0);

        j.insert(3, 1) = jPart(0, 1);
        j.insert(4, 1) = jPart(1, 1);
        j.insert(5, 1) = jPart(2, 1);

        j.insert(3, 2) = jPart(0, 2);
        j.insert(4, 2) = jPart(1, 2);
        j.insert(5, 2) = jPart(2, 2);

        return j;
    }

    System::Vector Planet::evalDerivative(const Vector &state) const
    {
        Vector d{Vector::Zero(6)};
        d.segment(0, 3) = state.segment(3, 3);

        const Vector3 pos{state.head<3>()};
        const double r2{pos.squaredNorm()};

        if (r2 > 0)
            d.segment(3, 3) = -state.segment(0, 3) / r2;

        for (const auto pinnedIdx : getPinnedParticleIds())
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

    Corrade::Containers::Array<Magnum::Vector3> Planet::getParticlePositions(const Vector &state) const
    {
        Corrade::Containers::Array<Magnum::Vector3> vertices{1};

        vertices[0].x() = static_cast<Float>(state(0));
        vertices[0].y() = static_cast<Float>(state(1));
        vertices[0].z() = static_cast<Float>(state(2));

        return vertices;
    }

    Corrade::Containers::Array<UnsignedInt> Planet::getMeshIndices() const {
        return Corrade::Containers::Array<UnsignedInt>();
    }

} // namespace clothsim
