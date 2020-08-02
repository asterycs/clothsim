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

        explicit UI(const Vector2i windowSize, const Vector2i framebufferSize, const Vector2 scaling);

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

        void setIntegratorCallback(std::function<void(std::function<void(System &, const Float)>)> f);
        void setSystemCallback(std::function<void(const std::size_t)> f);
        void setStepLengthCallback(std::function<void(const Float)> f);
        void setStepsPerFrameCallback(std::function<void(const UnsignedInt)> f);
        void setVertexMarkerVisibilityCallback(std::function<void(const bool)> f);
        void setResetCallback(std::function<void(void)> f);
        void setClearPinnedCallback(std::function<void(void)> f);
        void setViewportClickCallback(std::function<void(const Vector2i)> f);
        void setLassoCallback(std::function<void(const UI::Lasso &lasso)> f);
        void setRotateCameraCallback(std::function<void(const Vector2i)> f);
        void setZoomCameraCallback(std::function<void(const Float)> f);
        void setSizeCallback(std::function<void(const Vector2ui)> f);

    private:
        bool drawCombo(const std::string &text, const std::vector<std::string> &options, std::size_t &optionPtr);

        void drawOptions();
        void drawLasso();
        std::vector<Vector2> toScreenCoordinates(const std::vector<Vector2i> &pixels);

        std::optional<std::function<void(std::function<void(System &system, const Float dt)>)>> m_setIntegratorCallback{};
        std::optional<std::function<void(const std::size_t)>> m_setSystemCallback;
        std::optional<std::function<void(const Float)>> m_stepLengthCallback;
        std::optional<std::function<void(const UnsignedInt)>> m_stepsPerFrameCallback;
        std::optional<std::function<void(const bool)>> m_vertexMarkersVisibilityCallback;
        std::optional<std::function<void(void)>> m_resetCallback;
        std::optional<std::function<void(void)>> m_clearPinnedCallback;
        std::optional<std::function<void(const Vector2i)>> m_viewportClickCallback;
        std::optional<std::function<void(const UI::Lasso &)>> m_lassoCallback;
        std::optional<std::function<void(const Vector2i)>> m_rotateCameraCallback;
        std::optional<std::function<void(const Float)>> m_zoomCameraCallback;
        std::optional<std::function<void(const Vector2ui)>> m_sizeCallback;

        ImGuiIntegration::Context m_imgui{NoCreate};
        Vector2i m_currentWindowSize;
        Vector2i m_currentFramebufferSize;

        bool m_showVertexMarkers;
        bool m_showAbout;

        bool m_inPinnedVertexLassoMode;
        Lasso m_currentLasso;
        Vector2i m_lassoPreviousPosition;

        Float m_stepLength{0.0001f};
        UnsignedInt m_stepsPerFrame{100};

        std::vector<std::string> m_integrators{std::string{"Forward Euler"}, std::string{"RK4"}, std::string{"Backward Euler"}, std::string{"Implicit midpoint"}};
        std::size_t m_currentIntegrator;

        std::vector<std::string> m_systems{std::string{"First order oscillator"}, std::string{"Planet"}, std::string{"Cloth"}};
        std::size_t m_currentSystem;
        Vector2i m_currentSize{3, 3};

        std::string m_licenceNotice;
    };

} // namespace clothsim

#endif //CLOTHSIM_UI_H
