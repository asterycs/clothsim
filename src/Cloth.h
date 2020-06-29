#ifndef CLOTHSIM_CLOTH_H
#define CLOTHSIM_CLOTH_H

#include <Corrade/Containers/Array.h>

#include <Magnum/Magnum.h>

#include "System.h"

#include <vector>

namespace clothsim
{
	class PhongShader;
	class VertexShader;
	using namespace Magnum;

	struct Spring
	{
		UnsignedInt leftIdx;
		UnsignedInt rightIdx;
		Float k;
		Float restLength;
	};

	class Cloth : public System
	{
	public:
		Cloth(PhongIdShader &phongShader,
			  VertexShader &vertexShader,
			  Object3D &parent,
			  Magnum::SceneGraph::DrawableGroup3D &drawableGroup);
		~Cloth();

		Corrade::Containers::Array<Vector3> evalDerivative(const Corrade::Containers::Array<Vector3> &state) const override;
		void reset();

	private:
		UnsignedInt firstByCoord(UnsignedInt x, UnsignedInt y) const;
		UnsignedInt secondByCoord(UnsignedInt x, UnsignedInt y) const;
		UnsignedInt firstByIdx(UnsignedInt idx) const;
		UnsignedInt secondByIdx(UnsignedInt idx) const;

		Vector2ui m_size;
		std::vector<Spring> m_springs;
	};

	void eulerStep(Cloth &cloth, const Float dt);
	void rk4Step(Cloth &ps, const Float dt);

} // namespace clothsim

#endif