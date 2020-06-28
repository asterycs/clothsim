#ifndef CLOTHSIM_CLOTH_H
#define CLOTHSIM_CLOTH_H

#include <Magnum/Math/Color.h>

#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include <Magnum/GL/Mesh.h>

#include <set>

#include "Shaders.h"
#include "Util.h"

namespace clothsim
{
    using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;

    class Cloth : public Object3D, Magnum::SceneGraph::Drawable3D
    {
    public:
        explicit Cloth(PhongIdShader &phongShader,
                       VertexShader &vertexShader,
                       Object3D &parent,
                       Magnum::SceneGraph::DrawableGroup3D &drawables);

        void togglePinnedVertex(const UnsignedInt vertexId);
        void setPinnedVertex(const UnsignedInt vertexId, const bool pinned);
        const std::set<UnsignedInt> &getPinnedVertexIds() const;
        void clearPinnedVertices();

        void drawVertexMarkers(const bool);

        void setVertexColors(const std::vector<Vector3> &colors);

    private:
        void draw(const Matrix4 &transformationMatrix, Magnum::SceneGraph::Camera3D &camera) override;
        void drawMesh(const Matrix4 &transformationMatrix, const Magnum::SceneGraph::Camera3D &camera);
        void drawVertexMarkers(const Matrix4 &transformationMatrix, const Magnum::SceneGraph::Camera3D &camera);

        void initVertexMarkers(const std::vector<Vector3> &vertices);
        void initMeshTriangles(std::vector<Vector3> vertices, std::vector<UnsignedInt> triangleIndices);

        bool m_drawVertexMarkers;
        std::set<UnsignedInt> m_pinnedVertexIds;

        PhongIdShader &m_phongShader;
        VertexShader &m_vertexShader;

        Magnum::GL::Buffer m_triangleBuffer, m_indexBuffer, m_colorBuffer;
        Magnum::GL::Mesh m_triangles;

        std::vector<Magnum::GL::Buffer> m_vertexMarkerVertexBuffer;
        std::vector<Magnum::GL::Buffer> m_vertexMarkerIndexBuffer;
        std::vector<Magnum::GL::Mesh> m_vertexMarkerMesh;

        std::vector<UnsignedInt> m_triangleIndices;
    };
} // namespace clothsim

#endif //CLOTHSIM_CLOTH_H
