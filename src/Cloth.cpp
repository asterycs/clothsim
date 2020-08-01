#include "Cloth.h"

#include <Corrade/Containers/Array.h>

#include <Magnum/EigenIntegration/Integration.h>

#include <cassert>
#include <iostream>
#include <numeric>

namespace clothsim
{
	System::Vector3 Spring::force(const System::Vector3 pos1, const System::Vector3 pos2) const
	{
		const auto d{pos2 - pos1};
		const System::Vector3 f{-k * (d.norm() - restLength) * d.normalized()};

		return f;
	}

	static inline System::Vector3 fGravity(const System::ScalarT m)
	{
		return System::Vector3{0.0f, 0.0f, -9.81f * m};
	}

	static inline System::Vector3 fDrag(const System::Vector3 v, System::ScalarT k)
	{
		return System::Vector3{-k * v};
	}

	Cloth::Cloth(PhongIdShader &phongShader,
				 VertexMarkerShader &vertexShader,
				 Object3D &parent,
				 Magnum::SceneGraph::DrawableGroup3D &drawableGroup) : System(phongShader,
																			  vertexShader,
																			  parent,
																			  drawableGroup),
																	   m_size{{2, 2}}
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

		constexpr ScalarT width{1.5f};
		constexpr ScalarT height{1.5f};
		const Vector3 yStep{0.0, -height / m_size.y(), 0.0};
		const Vector3 xStep{width / m_size.x(), 0.0, 0.0};
		const ScalarT restLengthX{xStep.norm()};
		const ScalarT restLengthY{yStep.norm()};
		const ScalarT restLengthD{std::sqrt(xStep.squaredNorm() + yStep.squaredNorm())};
		const ScalarT restLength2X{2.0f * xStep.norm()};
		const ScalarT restLength2Y{2.0f * yStep.norm()};
		const Vector3 offset{-width * 0.5f, 0.0f, 1.0f};

		Vector state{2 * m_size.x() * m_size.y() * 3};
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

		const auto xSquares{m_size.x() - 1};
		const auto ySquares{m_size.y() - 1};
		auto triangleIndices{Corrade::Containers::Array<UnsignedInt>(2 * xSquares * ySquares * 3)};

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
		clearPinnedParticles();

		setPinnedParticle(0, true);
		setPinnedParticle(m_size.x() - 1, true);
	}

	System::SparseMatrix Cloth::evalJacobian(const Vector &state) const
	{
		const auto n{m_size.x() * m_size.y()};

		SparseMatrixRM j(n * 3 * 2, n * 3 * 2);

		using T = Eigen::Triplet<ScalarT>;
		std::vector<T> triplets;
		triplets.reserve(n * 3 * 2 + 6 * m_size.x() * m_size.y());

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
			const Vector3 xl{state.segment(li, 3)};
			const Vector3 xr{state.segment(ri, 3)};

			const Vector3 dx{xl - xr};
			const Vector3 dxn{dx.normalized()};
			const auto I{Matrix3::Identity(3, 3)};
			const auto dxdxt{dxn * dxn.transpose()};

			const Matrix3 jPart{-s.k * ((1.0f - s.restLength / dx.norm()) * (I - dxdxt) + dxdxt) / getParticleMass()};

			for (UnsignedInt yi = 0; yi < 3; ++yi)
			{
				for (UnsignedInt xi = 0; xi < 3; ++xi)
				{
					triplets.push_back(T(li + yi + 3 * n, li + xi, jPart(yi, xi)));
					triplets.push_back(T(li + yi + 3 * n, ri + xi, -jPart(yi, xi)));
					triplets.push_back(T(ri + yi + 3 * n, li + xi, -jPart(yi, xi)));
					triplets.push_back(T(ri + yi + 3 * n, ri + xi, jPart(yi, xi)));
				}
			}
		}

		j.setFromTriplets(triplets.begin(), triplets.end());

		const auto &pinnedParticles{getPinnedParticleIds()};
		for (const auto pinnedParticle : pinnedParticles)
		{
			j.row(pinnedParticle * 3 + n * 3) = SparseVector(n * 3 * 2);
		}

		return j;
	}

	System::Vector Cloth::evalDerivative(const Vector &state) const
	{
		const auto n{m_size.x() * m_size.y()};
		Vector dxdt{Vector::Zero(n * 3 * 2)};
		const ScalarT massInv{1.0f / getParticleMass()};

		for (UnsignedInt i = 0; i < n; ++i)
		{
			xFromCoord(dxdt, i) = dxFromCoord(state, i);
		}

		for (const auto spring : m_springs)
		{
			const Vector3 fS{spring.force(xFromCoord(state, spring.leftIdx), xFromCoord(state, spring.rightIdx))};
			dxFromCoord(dxdt, spring.rightIdx) += fS * massInv;
			dxFromCoord(dxdt, spring.leftIdx) -= fS * massInv;
		}

		for (UnsignedInt i = 0; i < n; ++i)
		{
			const Vector3 v{dxFromCoord(state, i)};
			dxFromCoord(dxdt, i) += (fDrag(v, m_dragCoeff) + fGravity(getParticleMass())) * massInv;
		}

		const auto &pinnedParticles{getPinnedParticleIds()};
		for (const auto pinnedIdx : pinnedParticles)
		{
			xFromCoord(dxdt, pinnedIdx) = Vector3::Zero();
			dxFromCoord(dxdt, pinnedIdx) = Vector3::Zero();
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

	Corrade::Containers::Array<Magnum::Vector3> Cloth::getParticlePositions(const Vector &state) const
	{
		const auto n{m_size.x() * m_size.y()};
		Corrade::Containers::Array<Magnum::Vector3> positions{n};

		for (auto i = 0u; i < n; ++i)
		{
			const auto si{i * 3};

			positions[i].x() = static_cast<Float>(state(si));
			positions[i].y() = static_cast<Float>(state(si + 1));
			positions[i].z() = static_cast<Float>(state(si + 2));
		}

		return positions;
	}

} // namespace clothsim