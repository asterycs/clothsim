#include "Cloth.h"

#include <Corrade/Containers/Array.h>

#include <Magnum/EigenIntegration/Integration.h>

#include <cassert>
#include <iostream>
#include <numeric>

namespace clothsim
{
	Eigen::Vector3d Spring::force(const Eigen::Vector3d pos1, const Eigen::Vector3d pos2) const
	{
		const auto d{pos2 - pos1};
		const Eigen::Vector3d f{-k * (d.norm() - restLength) * d.normalized()};

		return f;
	}

	static inline Eigen::Vector3d fGravity(const double m)
	{
		return Eigen::Vector3d{0.0, 0.0, -9.81 * m};
	}

	static inline Eigen::Vector3d fDrag(const Eigen::Vector3d v, double k)
	{
		return Eigen::Vector3d{-k * v};
	}

	Cloth::Cloth(PhongIdShader &phongShader,
				 VertexMarkerShader &vertexShader,
				 Object3D &parent,
				 Magnum::SceneGraph::DrawableGroup3D &drawableGroup) : System(phongShader,
																			  vertexShader,
																			  parent,
																			  drawableGroup),
																	   m_size{{4, 4}}
	{
		reset();
	}

	Cloth::~Cloth()
	{
	}

	void Cloth::reset()
	{
		if (m_size.x() == 0 || m_size.y() == 0)
			throw std::runtime_error("Invalid cloth size");

		constexpr double width{1.5};
		constexpr double height{1.5};
		const Eigen::Vector3d yStep{0.0, -height / m_size.x(), 0.0};
		const Eigen::Vector3d xStep{width / m_size.y(), 0.0, 0.0};
		const double restLengthX{xStep.norm()};
		const double restLengthY{yStep.norm()};
		const double restLengthD{std::sqrt(xStep.squaredNorm() + yStep.squaredNorm())};
		const double restLength2X{2.0 * xStep.norm()};
		const double restLength2Y{2.0 * yStep.norm()};
		const Eigen::Vector3d offset{-width * 0.5, 0.0, 1.0};

		Eigen::VectorXd state{2 * m_size.x() * m_size.y() * 3};
		state.setZero();

		m_springs.clear();

		for (UnsignedInt y = 0; y < m_size.y(); ++y)
		{
			for (UnsignedInt x = 0; x < m_size.x() - 1; ++x)
			{
				const Spring s{toCoord(x, y), toCoord(x + 1, y), m_k, restLengthX};
				m_springs.push_back(s);
			}
		}

		for (UnsignedInt x = 0; x < m_size.x(); ++x)
		{
			for (UnsignedInt y = 0; y < m_size.y() - 1; ++y)
			{
				const Spring s{toCoord(x, y), toCoord(x, y + 1), m_k, restLengthY};
				m_springs.push_back(s);
			}
		}

		for (UnsignedInt y = 0; y < m_size.y() - 1; ++y)
		{
			for (UnsignedInt x = 0; x < m_size.x() - 1; ++x)
			{
				const Spring s{toCoord(x, y), toCoord(x + 1, y + 1), m_k, restLengthD};
				m_springs.push_back(s);
			}
		}

		for (UnsignedInt y = 1; y < m_size.y(); ++y)
		{
			for (UnsignedInt x = 0; x < m_size.x() - 1; ++x)
			{
				const Spring s{toCoord(x, y), toCoord(x + 1, y - 1), m_k, restLengthD};
				m_springs.push_back(s);
			}
		}

		for (UnsignedInt y = 0; y < m_size.y(); ++y)
		{
			for (UnsignedInt x = 0; x < m_size.x(); ++x)
			{
				if (m_size.y() > 2 && y < m_size.y() - 2)
				{
					const Spring s1{toCoord(x, y), toCoord(x, y + 2), m_k, restLength2Y};
					m_springs.push_back(s1);
				}

				if (m_size.x() > 2 && x < m_size.x() - 2)
				{
					const Spring s2{toCoord(x, y), toCoord(x + 2, y), m_k, restLength2X};
					m_springs.push_back(s2);
				}
			}
		}

		for (UnsignedInt y = 0; y < m_size.y(); ++y)
		{
			for (UnsignedInt x = 0; x < m_size.x(); ++x)
			{
				xFromCoord(state, x, y) = y * yStep + x * xStep + offset;
			}
		}

		const auto xSquares = m_size.x() - 1;
		const auto ySquares = m_size.y() - 1;
		auto triangleIndices = Corrade::Containers::Array<UnsignedInt>(2 * xSquares * ySquares * 3);

		for (UnsignedInt triRow = 0; triRow < ySquares; ++triRow)
		{
			for (UnsignedInt triCol = 0; triCol < xSquares; ++triCol)
			{
				triangleIndices[triRow * xSquares * 2 * 3 + triCol * 2 * 3] = triRow * m_size.x() + triCol;
				triangleIndices[triRow * xSquares * 2 * 3 + triCol * 2 * 3 + 1] = triRow * m_size.x() + triCol + 1;
				triangleIndices[triRow * xSquares * 2 * 3 + triCol * 2 * 3 + 2] = triRow * m_size.x() + triCol + m_size.x();

				triangleIndices[triRow * xSquares * 2 * 3 + triCol * 2 * 3 + 3] = triRow * m_size.x() + triCol + 1;
				triangleIndices[triRow * xSquares * 2 * 3 + triCol * 2 * 3 + 4] = triRow * m_size.x() + triCol + 1 + m_size.x();
				triangleIndices[triRow * xSquares * 2 * 3 + triCol * 2 * 3 + 5] = triRow * m_size.x() + triCol + m_size.x();
			}
		}

		setTriangleIndices(std::move(triangleIndices));
		setState(std::move(state));
		clearPinnedVertices();

		setPinnedVertex(0, true);
		setPinnedVertex(m_size.x() - 1, true);
	}

	Eigen::SparseMatrix<double> Cloth::evalJacobian(const Eigen::VectorXd &state) const
	{
		const std::size_t n{m_size.x() * m_size.y()};

		Eigen::SparseMatrix<double> j(n * 3 * 2, n * 3 * 2);

		std::vector<Eigen::Triplet<double>> triplets;
		triplets.reserve(n * 3 * 2 + 6 * m_size.x() * m_size.y());

		using T = Eigen::Triplet<double>;

		for (auto i = 0u; i < n; ++i)
		{
			triplets.push_back(T(3 * i, 3 * i + n * 3, 1.0));
			triplets.push_back(T(3 * i + 1, 3 * i + n * 3 + 1, 1.0));
			triplets.push_back(T(3 * i + 2, 3 * i + n * 3 + 2, 1.0));
		}

		for (auto i = 0u; i < n; ++i)
		{
			triplets.push_back(T(3 * i + n * 3, 3 * i + 3 * n, m_dragCoeff));
			triplets.push_back(T(3 * i + n * 3 + 1, 3 * i + 3 * n + 1, m_dragCoeff));
			triplets.push_back(T(3 * i + n * 3 + 2, 3 * i + 3 * n + 2, m_dragCoeff));
		}

		for (const auto s : m_springs)
		{
			const auto li{xFromCoord(s.leftIdx)};
			const auto ri{xFromCoord(s.rightIdx)};
			const Eigen::Vector3d xl{state.segment(li, 3)};
			const Eigen::Vector3d xr{state.segment(ri, 3)};

			const Eigen::Vector3d dx{xl - xr};
			const Eigen::Vector3d dxn{dx.normalized()};
			const Eigen::Matrix3d I{Eigen::Matrix3d::Identity(3, 3)};
			const Eigen::Matrix3d dxdxt{dxn * dxn.transpose()};

			const Eigen::Matrix3d jPart{-s.k * ((1.0 - s.restLength / dx.norm()) * (I - dxdxt) + dxdxt) / getParticleMass()};

			for (UnsignedInt yi = 0; yi < 3; ++yi)
			{
				for (UnsignedInt xi = 0; xi < 3; ++xi)
				{
					if (!getPinnedVertexIds().contains(li / 3))
						triplets.push_back(T(li + yi + 3 * n, li + xi, jPart.coeffRef(yi, xi)));

					if (!getPinnedVertexIds().contains(li / 3) && !getPinnedVertexIds().contains(ri / 3))
						triplets.push_back(T(li + yi + 3 * n, ri + xi, -jPart.coeffRef(yi, xi)));

					if (!getPinnedVertexIds().contains(ri / 3) && !getPinnedVertexIds().contains(li / 3))
						triplets.push_back(T(ri + yi + 3 * n, li + xi, -jPart.coeffRef(yi, xi)));

					if (!getPinnedVertexIds().contains(ri / 3))
						triplets.push_back(T(ri + yi + 3 * n, ri + xi, jPart.coeffRef(yi, xi)));
				}
			}
		}

		j.setFromTriplets(triplets.begin(), triplets.end());

		return j;
	}

	Eigen::VectorXd Cloth::evalDerivative(const Eigen::VectorXd &state) const
	{
		const auto n{m_size.x() * m_size.y()};
		Eigen::VectorXd dxdt{Eigen::VectorXd::Zero(n * 3 * 2)};
		const double massInv{1.0 / getParticleMass()};

		for (UnsignedInt i = 0; i < n; ++i)
		{
			xFromCoord(dxdt, i) = dxFromCoord(state, i);
		}

		for (const auto spring : m_springs)
		{
			const Eigen::Vector3d fS{spring.force(xFromCoord(state, spring.leftIdx), xFromCoord(state, spring.rightIdx))};
			dxFromCoord(dxdt, spring.rightIdx) += fS * massInv;
			dxFromCoord(dxdt, spring.leftIdx) -= fS * massInv;
		}

		for (UnsignedInt i = 0; i < n; ++i)
		{
			const Eigen::Vector3d v{dxFromCoord(state, i)};
			dxFromCoord(dxdt, i) += (fDrag(v, m_dragCoeff) + fGravity(getParticleMass())) * massInv;
		}

		for (const auto pinnedIdx : getPinnedVertexIds())
		{
			xFromCoord(dxdt, pinnedIdx) = Eigen::Vector3d::Zero();
			dxFromCoord(dxdt, pinnedIdx) = Eigen::Vector3d::Zero();
		}

		return dxdt;
	}

	Vector2ui Cloth::getSize() const
	{
		return m_size;
	}

	void Cloth::setSize(const Vector2ui size)
	{
		m_size = size;

		reset();
	}

	Corrade::Containers::Array<Vector3> Cloth::getParticlePositions(const Eigen::VectorXd &state) const
	{
		const auto n{m_size.x() * m_size.y()};
		Corrade::Containers::Array<Vector3> vertices{n};

		for (auto i = 0u; i < n; ++i)
		{
			const auto si{i * 3};

			vertices[i].x() = static_cast<Float>(state(si));
			vertices[i].y() = static_cast<Float>(state(si + 1));
			vertices[i].z() = static_cast<Float>(state(si + 2));
		}

		return vertices;
	}

} // namespace clothsim