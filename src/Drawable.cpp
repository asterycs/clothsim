#include "Drawable.h"

#include <Magnum/MeshTools/GenerateFlatNormals.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/MeshTools/Transform.h>
#include <Magnum/MeshTools/Duplicate.h>

#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/Trade/MeshData3D.h>

namespace clothsim
{
    Drawable::Drawable(PhongIdShader &phongShader,
                       VertexMarkerShader &vertexShader,
                       Object3D &parent,
                       Magnum::SceneGraph::DrawableGroup3D &drawables)
        : Object3D{&parent},
          Magnum::SceneGraph::Drawable3D{*this, &drawables},
          m_drawVertexMarkers{true},
          m_phongShader{phongShader},
          m_vertexShader{vertexShader},
          m_triangleBuffer{Magnum::GL::Buffer::TargetHint::Array},
          m_indexBuffer{Magnum::GL::Buffer::TargetHint::ElementArray},
          m_colorBuffer{Magnum::GL::Buffer::TargetHint::Array}
    {
        initVertexMarkers();
    }

    void Drawable::setVertexData(Corrade::Containers::Array<Vector3> indexedVertices, Corrade::Containers::Array<UnsignedInt> triangleIndices)
    {
        m_indexedVertices = std::move(indexedVertices);
        m_triangleIndices = std::move(triangleIndices);

        initMesh();
    }

    void Drawable::initMesh()
    {
        const Corrade::Containers::Array<Vector3> verticesExpanded{
            Magnum::MeshTools::duplicate<UnsignedInt, Vector3>(m_triangleIndices, m_indexedVertices)};

        const Corrade::Containers::Array<Vector3> normals{Magnum::MeshTools::generateFlatNormals(verticesExpanded)};

        std::vector<Vector3> colors(m_triangleIndices.size(), Vector3{1.f, 1.f, 1.f});

        m_triangleBuffer.setData(Magnum::MeshTools::interleave(verticesExpanded, normals), Magnum::GL::BufferUsage::StaticDraw);
        m_colorBuffer.setData(colors, Magnum::GL::BufferUsage::StaticDraw);

        m_triangles.setPrimitive(Magnum::GL::MeshPrimitive::Triangles)
            .addVertexBuffer(m_triangleBuffer, 0, PhongIdShader::Position{}, PhongIdShader::Normal{})
            .addVertexBuffer(m_colorBuffer, 0, PhongIdShader::VertexColor{})
            .setCount(static_cast<Int>(m_triangleIndices.size()));
    }

    void Drawable::initVertexMarkers()
    {
        const auto data{Magnum::Primitives::uvSphereSolid(16, 32)};

        constexpr float radius{0.02f};
        const auto vertices{Magnum::MeshTools::transformPoints(Matrix4::scaling(Vector3{radius}), data.positions3DAsArray(0))};
        const auto normals{data.normalsAsArray(0)};

        m_vertexMarkerVertexBuffer.setTargetHint(Magnum::GL::Buffer::TargetHint::Array);
        m_vertexMarkerVertexBuffer.setData(Magnum::MeshTools::interleave(vertices, normals),
                                           Magnum::GL::BufferUsage::StaticDraw);

        m_vertexMarkerIndexBuffer.setTargetHint(Magnum::GL::Buffer::TargetHint::ElementArray);
        m_vertexMarkerIndexBuffer.setData(data.indicesAsArray(), Magnum::GL::BufferUsage::StaticDraw);

        m_vertexMarkerMesh.setCount(data.indexCount());
        m_vertexMarkerMesh.setPrimitive(data.primitive());
        m_vertexMarkerMesh.addVertexBuffer(m_vertexMarkerVertexBuffer, 0, VertexMarkerShader::VertexPosition{}, VertexMarkerShader::Normal{});
        m_vertexMarkerMesh.setIndexBuffer(m_vertexMarkerIndexBuffer, 0, Magnum::MeshIndexType::UnsignedInt);
    }

    void Drawable::setVertexColors(Corrade::Containers::Array<Color3> indexedColors)
    {
        const auto colorsExpanded{Magnum::MeshTools::duplicate<UnsignedInt, Color3>(m_triangleIndices, indexedColors)};

        m_colorBuffer.setData(colorsExpanded, Magnum::GL::BufferUsage::StaticDraw);
    }

    void Drawable::setVertexMarkerColors(Corrade::Containers::Array<Color3> vertexMarkerColors)
    {
        m_vertexMarkerColors = std::move(vertexMarkerColors);
    }

    void Drawable::drawVertexMarkers(const bool draw)
    {
        m_drawVertexMarkers = draw;
    }

    void Drawable::draw(const Matrix4 &viewProjection, Magnum::SceneGraph::Camera3D &camera)
    {
        drawMesh(viewProjection, camera);

        if (m_drawVertexMarkers)
        {
            drawVertexMarkers(viewProjection, camera);
        }
    }

    void Drawable::drawVertexMarkers(const Matrix4 &viewProjection, const Magnum::SceneGraph::Camera3D &camera)
    {
        for (UnsignedInt i = 0; i < m_indexedVertices.size(); ++i)
        {
            m_vertexShader.setTransformationMatrix(
                              viewProjection * Matrix4::translation(viewProjection.inverted().backward() * 0.01f) *
                              Matrix4::translation(m_indexedVertices[i]))
                .setNormalMatrix(viewProjection.rotationScaling())
                .setProjectionMatrix(camera.projectionMatrix())
                .setLightPosition({13.0f, 2.0f, 5.0f});

            const auto color = m_vertexMarkerColors[i];
            m_vertexShader.setColor(color);

            m_vertexShader.setObjectId(static_cast<Int>(i));

            m_vertexShader.draw(m_vertexMarkerMesh);
        }
    }

    void Drawable::drawMesh(const Matrix4 &viewProjection, const Magnum::SceneGraph::Camera3D &camera)
    {
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);

        m_phongShader.setTransformationMatrix(viewProjection)
            .setNormalMatrix(viewProjection.rotationScaling())
            .setProjectionMatrix(camera.projectionMatrix())
            .setLightPosition({13.0f, 2.0f, 5.0f}); // Relative to camera

        m_phongShader.draw(m_triangles);

        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    }
} // namespace clothsim