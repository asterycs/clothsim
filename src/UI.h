#ifndef CLOTHSIM_UI_H
#define CLOTHSIM_UI_H

#include <imgui.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

#ifndef CORRADE_TARGET_EMSCRIPTEN
#include <Magnum/Platform/Sdl2Application.h>
#else
#include <Magnum/Platform/EmscriptenApplication.h>
#endif

#include <vector>
#include <functional>
#include <memory>

#include "System.h"

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

        UI(Vector2i windowSize, Vector2i framebufferSize, Vector2 scaling, App& app);

        void resize(Vector2i windowSize, Vector2 scaling, Vector2i framebufferSize);
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
        bool drawCombo(const std::string &text, const std::vector<std::string> &options, std::size_t &optionPtr);

        void drawOptions();
        void drawLasso();
        std::vector<Vector2> toScreenCoordinates(const std::vector<Vector2i> &pixels);

        App& m_app;

        ImGuiIntegration::Context m_imgui{NoCreate};
        Vector2i m_currentWindowSize;
        Vector2i m_currentFramebufferSize;

        bool m_showVertexMarkers;
        bool m_showAbout;

        bool m_inPinnedVertexLassoMode;
        Lasso m_currentLasso;
        Vector2i m_lassoPreviousPosition;

        Float m_stepLength{0.0001f};
        UnsignedInt m_stepsPerFrame{5};

        std::vector<std::string> m_integrators{std::string{"Forward Euler"}, std::string{"RK4"}, std::string{"Backward Euler"}, std::string{"Implicit midpoint"}};
        std::size_t m_currentIntegrator{0};

        std::vector<std::string> m_systems{std::string{"First order oscillator"}, std::string{"Planet"}, std::string{"Cloth"}};
        std::size_t m_currentSystem{2};
        Vector2i m_currentSize{3, 3};

        std::string m_licenceNotice;
    };

} // namespace clothsim

#endif //CLOTHSIM_UI_H
