#ifndef CLOTHSIM_SYSTEM_H
#define CLOTHSIM_SYSTEM_H

#include <Corrade/Containers/Array.h>

#include <Magnum/Magnum.h>

#include "Drawable.h"

#include <vector>
#include <set>

namespace clothsim
{
	class PhongShader;
	class VertexShader;
	using namespace Magnum;

	class System : public Drawable
	{
	public:
		System(PhongIdShader &phongShader,
			   VertexShader &vertexShader,
			   Object3D &parent,
			   Magnum::SceneGraph::DrawableGroup3D &drawableGroup);
		~System();

		virtual Corrade::Containers::Array<Vector3> evalDerivative(const Corrade::Containers::Array<Vector3> &state) const = 0;

		const Corrade::Containers::Array<Vector3> &getState() const;
		void setState(const Corrade::Containers::Array<Vector3> newState);

		void togglePinnedVertex(const UnsignedInt vertexId);
		void setPinnedVertex(const UnsignedInt vertexId, const bool pinned);
		const std::set<UnsignedInt> &getPinnedVertexIds() const;
		void clearPinnedVertices();

	private:
		void updateVertexMarkerColors();

		Corrade::Containers::Array<Vector3> m_state{};
		std::set<UnsignedInt> m_pinnedVertexIds{};
	};
} // namespace clothsim

#endif