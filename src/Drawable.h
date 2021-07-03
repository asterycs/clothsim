#ifndef CLOTHSIM_DRAWABLE_H
#define CLOTHSIM_DRAWABLE_H

#include <Corrade/Containers/Array.h>

#include <Magnum/Magnum.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include <Magnum/GL/Mesh.h>

#include "Shaders.h"
#include "Util.h"

namespace clothsim
{
    using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;

    class Drawable : public Object3D, Magnum::SceneGraph::Drawable3D
    {
    public:
        Drawable(PhongIdShader &phongShader,
                 VertexMarkerShader &vertexShader,
                 Object3D &parent,
                 Magnum::SceneGraph::DrawableGroup3D &drawables);

        void setVertexMarkerColors(Corrade::Containers::Array<Color3> vertexMarkerColors);

        virtual Corrade::Containers::Array<UnsignedInt> getMeshIndices() const = 0;
        virtual Corrade::Containers::Array<Magnum::Vector3> getMeshVertices() const = 0;
        virtual Corrade::Containers::Array<Magnum::Color3> getVertexMarkerColors() const = 0;

        void drawVertexMarkers(bool);

    private:
        void draw(const Matrix4 &viewProjection, Magnum::SceneGraph::Camera3D &camera) override;
        void drawMesh(const Matrix4 &viewProjection, const Magnum::SceneGraph::Camera3D &camera);
        void drawVertexMarkers(const Matrix4 &viewProjection, const Magnum::SceneGraph::Camera3D &camera);

        void initVertexMarkers();
        void initVertexMarkerColors();
        void initMesh();

        bool m_drawVertexMarkers;

        PhongIdShader &m_phongShader;
        VertexMarkerShader &m_vertexShader;

        Magnum::GL::Buffer m_triangleBuffer, m_indexBuffer, m_colorBuffer;
        Magnum::GL::Mesh m_triangles;

        Magnum::GL::Buffer m_vertexMarkerVertexBuffer;
        Magnum::GL::Buffer m_vertexMarkerIndexBuffer;
        Magnum::GL::Mesh m_vertexMarkerMesh;
    };
} // namespace clothsim

#endif //CLOTHSIM_DRAWABLE_H
