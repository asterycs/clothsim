#ifndef CLOTHSIM_MESHDATA_H
#define CLOTHSIM_MESHDATA_H

#include <Magnum/Magnum.h>

#include "Eigen/Dense"

#include "Util.h"

#include <vector>

namespace clothsim
{
    using namespace Magnum;

    class MeshData
    {
    public:
        MeshData() = default;

        MeshData(const UnsignedInt dimensions, const std::vector<Vector3> &vertices,
                 const std::vector<std::vector<UnsignedInt>> &elements,
                 const std::vector<UnsignedInt> &boundaryIndices);

        MeshData(const UnsignedInt dimensions, std::vector<Vector3> &&vertices,
                 std::vector<std::vector<UnsignedInt>> &&elements,
                 std::vector<UnsignedInt> &&boundaryIndices);

        void centerToOrigin();

        const std::vector<UnsignedInt> &getBoundaryIndices() const;
        const std::vector<Vector3> &getVertices() const;
        const std::vector<std::vector<UnsignedInt>> &getElements() const;
        UnsignedInt getDimensions() const;

        const AffineTransformVectorized3D &getAffineTransform() const;

    private:
        void initAffine();

        std::pair<Eigen::Matrix3f, Eigen::Vector3f> computeAffine(const std::vector<UnsignedInt> &elemVertexIndices) const;

        UnsignedInt m_dimensions;
        std::vector<Vector3> m_vertices;
        std::vector<std::vector<UnsignedInt>> m_elements;
        std::vector<UnsignedInt> m_boundaryIndices;

        AffineTransformVectorized3D m_affineTransform;
    };
} // namespace clothsim

#endif //CLOTHSIM_MESHDATA_H