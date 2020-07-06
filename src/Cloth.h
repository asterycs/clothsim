#ifndef CLOTHSIM_CLOTH_H
#define CLOTHSIM_CLOTH_H

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

	struct Spring
	{
		Eigen::Vector3d force(const Eigen::Vector3d pos1, const Eigen::Vector3d pos2) const;
		UnsignedInt leftIdx;
		UnsignedInt rightIdx;
		double k;
		double restLength;
	};

	class Cloth : public System
	{
	public:
		Cloth(PhongIdShader &phongShader,
			  VertexMarkerShader &vertexShader,
			  Object3D &parent,
			  Magnum::SceneGraph::DrawableGroup3D &drawableGroup);
		~Cloth() override;

		Corrade::Containers::Array<Vector3> getParticlePositions(const Eigen::VectorXd &state) const override;

		Eigen::VectorXd evalDerivative(const Eigen::VectorXd &state) const override;
		Eigen::SparseMatrix<double> evalJacobian(const Eigen::VectorXd &state) const override;

		void reset() override;
		void setSize(const Vector2ui size);
		Vector2ui getSize() const;

		Float getMass() const;

	private:
		inline decltype(auto) xFromCoord(const UnsignedInt x, const UnsignedInt y) const
		{
			assert(x < m_size.x() && y < m_size.y());

			return toIdx(x, y);
		}

		template <typename Derived>
		inline decltype(auto) xFromCoord(Eigen::MatrixBase<Derived> &v, const UnsignedInt x, const UnsignedInt y) const
		{
			assert(x < m_size.x() && y < m_size.y());

			return Eigen::VectorBlock<Derived>(v.derived(), toIdx(x, y), 3);
		}

		template <typename Derived>
		inline decltype(auto) xFromCoord(const Eigen::MatrixBase<Derived> &v, const UnsignedInt x, const UnsignedInt y) const
		{
			assert(x < m_size.x() && y < m_size.y());

			return Eigen::VectorBlock<const Derived>(v.derived(), toIdx(x, y), 3);
		}

		inline decltype(auto) dxFromCoord(const UnsignedInt x, const UnsignedInt y) const
		{
			assert(x < m_size.x() && y < m_size.y());

			const auto n{m_size.x() * m_size.y() * 3};

			return n + toIdx(x, y);
		}

		template <typename Derived>
		inline decltype(auto) dxFromCoord(Eigen::MatrixBase<Derived> &v, const UnsignedInt x, const UnsignedInt y) const
		{
			assert(x < m_size.x() && y < m_size.y());

			const auto n{m_size.x() * m_size.y() * 3};

			return Eigen::VectorBlock<Derived>(v.derived(), n + toIdx(x, y), 3);
		}

		template <typename Derived>
		inline decltype(auto) dxFromCoord(const Eigen::MatrixBase<Derived> &v, const UnsignedInt x, const UnsignedInt y) const
		{
			assert(x < m_size.x() && y < m_size.y());

			const auto n{m_size.x() * m_size.y() * 3};

			return Eigen::VectorBlock<const Derived>(v.derived(), n + toIdx(x, y), 3);
		}

		inline decltype(auto) xFromCoord(const UnsignedInt idx) const
		{
			assert(idx < m_size.x() * m_size.y());

			return idx * 3;
		}

		template <typename Derived>
		inline decltype(auto) xFromCoord(Eigen::MatrixBase<Derived> &v, const UnsignedInt idx) const
		{
			assert(idx < m_size.x() * m_size.y());

			return Eigen::VectorBlock<Derived>(v.derived(), idx * 3, 3);
		}

		template <typename Derived>
		inline decltype(auto) xFromCoord(const Eigen::MatrixBase<Derived> &v, const UnsignedInt idx) const
		{
			assert(idx < m_size.x() * m_size.y());

			return Eigen::VectorBlock<const Derived>(v.derived(), idx * 3, 3);
		}

		inline decltype(auto) dxFromCoord(const UnsignedInt idx) const
		{
			assert(idx < m_size.x() * m_size.y());

			const auto n{m_size.x() * m_size.y() * 3};

			return n + idx * 3;
		}

		template <typename Derived>
		inline decltype(auto) dxFromCoord(Eigen::MatrixBase<Derived> &v, const UnsignedInt idx) const
		{
			assert(idx < m_size.x() * m_size.y());

			const auto n{m_size.x() * m_size.y() * 3};

			return Eigen::VectorBlock<Derived>(v.derived(), n + idx * 3, 3);
		}

		template <typename Derived>
		inline decltype(auto) dxFromCoord(const Eigen::MatrixBase<Derived> &v, const UnsignedInt idx) const
		{
			assert(idx < m_size.x() * m_size.y());

			const auto n{m_size.x() * m_size.y() * 3};

			return Eigen::VectorBlock<const Derived>(v.derived(), n + idx * 3, 3);
		}

		inline UnsignedInt toIdx(const UnsignedInt x, const UnsignedInt y) const
		{
			assert(x < m_size.x() && y < m_size.y());

			return (y * m_size.x() + x) * 3;
		}

		inline UnsignedInt toCoord(const UnsignedInt x, const UnsignedInt y) const
		{
			assert(x < m_size.x() && y < m_size.y());

			return y * m_size.x() + x;
		}

		const double m_k{300.0f};
		const double m_dragCoeff{0.08f};

		Vector2ui m_size;
		std::vector<Spring> m_springs;
	};
} // namespace clothsim

#endif