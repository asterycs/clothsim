#ifndef CLOTHSIM_SYSTEM_H
#define CLOTHSIM_SYSTEM_H

#include <Corrade/Containers/Array.h>

#include <Magnum/Magnum.h>

#include <Eigen/Sparse>

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
			   VertexMarkerShader &vertexShader,
			   Object3D &parent,
			   Magnum::SceneGraph::DrawableGroup3D &drawableGroup);
		~System();

		virtual Eigen::VectorXd evalDerivative(const Eigen::VectorXd &state) const = 0;
		virtual Eigen::SparseMatrix<double> evalJacobian(const Eigen::VectorXd &state) const = 0;

		virtual void reset() = 0;
		virtual double getParticleMass() const;

		const Eigen::VectorXd &getState() const;
		void setState(Eigen::VectorXd newState);

		virtual Corrade::Containers::Array<Vector3> getParticlePositions(const Eigen::VectorXd &state) const = 0;

		void setTriangleIndices(Corrade::Containers::Array<UnsignedInt> triangleIndices);

		void togglePinnedVertex(const UnsignedInt vertexId);
		void setPinnedVertex(const UnsignedInt vertexId, const bool pinned);
		const std::set<UnsignedInt> &getPinnedVertexIds() const;
		void clearPinnedVertices();

	private:
		void updateVertexMarkerColors();

		Eigen::VectorXd m_state{};
		Corrade::Containers::Array<UnsignedInt> m_triangleIndices{};
		std::set<UnsignedInt> m_pinnedVertexIds{};
	};
} // namespace clothsim

#endif