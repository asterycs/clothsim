#include "System.h"

#include <Corrade/Containers/Array.h>

#include <cassert>
#include <iostream>

namespace clothsim
{
	System::System(PhongIdShader &phongShader,
				   VertexMarkerShader &vertexShader,
				   Object3D &parent,
				   Magnum::SceneGraph::DrawableGroup3D &drawableGroup) : Drawable(phongShader,
																				  vertexShader,
																				  parent,
																				  drawableGroup)
	{
	}

	System::~System()
	{
	}

	const System::Vector &System::getState() const
	{
		return m_state;
	}

	void System::setState(Vector newState)
	{
		m_state = std::move(newState);
	}

	Corrade::Containers::Array<Magnum::Vector3> System::getMeshVertices() const
	{
		return Corrade::Containers::Array<Magnum::Vector3>{getParticlePositions(m_state)};
	}


	System::ScalarT System::getParticleMass() const
	{
		return 0.025f;
	}

	void System::togglePinnedParticle(const UnsignedInt particleId)
	{
		const auto pos = m_pinnedParticleIds.find(particleId);
		if (pos != m_pinnedParticleIds.end())
			m_pinnedParticleIds.erase(pos);
		else
			m_pinnedParticleIds.insert(particleId);

		updateParticleMarkerColors();
	}

	void System::clearPinnedParticles()
	{
		m_pinnedParticleIds.clear();

		updateParticleMarkerColors();
	}

	void System::setPinnedParticle(const UnsignedInt particleId, const bool pinned)
	{
		const auto pos = m_pinnedParticleIds.find(particleId);
		if (!pinned)
		{
			if (pos != m_pinnedParticleIds.end())
				m_pinnedParticleIds.erase(pos);
		}
		else
		{
			m_pinnedParticleIds.insert(particleId);
		}

		updateParticleMarkerColors();
	}

	const std::set<UnsignedInt> &System::getPinnedParticleIds() const
	{
		return m_pinnedParticleIds;
	}

	void System::updateParticleMarkerColors()
	{
		const auto nVertices{getParticlePositions(getState()).size()};
		auto colors{Corrade::Containers::Array<Color3>(Corrade::Containers::NoInit, nVertices)};

		for (std::size_t i = 0; i < nVertices; ++i)
		{
			if (m_pinnedParticleIds.find(i) == m_pinnedParticleIds.end())
			{
				colors[i] = Color3{1.0f, 1.0f, 1.0f};
			}
			else
			{
				colors[i] = Color3{1.0f, 0.0f, 0.0f};
			}
		}

		setVertexMarkerColors(std::move(colors));
	}
} // namespace clothsim
