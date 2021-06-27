#ifndef CLOTHSIM_APP_H
#define CLOTHSIM_APP_H

#include <Corrade/Containers/Reference.h>
#include <Corrade/Interconnect/Receiver.h>

#ifdef CORRADE_TARGET_EMSCRIPTEN
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif

#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Camera.h>

#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderbuffer.h>

#include <memory>

#include "Cloth.h"
#include "Integrators.h"
#include "Planet.h"
#include "Oscillator.h"
#include "UI.h"
#include "Shaders.h"

namespace clothsim
{
    using Scene3D = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;
    using Integrator = std::function<void(System &, const float)>;

    class App : public Platform::Application
    {
    public:
        explicit App(const Arguments &arguments);
        virtual ~App(){};

        void setVertexMarkersVisibility(bool show);
        void pinVertices(const UI::Lasso &lasso);

        void zoomCamera(const Float offset);
        void handleViewportClick(const Vector2i position);
        void rotateCamera(const Vector2i offset);
        void resetSimulation();

        void setStepLength(const Float stepLength);
        void setStepsPerFrame(const UnsignedInt steps);
        const std::unique_ptr<System> &getSystem();

        void setIntegrator(std::function<void(System &system, const Float dt)> integrator);

        void setSystem(const std::size_t i);

    private:
        void viewportEvent(ViewportEvent &event) override;
        void drawEvent() override;
        void mousePressEvent(MouseEvent &event) override;
        void mouseMoveEvent(MouseMoveEvent &event) override;
        void mouseReleaseEvent(MouseEvent &event) override;
        void mouseScrollEvent(MouseScrollEvent &event) override;
        void keyPressEvent(KeyEvent &event) override;
        void keyReleaseEvent(KeyEvent &event) override;
        void textInputEvent(TextInputEvent &event) override;

        void resizeFramebuffers(const Vector2i &size);
        void resizeRenderbuffers(const Vector2i &size);
        void resizeTextures(const Vector2i &size);
        void resizeCamera(const Vector2i &size);

        Scene3D m_scene{};
        std::unique_ptr<Object3D> m_cameraObject{};
        std::unique_ptr<Magnum::SceneGraph::Camera3D> m_camera{};
        Magnum::SceneGraph::DrawableGroup3D m_drawableGroup{};

        PhongIdShader m_phongShader{};
        VertexMarkerShader m_vertexShader{};

        std::unique_ptr<System> m_system{};

        Magnum::GL::Framebuffer m_framebuffer;
        Magnum::GL::Renderbuffer m_particleId{}, m_depth{};
        Magnum::GL::Texture2D m_color{};

        Vector2 m_cameraTrackballAngles{0.f};

        Float m_stepLength{0.0f};
        UnsignedInt m_stepsPerFrame{0};
        Integrator m_integrator{forwardEulerStep};

        UI m_ui;
    };

} // namespace clothsim

#endif
