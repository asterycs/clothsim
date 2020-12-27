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
        using Position = Magnum::GL::Attribute<0, Vector3>;
        using Normal = Magnum::GL::Attribute<1, Vector3>;
        using VertexColor = Magnum::GL::Attribute<2, Vector3>;

        enum : UnsignedInt
        {
            ColorOutput = 0,
            ObjectIdOutput = 1
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

    private:
    };

    class VertexMarkerShader : public Magnum::GL::AbstractShaderProgram
    {
    public:
        using VertexPosition = Magnum::GL::Attribute<0, Vector3>;
        using Normal = Magnum::GL::Attribute<1, Vector3>;

        enum : UnsignedInt
        {
            ColorOutput = 0,
            ObjectIdOutput = 1
        };

        explicit VertexMarkerShader();

        VertexMarkerShader &setLightPosition(const Vector3 &position)
        {
            setUniform(uniformLocation("light"), position);
            return *this;
        }

        VertexMarkerShader &setColor(const Vector3 &color)
        {
            setUniform(uniformLocation("vertexColor"), color);
            return *this;
        }

        VertexMarkerShader &setNormalMatrix(const Matrix3x3 &matrix)
        {
            setUniform(uniformLocation("normalMatrix"), matrix);
            return *this;
        }

        VertexMarkerShader &setObjectId(Int id)
        {
            setUniform(uniformLocation("objectId"), id);
            return *this;
        }

        VertexMarkerShader &setTransformationMatrix(const Matrix4 &matrix)
        {
            setUniform(uniformLocation("transformationMatrix"), matrix);
            return *this;
        }

        VertexMarkerShader &setProjectionMatrix(const Matrix4 &matrix)
        {
            setUniform(uniformLocation("projectionMatrix"), matrix);
            return *this;
        }

    private:
    };
} // namespace clothsim

#endif //CLOTHSIM_SHADERS_H
