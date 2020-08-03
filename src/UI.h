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
        template <typename T>
        using opt = std::optional<T>;
        template <typename T>
        using fun = std::function<T>;

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

        void setIntegratorCallback(opt<fun<void(std::function<void(System &, const Float)>)>> f);
        void setSystemCallback(opt<fun<void(const std::size_t)>> f);
        void setStepLengthCallback(opt<fun<void(const Float)>> f);
        void setStepsPerFrameCallback(opt<fun<void(const UnsignedInt)>> f);
        void setVertexMarkerVisibilityCallback(opt<fun<void(const bool)>> f);
        void setResetCallback(opt<fun<void(void)>> f);
        void setClearPinnedCallback(opt<fun<void(void)>> f);
        void setViewportClickCallback(opt<fun<void(const Vector2i)>> f);
        void setLassoCallback(opt<fun<void(const UI::Lasso &lasso)>> f);
        void setRotateCameraCallback(opt<fun<void(const Vector2i)>> f);
        void setZoomCameraCallback(opt<fun<void(const Float)>> f);
        void setSizeCallback(opt<fun<void(const Vector2ui)>> f);

    private:
        bool drawCombo(const std::string &text, const std::vector<std::string> &options, std::size_t &optionPtr);

        void drawOptions();
        void drawLasso();
        std::vector<Vector2> toScreenCoordinates(const std::vector<Vector2i> &pixels);

        std::optional<fun<void(std::function<void(System &system, const Float dt)>)>> m_setIntegratorCallback{};
        std::optional<fun<void(const std::size_t)>> m_setSystemCallback;
        std::optional<fun<void(const Float)>> m_stepLengthCallback;
        std::optional<fun<void(const UnsignedInt)>> m_stepsPerFrameCallback;
        std::optional<fun<void(const bool)>> m_vertexMarkersVisibilityCallback;
        std::optional<fun<void(void)>> m_resetCallback;
        std::optional<fun<void(void)>> m_clearPinnedCallback;
        std::optional<fun<void(const Vector2i)>> m_viewportClickCallback;
        std::optional<fun<void(const UI::Lasso &)>> m_lassoCallback;
        std::optional<fun<void(const Vector2i)>> m_rotateCameraCallback;
        std::optional<fun<void(const Float)>> m_zoomCameraCallback;
        std::optional<fun<void(const Vector2ui)>> m_sizeCallback;

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
