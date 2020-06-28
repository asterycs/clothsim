#include "Shaders.h"

#include <Magnum/GL/Version.h>
#include <Magnum/GL/Shader.h>
#ifndef MAGNUM_TARGET_EMSCRIPTEN
#include <Corrade/Utility/Resource.h>
#include <Corrade/Containers/Reference.h>

#include <fstream>
#include <streambuf>
#endif

#include <stdexcept>

namespace clothsim
{

    PhongIdShader::PhongIdShader()
    {
#ifndef MAGNUM_TARGET_GLES
        Magnum::GL::Shader vert{Magnum::GL::Version::GL430, Magnum::GL::Shader::Type::Vertex},
            frag{Magnum::GL::Version::GL430, Magnum::GL::Shader::Type::Fragment};
#else
        Magnum::GL::Shader vert{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Vertex},
            frag{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Fragment};
#endif

#ifdef CORRADE_TARGET_EMSCRIPTEN
        Magnum::Utility::Resource rs("clothsim-data");

        vert.addSource(rs.get("PhongTransparentId.vert"));
        frag.addSource(rs.get("PhongTransparentId.frag"));
        CORRADE_INTERNAL_ASSERT(Magnum::GL::Shader::compile({vert, frag}));
        attachShaders({vert, frag});
        CORRADE_INTERNAL_ASSERT(link());
#else
        std::ifstream vertSrc(CLOTHSIM_SOURCE_DIRECTORY "/PhongTransparentId.vert");
        std::ifstream fragSrc(CLOTHSIM_SOURCE_DIRECTORY "/PhongTransparentId.frag");

        std::string vertStr((std::istreambuf_iterator<char>(vertSrc)),
                            std::istreambuf_iterator<char>());

        std::string fragStr((std::istreambuf_iterator<char>(fragSrc)),
                            std::istreambuf_iterator<char>());

        vert.addSource(vertStr);
        frag.addSource(fragStr);

        if (!Magnum::GL::Shader::compile({vert, frag}))
            throw std::runtime_error("Shader compilation failed");

        attachShaders({vert, frag});

        if (!link())
            throw std::runtime_error("Shader linking failed");
#endif
    }

    VertexShader::VertexShader()
    {
        Magnum::Utility::Resource rs("clothsim-data");

#ifndef MAGNUM_TARGET_GLES
        Magnum::GL::Shader vert{Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Vertex},
            frag{Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Fragment};
#else
        Magnum::GL::Shader vert{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Vertex},
            frag{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Fragment};
#endif
        vert.addSource(rs.get("Vertex.vert"));
        frag.addSource(rs.get("Vertex.frag"));
        CORRADE_INTERNAL_ASSERT(Magnum::GL::Shader::compile({vert, frag}));
        attachShaders({vert, frag});
        CORRADE_INTERNAL_ASSERT(link());

        m_colorUniform = uniformLocation("color");
        m_objectIdUniform = uniformLocation("objectId");
        m_transformationMatrixUniform = uniformLocation("transformationMatrix");
        m_projectionMatrixUniform = uniformLocation("projectionMatrix");
    }

    CompositionShader::CompositionShader()
    {
        Magnum::Utility::Resource rs("clothsim-data");

#ifndef MAGNUM_TARGET_GLES
        Magnum::GL::Shader vert{Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Vertex},
            frag{Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Fragment};
#else
        Magnum::GL::Shader vert{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Vertex},
            frag{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Fragment};
#endif
        vert.addSource(rs.get("Composition.vert"));
        frag.addSource(rs.get("Composition.frag"));
        CORRADE_INTERNAL_ASSERT(Magnum::GL::Shader::compile({vert, frag}));
        attachShaders({vert, frag});
        CORRADE_INTERNAL_ASSERT(link());
    }

} // namespace clothsim