#include "System.h"

#include <Corrade/Containers/Array.h>

#include <cassert>
#include <iostream>

namespace clothsim {
System::System(PhongIdShader &phongShader, VertexMarkerShader &vertexShader,
               Object3D &parent,
               Magnum::SceneGraph::DrawableGroup3D &drawableGroup)
    : Drawable(phongShader, vertexShader, parent, drawableGroup) {}

const System::Vector &System::getState() const { return m_state; }

void System::setState(Vector newState) { m_state = std::move(newState); }

Corrade::Containers::Array<Magnum::Vector3> System::getMeshVertices() const {
  return Corrade::Containers::Array<Magnum::Vector3>{
      getParticlePositions(m_state)};
}

System::ScalarT System::getParticleMass() const { return 0.025f; }

void System::togglePinnedParticle(const UnsignedInt particleId) {
  const auto pos = m_pinnedParticleIds.find(particleId);
  if (pos != m_pinnedParticleIds.end())
    m_pinnedParticleIds.erase(pos);
  else
    m_pinnedParticleIds.insert(particleId);
}

void System::clearPinnedParticles() {
  m_pinnedParticleIds.clear();
}

void System::setPinnedParticle(const UnsignedInt particleId,
                               const bool pinned) {
  const auto pos = m_pinnedParticleIds.find(particleId);
  if (!pinned) {
    if (pos != m_pinnedParticleIds.end())
      m_pinnedParticleIds.erase(pos);
  } else {
    m_pinnedParticleIds.insert(particleId);
  }
}

const std::set<UnsignedInt> &System::getPinnedParticleIds() const {
  return m_pinnedParticleIds;
}

const Corrade::Containers::Array<Magnum::Color3>& System::getVertexMarkerColors() {
  // TODO: Add a nParticles getter
  const auto nVertices{getParticlePositions(getState()).size()};
  m_vertexMarkerColors = Corrade::Containers::Array<Color3>{Corrade::Containers::NoInit,
                                                 nVertices};

  for (std::size_t i{0}; i < nVertices; ++i) {
    if (m_pinnedParticleIds.find(i) == m_pinnedParticleIds.end()) {
      m_vertexMarkerColors[i] = Color3{1.0f, 1.0f, 1.0f};
    } else {
      m_vertexMarkerColors[i] = Color3{1.0f, 0.0f, 0.0f};
    }
  }

  return m_vertexMarkerColors;
}
} // namespace clothsim
