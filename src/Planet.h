#ifndef CLOTHSIM_PLANET_H
#define CLOTHSIM_PLANET_H

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

    class Planet : public System
    {
    public:
        Planet(PhongIdShader &phongShader,
               VertexMarkerShader &vertexShader,
               Object3D &parent,
               Magnum::SceneGraph::DrawableGroup3D &drawableGroup);
        ~Planet() override;

        Corrade::Containers::Array<Vector3> getParticlePositions(const Eigen::VectorXd &state) const override;

        Eigen::VectorXd evalDerivative(const Eigen::VectorXd &state) const override;
        Eigen::SparseMatrix<double> evalJacobian(const Eigen::VectorXd &state) const override;

        void reset() override;

    private:
        Float m_radius{1.0f};
    };
} // namespace clothsim

#endif