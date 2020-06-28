#ifndef CLOTHSIM_APP_H
#define CLOTHSIM_APP_H

#include <Corrade/Containers/Reference.h>
#include <Corrade/Interconnect/Receiver.h>

#include <Magnum/Platform/Sdl2Application.h>

#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Camera.h>

#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderbuffer.h>

#include <memory>
#include <optional>

#include "Cloth.h"
#include "UI.h"

namespace clothsim
{
    using Scene3D = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;

    class App : public Magnum::Platform::Application
    {
    public:
        explicit App(const Arguments &arguments);
        virtual ~App(){};

        void setVertexMarkersVisibility(bool show);
        void solveCurrent(bool showGradient);
        void setCurrentGeometry(UnsignedInt geometry);
        void clearPinnedVertices();

        void pinVertices(const UI::Lasso &lasso);

        void zoomCamera(const Float offset);
        void handleViewportClick(const Vector2i position);
        void rotateCamera(const Vector2i offset);

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
        VertexShader m_vertexSelectionShader{};
        CompositionShader m_compositionShader{};

        std::optional<Cloth> m_cloth{};

        Magnum::GL::Framebuffer m_framebuffer;
        Magnum::GL::Renderbuffer m_vertexId{}, m_depth{};
        Magnum::GL::Texture2D m_color{}, m_transparencyAccumulation{}, m_transparencyRevealage{};

        Vector2 m_cameraTrackballAngles{0.f};

        UI m_ui;
    };

} // namespace clothsim

#endif
