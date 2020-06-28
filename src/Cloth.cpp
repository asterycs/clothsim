#include "Cloth.h"

#include <Corrade/Containers/StridedArrayView.h>

#include <Magnum/MeshTools/GenerateFlatNormals.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/CombineIndexedArrays.h>
#include <Magnum/MeshTools/Transform.h>
#include <Magnum/MeshTools/Duplicate.h>

#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/Trade/MeshData3D.h>

namespace clothsim
{

    Cloth::Cloth(PhongIdShader &phongShader,
                 VertexShader &vertexShader,
                 Object3D &parent,
                 Magnum::SceneGraph::DrawableGroup3D &drawables)
        : Object3D{&parent},
          Magnum::SceneGraph::Drawable3D{*this, &drawables},
          m_drawVertexMarkers{true},
          m_pinnedVertexIds{},
          m_phongShader{phongShader},
          m_vertexShader{vertexShader},
          m_triangleBuffer{Magnum::GL::Buffer::TargetHint::Array},
          m_indexBuffer{Magnum::GL::Buffer::TargetHint::ElementArray},
          m_colorBuffer{Magnum::GL::Buffer::TargetHint::Array}
    {
        // Expand tetrahedrons to triangles for visualization
        //const auto triangleIndices = extractTriangleIndices(mesh.getElements());

        const std::vector<UnsignedInt> triangleIndices{0, 1, 2, 1, 3, 2};
        const std::vector<Vector3> vertices{{-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}, {0.5f, 0.5f, 0.0f}};

        initVertexMarkers();
        initMeshTriangles(vertices, triangleIndices);
    }

    void Cloth::initMeshTriangles(std::vector<Vector3> indexedVertices, std::vector<UnsignedInt> triangleIndices)
    {
        m_triangleIndices = triangleIndices;
        m_indexedVertices = indexedVertices;

        Corrade::Containers::ArrayView<UnsignedInt> triangleIndicesView{triangleIndices};
        Corrade::Containers::ArrayView<Vector3> indexedVerticesView{indexedVertices};

        Corrade::Containers::Array<Vector3> verticesExpanded =
            Magnum::MeshTools::duplicate<UnsignedInt, Vector3>(triangleIndicesView, indexedVerticesView);

        Corrade::Containers::Array<Vector3> normals = Magnum::MeshTools::generateFlatNormals(verticesExpanded);

        std::vector<Vector3> colors(triangleIndices.size(), Vector3{1.f, 1.f, 1.f});

        m_triangleBuffer.setData(Magnum::MeshTools::interleave(verticesExpanded, normals), Magnum::GL::BufferUsage::StaticDraw);
        m_colorBuffer.setData(colors, Magnum::GL::BufferUsage::StaticDraw);

        // Using a vertex buffer would be beneficial but that makes updating colors later much more difficult
        m_triangles.setPrimitive(Magnum::GL::MeshPrimitive::Triangles)
            .addVertexBuffer(m_triangleBuffer, 0, PhongIdShader::Position{}, PhongIdShader::Normal{})
            .addVertexBuffer(m_colorBuffer, 0, PhongIdShader::VertexColor{})
            .setCount(static_cast<Int>(triangleIndices.size()));
    }

    void Cloth::initVertexMarkers()
    {
        const auto data = Magnum::Primitives::uvSphereSolid(16, 32);

        const auto vertices = Magnum::MeshTools::transformPoints(Matrix4::scaling({0.03f, 0.03f, 0.03f}), data.positions3DAsArray(0));
        const auto normals = data.normalsAsArray(0);

        m_vertexMarkerVertexBuffer.setTargetHint(Magnum::GL::Buffer::TargetHint::Array);
        m_vertexMarkerVertexBuffer.setData(Magnum::MeshTools::interleave(vertices, normals),
                                           Magnum::GL::BufferUsage::StaticDraw);

        m_vertexMarkerIndexBuffer.setTargetHint(Magnum::GL::Buffer::TargetHint::ElementArray);
        m_vertexMarkerIndexBuffer.setData(data.indicesAsArray(), Magnum::GL::BufferUsage::StaticDraw);

        m_vertexMarkerMesh.setCount(data.indexCount());
        m_vertexMarkerMesh.setPrimitive(data.primitive());
        m_vertexMarkerMesh.addVertexBuffer(m_vertexMarkerVertexBuffer, 0, PhongIdShader::Position{}, PhongIdShader::Normal{});
        m_vertexMarkerMesh.setIndexBuffer(m_vertexMarkerIndexBuffer, 0, Magnum::MeshIndexType::UnsignedInt);
    }

    void Cloth::setVertexColors(const std::vector<Vector3> &colors)
    {
        std::vector<Vector3> expandedColor = expand(colors, m_triangleIndices);
        m_colorBuffer.setData(expandedColor, Magnum::GL::BufferUsage::StaticDraw);
    }

    void Cloth::draw(const Matrix4 &transformationMatrix, Magnum::SceneGraph::Camera3D &camera)
    {
        drawMesh(transformationMatrix, camera);

        if (m_drawVertexMarkers)
        {
            drawVertexMarkers(transformationMatrix, camera);
        }
    }

    void Cloth::drawVertexMarkers(const Matrix4 &transformationMatrix, const Magnum::SceneGraph::Camera3D &camera)
    {
        for (UnsignedInt i = 0; i < m_indexedVertices.size(); ++i)
        {
            m_vertexShader.setTransformationMatrix(
                              transformationMatrix * Matrix4::translation(transformationMatrix.inverted().backward() * 0.01f) *
                              Matrix4::translation(m_indexedVertices[i]))
                .setProjectionMatrix(camera.projectionMatrix());

            if (m_pinnedVertexIds.find(i) != m_pinnedVertexIds.end())
                m_vertexShader.setColor({1.f, 0.f, 0.f});
            else
                m_vertexShader.setColor({1.f, 1.f, 1.f});

            m_vertexShader.setObjectId(static_cast<Int>(i));

            m_vertexShader.draw(m_vertexMarkerMesh);
        }
    }

    void Cloth::drawMesh(const Matrix4 &transformationMatrix, const Magnum::SceneGraph::Camera3D &camera)
    {
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
        Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add,
                                               Magnum::GL::Renderer::BlendEquation::Add);
        Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha,
                                               Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);

        m_phongShader.setTransformationMatrix(transformationMatrix)
            .setNormalMatrix(transformationMatrix.rotationScaling())
            .setProjectionMatrix(camera.projectionMatrix())
            .setDepthScale(0.5f)
            .setLightPosition({13.0f, 2.0f, 5.0f}); // Relative to camera

        m_phongShader.draw(m_triangles);

        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    }

    void Cloth::togglePinnedVertex(const UnsignedInt vertexId)
    {
        const auto pos = m_pinnedVertexIds.find(vertexId);
        if (pos != m_pinnedVertexIds.end())
            m_pinnedVertexIds.erase(pos);
        else
            m_pinnedVertexIds.insert(vertexId);
    }

    void Cloth::clearPinnedVertices()
    {
        m_pinnedVertexIds.clear();
    }

    void Cloth::drawVertexMarkers(const bool draw)
    {
        m_drawVertexMarkers = draw;
    }

    void Cloth::setPinnedVertex(const UnsignedInt vertexId, const bool pinned)
    {
        const auto pos = m_pinnedVertexIds.find(vertexId);
        if (!pinned)
        {
            if (pos != m_pinnedVertexIds.end())
                m_pinnedVertexIds.erase(pos);
        }
        else
        {
            m_pinnedVertexIds.insert(vertexId);
        }
    }

    const std::set<UnsignedInt> &Cloth::getPinnedVertexIds() const
    {
        return m_pinnedVertexIds;
    }
} // namespace clothsim