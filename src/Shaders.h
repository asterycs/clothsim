#ifndef CLOTHSIM_SHADERS_H
#define CLOTHSIM_SHADERS_H

#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Renderer.h>

namespace clothsim
{
    using namespace Magnum;

    class PhongIdShader : public Magnum::GL::AbstractShaderProgram
    {
    public:
        typedef Magnum::GL::Attribute<0, Vector3> Position;
        typedef Magnum::GL::Attribute<1, Vector3> Normal;
        typedef Magnum::GL::Attribute<2, Vector3> VertexColor;

        enum : UnsignedInt
        {
            ColorOutput = 0,
            ObjectIdOutput = 1,
            TransparencyAccumulationOutput = 2,
            TransparencyRevealageOutput = 3
        };

        explicit PhongIdShader();

        PhongIdShader &setLightPosition(const Vector3 &position)
        {
            setUniform(uniformLocation("light"), position);
            return *this;
        }

        PhongIdShader &setTransformationMatrix(const Matrix4 &matrix)
        {
            setUniform(uniformLocation("transformationMatrix"), matrix);
            return *this;
        }

        PhongIdShader &setNormalMatrix(const Matrix3x3 &matrix)
        {
            setUniform(uniformLocation("normalMatrix"), matrix);
            return *this;
        }

        PhongIdShader &setProjectionMatrix(const Matrix4 &matrix)
        {
            setUniform(uniformLocation("projectionMatrix"), matrix);
            return *this;
        }

        PhongIdShader &setDepthScale(const Float scale)
        {
            setUniform(uniformLocation("depthScale"), scale);
            return *this;
        }

    private:
    };

    class VertexShader : public Magnum::GL::AbstractShaderProgram
    {
    public:
        typedef Magnum::GL::Attribute<0, Vector3> Position;

        enum : UnsignedInt
        {
            ColorOutput = 0,
            ObjectIdOutput = 1
        };

        explicit VertexShader();

        VertexShader &setColor(const Vector3 &color)
        {
            setUniform(m_colorUniform, color);
            return *this;
        }

        VertexShader &setObjectId(Int id)
        {
            setUniform(m_objectIdUniform, id);
            return *this;
        }

        VertexShader &setTransformationMatrix(const Matrix4 &matrix)
        {
            setUniform(m_transformationMatrixUniform, matrix);
            return *this;
        }

        VertexShader &setProjectionMatrix(const Matrix4 &matrix)
        {
            setUniform(m_projectionMatrixUniform, matrix);
            return *this;
        }

    private:
        Int m_objectIdUniform,
            m_transformationMatrixUniform,
            m_projectionMatrixUniform,
            m_colorUniform;
    };

    class CompositionShader : public Magnum::GL::AbstractShaderProgram
    {
    public:
        typedef Magnum::GL::Attribute<0, Vector3> Position;

        enum : UnsignedInt
        {
            ColorOutput = 0
        };

        enum : Int
        {
            Opaque = 0,
            TransparencyAccumulation = 1,
            TransparencyRevealage = 2
        };

        explicit CompositionShader();

        CompositionShader &setOpaqueTexture(Magnum::GL::Texture2D &texture)
        {
            setUniform(uniformLocation("Opaque"), Opaque);
            texture.bind(Opaque);
            return *this;
        }

        CompositionShader &setTransparencyAccumulationTexture(Magnum::GL::Texture2D &texture)
        {
            setUniform(uniformLocation("TransparencyAccumulation"), TransparencyAccumulation);
            texture.bind(TransparencyAccumulation);
            return *this;
        }

        CompositionShader &setTransparencyRevealageTexture(Magnum::GL::Texture2D &texture)
        {
            setUniform(uniformLocation("TransparencyRevealage"), TransparencyRevealage);
            texture.bind(TransparencyRevealage);
            return *this;
        }

        CompositionShader &setViewportSize(Vector2i size)
        {
            setUniform(uniformLocation("viewportSize"), size);
            return *this;
        }

    private:
    };
} // namespace clothsim

#endif //CLOTHSIM_SHADERS_H
