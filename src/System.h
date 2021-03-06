#ifndef CLOTHSIM_SYSTEM_H
#define CLOTHSIM_SYSTEM_H

#include <Corrade/Containers/Array.h>

#include <Magnum/Magnum.h>

#include <Eigen/Sparse>

#include "Drawable.h"

#include <set>
#include <vector>

namespace clothsim {
class PhongShader;
class VertexShader;
using namespace Magnum;

class System : public Drawable {
public:
  using ScalarT = float;
  using Vector3 = Eigen::Matrix<ScalarT, 3, 1>;
  using Vector = Eigen::Matrix<ScalarT, Eigen::Dynamic, 1>;
  using Matrix3 = Eigen::Matrix<ScalarT, 3, 3>;
  using Matrix = Eigen::Matrix<ScalarT, Eigen::Dynamic, Eigen::Dynamic>;
  using SparseVector = Eigen::SparseVector<ScalarT>;
  using SparseMatrix = Eigen::SparseMatrix<ScalarT>;
  using SparseMatrixRM = Eigen::SparseMatrix<ScalarT, Eigen::RowMajor>;

  System(PhongIdShader &phongShader, VertexMarkerShader &vertexShader,
         Object3D &parent, Magnum::SceneGraph::DrawableGroup3D &drawableGroup);

  virtual Vector evalDerivative(const Vector &state) const = 0;
  virtual SparseMatrix evalJacobian(const Vector &state) const = 0;

  virtual void reset() = 0;

  virtual Corrade::Containers::Array<Magnum::Vector3>
  getParticlePositions(const Vector &state) const = 0;
  Corrade::Containers::Array<Magnum::Vector3> getMeshVertices() const override;
  const Corrade::Containers::Array<Magnum::Color3>& getVertexMarkerColors() override;

  virtual ScalarT getParticleMass() const;

  const Vector &getState() const;
  void setState(Vector newState);

  void togglePinnedParticle(const UnsignedInt particleId);
  void setPinnedParticle(const UnsignedInt particleId, const bool pinned);
  const std::set<UnsignedInt> &getPinnedParticleIds() const;
  void clearPinnedParticles();

private:
  Vector m_state{};
  std::set<UnsignedInt> m_pinnedParticleIds{};
  Corrade::Containers::Array<Magnum::Color3> m_vertexMarkerColors;
};
} // namespace clothsim

#endif
