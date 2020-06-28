#ifndef CLOTHSIM_UI_H
#define CLOTHSIM_UI_H

#include <imgui.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Magnum.h>

namespace clothsim
{
    using namespace Magnum;
    class App;

    class UI
    {
    public:
        struct Lasso
        {
            std::vector<Vector2i> pixels;
            std::vector<Vector2> screenCoord;

            void clear()
            {
                pixels.clear();
                screenCoord.clear();
            }
        };

        explicit UI(App &app, const Vector2i windowSize, const Vector2i framebufferSize, const Vector2 scaling);

        void resize(const Vector2i windowSize, const Vector2 scaling, const Vector2i framebufferSize);
        void draw();

        bool wantsTextInput();

        bool handleKeyPressEvent(Platform::Application::KeyEvent &event);
        bool handleKeyReleaseEvent(Platform::Application::KeyEvent &event);

        bool handleMousePressEvent(Platform::Application::MouseEvent &event);
        bool handleMouseReleaseEvent(Platform::Application::MouseEvent &event);
        bool handleMouseMoveEvent(Platform::Application::MouseMoveEvent &event);
        bool handleMouseScrollEvent(Platform::Application::MouseScrollEvent &event);
        bool handleTextInputEvent(Platform::Application::TextInputEvent &event);

    private:
        void drawOptions();
        void drawLasso();
        std::vector<Vector2> toScreenCoordinates(const std::vector<Vector2i> &pixels);

        ImGuiIntegration::Context m_imgui{NoCreate};
        Vector2i m_currentWindowSize;
        Vector2i m_currentFramebufferSize;

        bool m_showVertexMarkers;
        bool m_showAbout;

        bool m_inPinnedVertexLassoMode;
        Lasso m_currentLasso;
        Vector2i m_lassoPreviousPosition;

        std::string m_licenceNotice;
        App &m_app;
    };

} // namespace clothsim

#endif //CLOTHSIM_UI_H
