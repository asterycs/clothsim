#ifndef CLOTHSIM_APP_H
#define CLOTHSIM_APP_H

#include <Corrade/Containers/Reference.h>
#include <Corrade/Interconnect/Receiver.h>

#ifndef CORRADE_TARGET_EMSCRIPTEN
#include <Magnum/Platform/Sdl2Application.h>
#else
#include <Magnum/Platform/EmscriptenApplication.h>
#endif

#include <Magnum/Timeline.h>

#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Camera.h>

#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderbuffer.h>

#include <memory>
#include <optional>

#include "Cloth.h"
#include "Integrators.h"
#include "Planet.h"
#include "Oscillator.h"
#include "UI.h"
#include "Shaders.h"

namespace clothsim
{
    using Scene3D = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;

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

        template <typename F>
        void setIntegrator(F &&f)
        {
            m_integrator = std::forward<F &&>(f);
        }

        template <typename SystemT>
        void setSystem()
        {
            m_system = std::make_unique<SystemT>(m_phongShader, m_vertexShader, m_scene, m_drawableGroup);
        }

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
        CompositionShader m_compositionShader{};

        std::unique_ptr<System> m_system{};

        Magnum::GL::Framebuffer m_framebuffer;
        Magnum::GL::Renderbuffer m_vertexId{}, m_depth{};
        Magnum::GL::Texture2D m_color{}, m_transparencyAccumulation{}, m_transparencyRevealage{};

        Vector2 m_cameraTrackballAngles{0.f};

        Float m_stepLength{0.0f};
        UnsignedInt m_stepsPerFrame{0};
        std::optional<std::function<void(System &, const float)>> m_integrator{};

        UI m_ui;
    };

} // namespace clothsim

#endif
