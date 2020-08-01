#ifndef CLOTHSIM_OSCILLATOR_H
#define CLOTHSIM_OSCILLATOR_H

#include <Corrade/Containers/Array.h>

#include <Magnum/Magnum.h>

#include <Eigen/Sparse>

#include "System.h"

#include <vector>

namespace clothsim
{
    class PhongShader;
    class VertexShader;
    using namespace Magnum;

    class Oscillator : public System
    {
    public:
        Oscillator(PhongIdShader &phongShader,
                   VertexMarkerShader &vertexShader,
                   Object3D &parent,
                   Magnum::SceneGraph::DrawableGroup3D &drawableGroup);
        ~Oscillator() override;

        Corrade::Containers::Array<Magnum::Vector3> getParticlePositions(const Vector &state) const override;

        Vector evalDerivative(const Vector &state) const override;
        SparseMatrix evalJacobian(const Vector &state) const override;

        void reset() override;

    private:
        ScalarT m_radius{1.0};
    };
} // namespace clothsim

#endif