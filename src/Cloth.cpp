#include "Cloth.h"

#include <Corrade/Containers/Array.h>

#include <cassert>

namespace clothsim
{
	static Vector3 fGravity(Float m)
	{
		return Vector3{0.0f, 0.0f, -9.81f * m};
	}

	static Vector3 fSpring(const Vector3 &pos1, const Vector3 &pos2, Float k, Float restLength)
	{
		const Vector3 d{pos2 - pos1};
		const Vector3 f{-k * (d.length() - restLength) * d.normalized()};

		return f;
	}

	static Vector3 fDrag(const Vector3 &v, Float k)
	{
		return Vector3{-k * v};
	}

	Cloth::Cloth(PhongIdShader &phongShader,
				 VertexMarkerShader &vertexShader,
				 Object3D &parent,
				 Magnum::SceneGraph::DrawableGroup3D &drawableGroup) : System(phongShader,
																			  vertexShader,
																			  parent,
																			  drawableGroup),
																	   m_size{{40, 40}}
	{
		reset();
	}

	Cloth::~Cloth()
	{
	}

	void Cloth::reset()
	{
		constexpr Float k{300.0f};
		constexpr Float width{1.5f};
		constexpr Float height{1.5f};
		const Vector3 yStep{0.0f, height / m_size.x(), 0.0f};
		const Vector3 xStep{width / m_size.y(), 0.0f, 0.0f};
		const Float restLength1 = xStep.length();
		const Float restLength2 = Math::sqrt(xStep.dot() + yStep.dot());
		const Float restLength3 = 2.0f * xStep.length();
		const Vector3 offset{-width * 0.5f, 0.0f, 1.0f};

		auto state = Corrade::Containers::Array<Vector3>(2 * m_size.x() * m_size.y());

		if (m_size.x() % 2 != 0 || m_size.y() % 2 != 0)
			Debug{} << "Check cloth dimensions!";

		m_springs = std::vector<Spring>();

		for (UnsignedInt y = 0; y < m_size.y(); ++y)
		{
			state[firstByCoord(0, y)] = y * yStep + offset;

			for (UnsignedInt x = 1; x < m_size.x(); ++x)
			{
				const Spring s{y * m_size.x() + x - 1, y * m_size.x() + x, k, restLength1};
				m_springs.push_back(s);
				state[firstByCoord(x, y)] = y * yStep + x * xStep + offset;
			}
		}

		for (UnsignedInt x = 0; x < m_size.x(); ++x)
		{
			for (UnsignedInt y = 1; y < m_size.y(); ++y)
			{
				const Spring s{(y - 1) * m_size.x() + x, y * m_size.x() + x, k, restLength1};
				m_springs.push_back(s);
			}
		}

		for (UnsignedInt y = 0; y < m_size.y() - 1; ++y)
		{
			for (UnsignedInt x = 0; x < m_size.x() - 1; ++x)
			{
				const Spring s{y * m_size.x() + x, (y + 1) * m_size.x() + (x + 1), k, restLength2};
				m_springs.push_back(s);
			}
		}

		for (UnsignedInt y = 1; y < m_size.y(); ++y)
		{
			for (UnsignedInt x = 0; x < m_size.x() - 1; ++x)
			{
				const Spring s{y * m_size.x() + x, (y - 1) * m_size.x() + (x + 1), k, restLength2};
				m_springs.push_back(s);
			}
		}

		for (UnsignedInt y = 0; y < m_size.y(); ++y)
		{
			for (UnsignedInt x = 0; x < m_size.x(); ++x)
			{
				if (y < m_size.y() - 2)
				{
					const Spring s1{y * m_size.x() + x, (y + 2) * m_size.x() + x, k, restLength3};
					m_springs.push_back(s1);
				}

				if (x < m_size.x() - 2)
				{
					const Spring s2{y * m_size.x() + x, y * m_size.x() + (x + 2), k, restLength3};
					m_springs.push_back(s2);
				}

				state[firstByCoord(x, y)] = y * yStep + x * xStep + offset;
			}
		}

		setState(std::move(state));
		clearPinnedVertices();

		setPinnedVertex(0, true);
		setPinnedVertex(m_size.x() - 1, true);
	}

	UnsignedInt Cloth::firstByCoord(UnsignedInt x, UnsignedInt y) const
	{
		assert(x < m_size.x() && y < m_size.y());

		return y * 2 * m_size.x() + 2 * x;
	}

	UnsignedInt Cloth::secondByCoord(UnsignedInt x, UnsignedInt y) const
	{
		assert(x < m_size.x() && y < m_size.y());

		return y * 2 * m_size.x() + 2 * x + 1;
	}

	UnsignedInt Cloth::firstByIdx(UnsignedInt idx) const
	{
		assert(idx < m_size.x() * m_size.y());

		return 2 * idx;
	}

	UnsignedInt Cloth::secondByIdx(UnsignedInt idx) const
	{
		assert(idx < m_size.x() * m_size.y());

		return 2 * idx + 1;
	}

	Corrade::Containers::Array<Vector3> Cloth::evalDerivative(const Corrade::Containers::Array<Vector3> &state) const
	{
		const Float dragCoeff = 0.08f;
		const auto n = m_size.x() * m_size.y();
		constexpr Float mass = 0.025f;
		auto f = Corrade::Containers::Array<Vector3>(2 * n);

		for (UnsignedInt i = 0; i < state.size(); i += 2)
		{
			f[i] = state[i + 1];
		}

		for (const auto &spring : m_springs)
		{
			const Vector3 fS{fSpring(state[firstByIdx(spring.leftIdx)], state[firstByIdx(spring.rightIdx)], spring.k, spring.restLength)};
			f[secondByIdx(spring.rightIdx)] += fS;
			f[secondByIdx(spring.leftIdx)] -= fS;
		}

		const Float massInv = 1.0f / mass;

		for (UnsignedInt i = 0; i < f.size(); i += 2)
		{
			Vector3 v = f[i];
			f[i + 1] += fDrag(v, dragCoeff) + fGravity(mass);
			f[i + 1] *= massInv;
		}

		for (const auto pinnedIdx : getPinnedVertexIds())
		{
			f[firstByIdx(pinnedIdx)] = Vector3{};
			f[secondByIdx(pinnedIdx)] = Vector3{};
		}

		return f;
	}

	void eulerStep(Cloth &cloth, const Float dt)
	{
		const auto &x0 = cloth.getState();
		const auto n = x0.size();
		const auto f0 = cloth.evalDerivative(x0);
		Corrade::Containers::Array<Vector3> x1(n);

		for (auto i = 0u; i < n; ++i)
		{
			x1[i] = x0[i] + dt * f0[i];
		}

		cloth.setState(std::move(x1));
	}

	void rk4Step(Cloth &cloth, const Float dt)
	{
		const auto &x0 = cloth.getState();
		const UnsignedInt n = x0.size();
		Corrade::Containers::Array<Vector3> x1(n);
		Corrade::Containers::Array<Vector3> xT(n);

		const auto k1 = cloth.evalDerivative(x0);

		for (auto i = 0u; i < n; ++i)
		{
			xT[i] = x0[i] + (0.5f * dt) * k1[i];
		}

		const auto k2 = cloth.evalDerivative(xT);

		for (auto i = 0u; i < n; ++i)
		{
			xT[i] = x0[i] + (0.5f * dt) * k2[i];
		}

		const auto k3 = cloth.evalDerivative(xT);

		for (auto i = 0u; i < n; ++i)
		{
			xT[i] = x0[i] + dt * k3[i];
		}

		const auto k4 = cloth.evalDerivative(xT);

		for (auto i = 0u; i < n; ++i)
		{
			x1[i] = x0[i] + dt / 6.0f * (k1[i] + 2.0f * k2[i] + 2.0f * k3[i] + k4[i]);
		}

		cloth.setState(std::move(x1));
	}

} // namespace clothsim