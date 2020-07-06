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

	const Eigen::VectorXd &System::getState() const
	{
		return m_state;
	}

	void System::setTriangleIndices(Corrade::Containers::Array<UnsignedInt> triangleIndices)
	{
		m_triangleIndices = std::move(triangleIndices);
	}

	void System::setState(Eigen::VectorXd newState)
	{
		m_state = std::move(newState);

		Corrade::Containers::Array<Vector3> vertices{getParticlePositions(m_state)};
		Corrade::Containers::Array<UnsignedInt> indices(m_triangleIndices.size());

		// TODO: memcpy instead
		for (UnsignedInt i = 0; i < m_triangleIndices.size(); ++i)
		{
			indices[i] = m_triangleIndices[i];
		}

		setVertexData(std::move(vertices), std::move(indices));
	}

	double System::getParticleMass() const
	{
		return 1.0;
	}

	void System::togglePinnedVertex(const UnsignedInt vertexId)
	{
		const auto pos = m_pinnedVertexIds.find(vertexId);
		if (pos != m_pinnedVertexIds.end())
			m_pinnedVertexIds.erase(pos);
		else
			m_pinnedVertexIds.insert(vertexId);

		updateVertexMarkerColors();
	}

	void System::clearPinnedVertices()
	{
		m_pinnedVertexIds.clear();

		updateVertexMarkerColors();
	}

	void System::setPinnedVertex(const UnsignedInt vertexId, const bool pinned)
	{
		const auto pos = m_pinnedVertexIds.find(vertexId);
		if (!pinned)
		{
			if (pos != m_pinnedVertexIds.end())
				m_pinnedVertexIds.erase(pos);
		}
		else
		{
			m_pinnedVertexIds.insert(vertexId);
		}

		updateVertexMarkerColors();
	}

	const std::set<UnsignedInt> &System::getPinnedVertexIds() const
	{
		return m_pinnedVertexIds;
	}

	void System::updateVertexMarkerColors()
	{
		const auto nVertices{getParticlePositions(getState()).size()};
		auto colors = Corrade::Containers::Array<Color3>(Corrade::Containers::NoInit, nVertices);

		for (std::size_t i = 0; i < nVertices; ++i)
		{
			if (m_pinnedVertexIds.find(i) == m_pinnedVertexIds.end())
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