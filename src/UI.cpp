#include "UI.h"

#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Context.h>

#include <Magnum/Shaders/Flat.h>

#include <cstdio>

#include "Util.h"
#include "Integrators.h"
#include "Oscillator.h"
#include "Planet.h"
#include "Cloth.h"

namespace clothsim
{

    using namespace Magnum::Math::Literals;
    using namespace Magnum;

    bool UI::drawCombo(const std::string &text, const std::vector<std::string> &options, std::size_t &optionPtr)
    {
        const ImGuiStyle &style = ImGui::GetStyle();
        const float width = ImGui::CalcItemWidth();
        const float spacing = style.ItemInnerSpacing.x;
        const float buttonSize = ImGui::GetFrameHeight();

        bool changed{false};

        ImGui::PushItemWidth(width - spacing * 2.0f - buttonSize * 2.0f);

        if (ImGui::BeginCombo(("##" + text).c_str(), options[optionPtr].c_str(), ImGuiComboFlags_NoArrowButton))
        {
            for (std::size_t i = 0; i < options.size(); ++i)
            {
                const bool isSelected = (optionPtr == i);

                if (ImGui::Selectable(options[i].c_str(), isSelected))
                {
                    optionPtr = i;
                    changed = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::PopItemWidth();

        ImGui::SameLine(0, spacing);

        if (ImGui::ArrowButton(("##l" + text).c_str(), ImGuiDir_Left))
        {
            optionPtr = optionPtr > 0 ? optionPtr - 1 : options.size() - 1;
            changed = true;
        }

        ImGui::SameLine(0, spacing);

        if (ImGui::ArrowButton(("##r" + text).c_str(), ImGuiDir_Right))
        {
            optionPtr = (optionPtr + 1) % options.size();
            changed = true;
        }

        ImGui::SameLine(0, style.ItemInnerSpacing.x);
        ImGui::Text("%s", text.c_str());

        return changed;
    }

    UI::UI(const Vector2i windowSize, const Vector2i framebufferSize, const Vector2 scaling)
        : m_imgui{NoCreate}, m_currentWindowSize{windowSize}, m_currentFramebufferSize{framebufferSize},
          m_showVertexMarkers{true}, m_showAbout{false}, m_inPinnedVertexLassoMode{false}, m_stepLength{0.0001f}, m_currentIntegrator{0}, m_currentSystem{0}
    {
        Utility::Resource rs("clothsim-data");
        m_licenceNotice = rs.get("LICENSE_NOTICE.txt");

        GL::Context::current().resetState(GL::Context::State::EnterExternal);

        m_imgui = ImGuiIntegration::Context{Vector2{windowSize} / scaling, windowSize, framebufferSize};

        GL::Context::current().resetState(GL::Context::State::ExitExternal);

        draw();
    }

    void UI::setIntegratorCallback(std::function<void(std::function<void(System &, const Float)>)> f)
    {
        m_setIntegratorCallback = f;
        (*m_setIntegratorCallback)(forwardEulerStep);
    }

    void UI::setSystemCallback(std::function<void(const std::size_t)> f)
    {
        m_setSystemCallback = f;
        (*m_setSystemCallback)(m_currentSystem);
    }

    void UI::setStepLengthCallback(std::function<void(const Float)> f)
    {
        m_stepLengthCallback = f;
        (*m_stepLengthCallback)(m_stepLength);
    }

    void UI::setStepsPerFrameCallback(std::function<void(const UnsignedInt)> f)
    {
        m_stepsPerFrameCallback = f;
        (*m_stepsPerFrameCallback)(m_stepsPerFrame);
    }

    void UI::setVertexMarkerVisibilityCallback(std::function<void(const bool)> f)
    {
        m_vertexMarkersVisibilityCallback = f;
        (*m_vertexMarkersVisibilityCallback)(m_showVertexMarkers);
    }

    void UI::setResetCallback(std::function<void(void)> f)
    {
        m_resetCallback = f;
    }

    void UI::setClearPinnedCallback(std::function<void(void)> f)
    {
        m_clearPinnedCallback = f;
    }

    void UI::setViewportClickCallback(std::function<void(const Vector2i)> f)
    {
        m_viewportClickCallback = f;
    }

    void UI::setLassoCallback(std::function<void(const UI::Lasso &lasso)> f)
    {
        m_lassoCallback = f;
    }

    void UI::setRotateCameraCallback(std::function<void(const Vector2i)> f)
    {
        m_rotateCameraCallback = f;
    }

    void UI::setZoomCameraCallback(std::function<void(const Float)> f)
    {
        m_zoomCameraCallback = f;
    }

    void UI::setSizeCallback(std::function<void(const Vector2ui)> f)
    {
        m_sizeCallback = f;
        (*m_sizeCallback)(Vector2ui{m_currentSize});
    }

    void UI::resize(const Vector2i windowSize, const Vector2 scaling, const Vector2i framebufferSize)
    {
        m_imgui.relayout(Vector2{windowSize} / scaling, windowSize, framebufferSize);
        m_currentWindowSize = windowSize;
        m_currentFramebufferSize = framebufferSize;
    }

    void UI::draw()
    {
        GL::Context::current().resetState(GL::Context::State::EnterExternal);

        GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
        GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                       GL::Renderer::BlendFunction::OneMinusSourceAlpha);

        GL::Renderer::enable(GL::Renderer::Feature::Blending);
        GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
        GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);

        m_imgui.newFrame();
        drawOptions();
        m_imgui.drawFrame();

        GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        GL::Renderer::disable(GL::Renderer::Feature::Blending);

        GL::Context::current().resetState(GL::Context::State::ExitExternal);

        drawLasso();
    }

    void UI::drawOptions()
    {
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("%.3f ms/frame (%.1f FPS)",
                    1000.0 / Double(ImGui::GetIO().Framerate), Double(ImGui::GetIO().Framerate));

        if (ImGui::Button(m_showVertexMarkers ? "Markers on" : "Markers off", ImVec2(110, 20)))
        {
            m_showVertexMarkers = !m_showVertexMarkers;
            (*m_vertexMarkersVisibilityCallback)(m_showVertexMarkers);
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset", ImVec2(110, 20)))
        {
            (*m_resetCallback)();
        }

        if (ImGui::SliderFloat("Step lenght", &m_stepLength, 0.00001f, 0.05f))
        {
            (*m_stepLengthCallback)(m_stepLength);
        }

        if (ImGui::SliderInt("Steps per draw", reinterpret_cast<int *>(&m_stepsPerFrame), 1, 1000))
        {
            (*m_stepsPerFrameCallback)(m_stepsPerFrame);
        }

        if (m_sizeCallback)
        {
            bool updated{false};

            if (ImGui::SliderInt("Cloth size x", &m_currentSize.x(), 1, 40))
            {
                updated = true;
            }

            if (ImGui::SliderInt("Cloth size y", &m_currentSize.y(), 1, 40))
            {
                updated = true;
            }

            if (updated)
            {
                (*m_sizeCallback)(Vector2ui{m_currentSize});
            }
        }

        if (drawCombo("Integrator", m_integrators, m_currentIntegrator))
        {
            switch (m_currentIntegrator)
            {
            case 0:
                (*m_setIntegratorCallback)(forwardEulerStep);
                break;
            case 1:
                (*m_setIntegratorCallback)(rk4Step);
                break;
            case 2:
                (*m_setIntegratorCallback)(backwardEulerStep);
                break;
            case 3:
                (*m_setIntegratorCallback)(backwardMidpointStep);
                break;
            }
        }

        if (drawCombo("System", m_systems, m_currentSystem))
        {
            (*m_setSystemCallback)(m_currentSystem);
        }

        if (ImGui::Button("Clear pinned", ImVec2(110, 20)))
        {
            (*m_clearPinnedCallback)();
        }

        ImGui::SameLine();
        if (ImGui::Button(m_inPinnedVertexLassoMode ? "Lasso on" : "Lasso off", ImVec2(110, 20)))
        {
            m_inPinnedVertexLassoMode = !m_inPinnedVertexLassoMode;
        }

        if (ImGui::Button("About", ImVec2(110, 20)))
            m_showAbout = !m_showAbout;

        if (m_showAbout)
        {
            ImGui::SetNextWindowPos(ImVec2(m_currentWindowSize.x() * 0.3f, m_currentWindowSize.y() * 0.3f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(550, 400), ImGuiCond_FirstUseEver);
            ImGui::Begin("Licenses", &m_showAbout, ImGuiWindowFlags_AlwaysVerticalScrollbar);

            ImGui::TextUnformatted(&(*m_licenceNotice.begin()), &(*m_licenceNotice.end()));
            ImGui::End();
        }

        ImGui::End();
    } // namespace clothsim

    void UI::drawLasso()
    {
        GL::Buffer vertices;
        vertices.setData(m_currentLasso.screenCoord, GL::BufferUsage::StaticDraw);

        GL::Mesh mesh;
        mesh.setPrimitive(GL::MeshPrimitive::Points)
            .addVertexBuffer(vertices, 0, Shaders::Flat2D::Position{})
            .setCount(m_currentLasso.screenCoord.size());

        Shaders::Flat2D shader;
        shader.setColor(0x2f83cc_rgbf)
            .setTransformationProjectionMatrix(Matrix3());

        shader.draw(mesh);
    }

    bool UI::wantsTextInput()
    {
        return ImGui::GetIO().WantTextInput;
    }

    bool UI::handleKeyPressEvent(Platform::Application::KeyEvent &event)
    {
        return m_imgui.handleKeyPressEvent(event);
    }

    bool UI::handleKeyReleaseEvent(Platform::Application::KeyEvent &event)
    {
        return m_imgui.handleKeyReleaseEvent(event);
    }

    bool UI::handleMousePressEvent(Platform::Application::MouseEvent &event)
    {
        if (!m_imgui.handleMousePressEvent(event))
        {
            if (m_inPinnedVertexLassoMode)
            {
                m_lassoPreviousPosition = event.position();
                event.setAccepted();
                return true;
            }
            else if (event.button() == Platform::Application::MouseEvent::Button::Left)
            {
                (*m_viewportClickCallback)(event.position());
                event.setAccepted();
                return true;
            }
        }

        return false;
    }

    bool UI::handleMouseReleaseEvent(Platform::Application::MouseEvent &event)
    {
        bool accept{false};

        if (m_imgui.handleMouseReleaseEvent(event))
        {
            accept = true;
        }
        else if (m_inPinnedVertexLassoMode && m_currentLasso.pixels.size() != 0)
        {
            m_inPinnedVertexLassoMode = false;

            (*m_lassoCallback)(m_currentLasso);
            m_currentLasso.clear();
            event.setAccepted();

            accept = true;
        }

        event.setAccepted(accept);
        return accept;
    }

    std::vector<Vector2> UI::toScreenCoordinates(const std::vector<Vector2i> &pixels)
    {
        std::vector<Vector2> output;

        std::transform(pixels.begin(), pixels.end(), std::back_inserter(output),
                       [=, this](const Vector2i p) -> Vector2 {
                           return {static_cast<Float>(p.x()) * 2.0f / static_cast<Float>(m_currentWindowSize.x()) - 1.f,
                                   static_cast<Float>(m_currentWindowSize.y() - p.y()) * 2.0f / static_cast<Float>(m_currentWindowSize.y()) - 1.f};
                       });

        return output;
    }

    bool UI::handleMouseMoveEvent(Platform::Application::MouseMoveEvent &event)
    {
        bool accept{false};

        if (m_imgui.handleMouseMoveEvent(event))
        {
            accept = true;
        }
        else if (m_inPinnedVertexLassoMode && event.relativePosition() != Vector2i{0, 0} && event.buttons() & Platform::Application::MouseMoveEvent::Button::Left)
        {
            const auto pixels = bresenham(m_lassoPreviousPosition, event.position());

            const auto screenCoord = toScreenCoordinates(pixels);
            m_currentLasso.screenCoord.insert(m_currentLasso.screenCoord.end(), screenCoord.begin(), screenCoord.end());
            m_currentLasso.pixels.insert(m_currentLasso.pixels.end(), pixels.begin(), pixels.end());

            m_lassoPreviousPosition = event.position();

            accept = true;
        }
        else if (event.buttons() & Platform::Application::MouseMoveEvent::Button::Left)
        {
            (*m_rotateCameraCallback)(event.relativePosition());
            accept = true;
        }

        event.setAccepted(accept);
        return accept;
    }

    bool UI::handleMouseScrollEvent(Platform::Application::MouseScrollEvent &event)
    {
        if (!m_imgui.handleMouseScrollEvent(event))
        {
            if (event.offset().y() != 0)
            {
                (*m_zoomCameraCallback)(event.offset().y());
                event.setAccepted();
                return true;
            }
        }

        return false;
    }

    bool UI::handleTextInputEvent(Platform::Application::TextInputEvent &event)
    {
        return m_imgui.handleTextInputEvent(event);
    }

} // namespace clothsim