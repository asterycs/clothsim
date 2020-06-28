#include "MeshData.h"

#include <Magnum/MeshTools/Transform.h>

#include "Util.h"

namespace clothsim
{

    MeshData::MeshData(const UnsignedInt dimensions, const std::vector<Vector3> &vertices,
                       const std::vector<std::vector<UnsignedInt>> &elements,
                       const std::vector<UnsignedInt> &boundaryIndices)
        : m_dimensions{dimensions}, m_vertices{vertices}, m_elements{elements}, m_boundaryIndices{boundaryIndices}
    {
        initAffine();
    }

    MeshData::MeshData(const UnsignedInt dimensions, std::vector<Vector3> &&vertices,
                       std::vector<std::vector<UnsignedInt>> &&elements,
                       std::vector<UnsignedInt> &&boundaryIndices)
        : m_dimensions{dimensions}, m_vertices{std::move(vertices)}, m_elements{std::move(elements)}, m_boundaryIndices{std::move(boundaryIndices)}
    {
        initAffine();
    }

    void MeshData::initAffine()
    {
        const std::size_t n_elements = m_elements.size();

        m_affineTransform._Bkx = Eigen::MatrixXf(n_elements, 3);
        m_affineTransform._Bky = Eigen::MatrixXf(n_elements, 3);
        m_affineTransform._Bkz = Eigen::MatrixXf(n_elements, 3);

        m_affineTransform._Bkitx = Eigen::MatrixXf(n_elements, 3);
        m_affineTransform._Bkity = Eigen::MatrixXf(n_elements, 3);
        m_affineTransform._Bkitz = Eigen::MatrixXf(n_elements, 3);

        m_affineTransform._bkx = Eigen::VectorXf(n_elements);
        m_affineTransform._bky = Eigen::VectorXf(n_elements);
        m_affineTransform._bkz = Eigen::VectorXf(n_elements);

        m_affineTransform._absDetBk = Eigen::ArrayXf(n_elements);

        UnsignedInt currentRow = 0;
        for (auto elem : m_elements)
        {
            const auto [Bk, bk] = computeAffine(elem);
            const Eigen::Matrix3f Bkit = Bk.transpose().inverse();

            m_affineTransform._Bkx.row(currentRow) = Bk.row(0);
            m_affineTransform._Bky.row(currentRow) = Bk.row(1);
            m_affineTransform._Bkz.row(currentRow) = Bk.row(2);

            m_affineTransform._Bkitx.row(currentRow) = Bkit.row(0);
            m_affineTransform._Bkity.row(currentRow) = Bkit.row(1);
            m_affineTransform._Bkitz.row(currentRow) = Bkit.row(2);

            m_affineTransform._bkx(currentRow) = bk(0);
            m_affineTransform._bky(currentRow) = bk(1);
            m_affineTransform._bkz(currentRow) = bk(2);

            m_affineTransform._absDetBk(currentRow) = std::abs(Bk.determinant());

            ++currentRow;
        }
    }

    std::pair<Eigen::Matrix3f, Eigen::Vector3f> MeshData::computeAffine(const std::vector<UnsignedInt> &elemVertexIndices) const
    {
        const Eigen::Vector3f p0 = toEigen(m_vertices[elemVertexIndices[0]]);
        const Eigen::Vector3f p1 = toEigen(m_vertices[elemVertexIndices[1]]);
        const Eigen::Vector3f p2 = toEigen(m_vertices[elemVertexIndices[2]]);
        const Eigen::Vector3f p3 = toEigen(m_vertices[elemVertexIndices[3]]);

        Eigen::Matrix3f Bk;
        Bk << p1 - p0, p2 - p0, p3 - p0;
        Eigen::Vector3f bk = p0;

        return std::make_pair(Bk, bk);
    }

    UnsignedInt MeshData::getDimensions() const
    {
        return m_dimensions;
    }

    const std::vector<UnsignedInt> &MeshData::getBoundaryIndices() const
    {
        return m_boundaryIndices;
    }

    const std::vector<Vector3> &MeshData::getVertices() const
    {
        return m_vertices;
    }

    const std::vector<std::vector<UnsignedInt>> &MeshData::getElements() const
    {
        return m_elements;
    }

    void MeshData::centerToOrigin()
    {
        const AABB<Vector3> aabb = computeAABB(m_vertices);
        const Vector3 origin = 0.5f * (aabb.max - aabb.min) + aabb.min;
        MeshTools::transformPointsInPlace(Matrix4::translation(-origin), m_vertices);
    }

    const AffineTransformVectorized3D &MeshData::getAffineTransform() const
    {
        return m_affineTransform;
    }
} // namespace clothsim