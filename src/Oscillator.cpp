#include "Oscillator.h"

#include <Corrade/Containers/Array.h>

#include <Magnum/EigenIntegration/Integration.h>

#include <cassert>
#include <iostream>

namespace clothsim {
Oscillator::Oscillator(PhongIdShader &phongShader,
                       VertexMarkerShader &vertexShader, Object3D &parent,
                       Magnum::SceneGraph::DrawableGroup3D &drawableGroup)
    : System(phongShader, vertexShader, parent, drawableGroup) {
  reset();
}

Oscillator::~Oscillator() {}

void Oscillator::reset() {
  Vector state(3);
  state(0) = 0.0;
  state(1) = 0.0;
  state(2) = m_radius;

  setState(std::move(state));
  clearPinnedParticles();
}

System::SparseMatrix Oscillator::evalJacobian(const Vector & /*state*/) const {
  SparseMatrix j{3, 3};

  j.insert(0, 2) = -1.0;
  j.insert(1, 1) = 1.0;
  j.insert(2, 0) = 1.0;

  return j;
}

System::Vector Oscillator::evalDerivative(const Vector &state) const {
  Vector d{3};
  d(0) = -state(2);
  d(1) = 0.0;
  d(2) = state(0);

  return d;
}

Corrade::Containers::Array<Magnum::Vector3>
Oscillator::getParticlePositions(const Vector &state) const {
  Corrade::Containers::Array<Magnum::Vector3> vertices{1};

  vertices[0].x() = static_cast<Float>(state(0));
  vertices[0].y() = static_cast<Float>(state(1));
  vertices[0].z() = static_cast<Float>(state(2));

  return vertices;
}

Corrade::Containers::Array<UnsignedInt> Oscillator::getMeshIndices() const {
  return Corrade::Containers::Array<UnsignedInt>{};
}

} // namespace clothsim
