#include "App.h"

#include <Corrade/Containers/StridedArrayView.h>

#include <Magnum/Image.h>

#include <Magnum/GL/Context.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/Version.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>

#include "Integrators.h"
#include "Util.h"

namespace clothsim {

using namespace Magnum::Math::Literals;
using namespace Magnum;

App::App(const Arguments &arguments)
    : Platform::Application{arguments,
                            Configuration{}
                                .setTitle("clothsim")
                                .setWindowFlags(
                                    Configuration::WindowFlag::Resizable)},
      m_framebuffer{GL::defaultFramebuffer.viewport()}, m_ui{windowSize(),
                                                             framebufferSize(),
                                                             dpiScaling(),
                                                             *this} {
#ifndef MAGNUM_TARGET_GLES
  MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL430);
#else
  MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GLES300);
#endif

  const auto vpSize{GL::defaultFramebuffer.viewport().size()};

  m_color.setBaseLevel(0)
      .setMaxLevel(0)
      .setImage(0, GL::TextureFormat::RGBA8,
                ImageView2D{PixelFormat::RGBA8Unorm, vpSize})
      .setMagnificationFilter(GL::SamplerFilter::Nearest)
      .setMinificationFilter(GL::SamplerFilter::Nearest);

  m_particleId.setStorage(GL::RenderbufferFormat::R32I, vpSize);
  m_depth.setStorage(GL::RenderbufferFormat::DepthComponent24, vpSize);

  m_framebuffer
      .attachTexture(
          GL::Framebuffer::ColorAttachment{m_phongShader.ColorOutput}, m_color,
          0)
      .attachRenderbuffer(
          GL::Framebuffer::ColorAttachment{m_phongShader.ObjectIdOutput},
          m_particleId)
      .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, m_depth)
      .mapForDraw(
          {{PhongIdShader::ColorOutput,
            GL::Framebuffer::ColorAttachment{m_phongShader.ColorOutput}},
           {PhongIdShader::ObjectIdOutput,
            GL::Framebuffer::ColorAttachment{m_phongShader.ObjectIdOutput}}});

  CORRADE_INTERNAL_ASSERT(
      m_framebuffer.checkStatus(GL::FramebufferTarget::Draw) ==
      GL::Framebuffer::Status::Complete);

  // Configure camera
  m_cameraObject = std::make_unique<Object3D>(&m_scene);
  m_cameraObject->translate(Vector3::zAxis(8.0f))
      .rotate(Math::Rad(Constants::pi() * 0.5f), Vector3{1.f, 0.f, 0.f});

  const auto aspectRatio = Vector2{vpSize}.aspectRatio();
  m_camera = std::make_unique<SceneGraph::Camera3D>(*m_cameraObject);
  m_camera->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::NotPreserved)
      .setProjectionMatrix(Matrix4::perspectiveProjection(
          35.0_degf, aspectRatio, 0.001f, 100.0f))
      .setViewport(vpSize);
}

void App::setIntegrator(
    std::function<void(System &system, const Float dt)> integrator) {
  m_integrator = integrator;
}

void App::setSystem(const std::size_t i) {
  switch (i) {
  case 0:
    m_system = std::make_unique<Oscillator>(m_phongShader, m_vertexShader,
                                            m_scene, m_drawableGroup);
    break;
  case 1:
    m_system = std::make_unique<Planet>(m_phongShader, m_vertexShader, m_scene,
                                        m_drawableGroup);
    break;
  case 2:
    m_system = std::make_unique<Cloth>(m_phongShader, m_vertexShader, m_scene,
                                       m_drawableGroup);
    break;
  }
}

void App::viewportEvent(ViewportEvent &event) {
  resizeFramebuffers(event.framebufferSize());
  resizeRenderbuffers(event.framebufferSize());
  resizeTextures(event.framebufferSize());
  resizeCamera(event.framebufferSize());

  m_ui.resize(event.windowSize(), event.dpiScaling(), event.framebufferSize());

  redraw();
}

void App::resizeCamera(const Vector2i &size) {
  m_camera
      ->setProjectionMatrix(Matrix4::perspectiveProjection(
          35.0_degf, Vector2{size}.aspectRatio(), 0.001f, 100.0f))
      .setViewport(size);
}

void App::resizeTextures(const Vector2i &size) {
  m_color.setImage(
      0, GL::TextureFormat::RGBA8,
      ImageView2D{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, size});
}

void App::resizeRenderbuffers(const Vector2i &size) {
  m_particleId.setStorage(GL::RenderbufferFormat::R32I, size);
  m_depth.setStorage(GL::RenderbufferFormat::DepthComponent24, size);
}

void App::resizeFramebuffers(const Vector2i &size) {
  GL::defaultFramebuffer.setViewport({{}, size});
  m_framebuffer.setViewport({{}, size});
}

void App::drawEvent() {
  for (UnsignedInt i = 0; i < m_stepsPerFrame; ++i) {
    m_integrator(*m_system, m_stepLength);
  }

  if (m_ui.wantsTextInput() && !isTextInputActive())
    startTextInput();
  else if (!m_ui.wantsTextInput() && isTextInputActive())
    stopTextInput();

  m_framebuffer.clearColor(m_phongShader.ColorOutput, Vector4{0.0f})
      .clearColor(m_phongShader.ObjectIdOutput, Vector4i{-1})
      .clearDepth(1.0f)
      .bind();

  m_camera->draw(m_drawableGroup);
  m_ui.draw();

  GL::defaultFramebuffer
      .clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth)
      .bind();

  m_framebuffer.mapForRead(
      GL::Framebuffer::ColorAttachment{m_phongShader.ColorOutput});
  GL::AbstractFramebuffer::blit(m_framebuffer, GL::defaultFramebuffer,
                                {{}, m_framebuffer.viewport().size()},
                                GL::FramebufferBlit::Color);

  swapBuffers();
  redraw();
}

void App::resetSimulation() {
  if (m_system)
    m_system->reset();
}

void App::mouseScrollEvent(MouseScrollEvent &event) {
  if (m_ui.handleMouseScrollEvent(event))
    redraw();
}

void App::zoomCamera(const Float offset) {
  // Distance to origin
  const Float distance = m_cameraObject->transformation().translation().z();

  // Move 15% of the distance back or forward
  m_cameraObject->translate(
      -m_camera->cameraMatrix().inverted().backward() *
      (distance * (1.0f - (offset > 0 ? 1 / 0.85f : 0.85f))));
}

void App::mousePressEvent(MouseEvent &event) {
  if (m_ui.handleMousePressEvent(event))
    redraw();
}

void App::handleViewportClick(const Vector2i position) {
  m_framebuffer.mapForRead(
      GL::Framebuffer::ColorAttachment{m_phongShader.ObjectIdOutput});

  const Vector2i fbPosition{position.x(), m_framebuffer.viewport().sizeY() -
                                              position.y() - 1};
  const Image2D data = m_framebuffer.read(
      Range2Di::fromSize(fbPosition, {1, 1}), PixelFormat::R32I);
  const Int selectedVertexId = data.pixels<Int>()[0][0];

  if (selectedVertexId >= 0) {
    m_system->togglePinnedParticle(static_cast<UnsignedInt>(selectedVertexId));
    Debug{} << "Toggled vertex number " << selectedVertexId;
  }
}

void App::pinVertices(const UI::Lasso &lasso) {
  if (lasso.pixels.size() == 0)
    return;

  const auto [min, max] = computeAABB(lasso.pixels);

  m_framebuffer.mapForRead(
      GL::Framebuffer::ColorAttachment{m_phongShader.ObjectIdOutput});
  const Image2D data = m_framebuffer.read(
      Range2Di({min.x(), m_framebuffer.viewport().sizeY() - max.y() - 1},
               {max.x(), m_framebuffer.viewport().sizeY() - min.y() - 1}),
      PixelFormat::R32I);

  const auto pixels = data.pixels<Int>();
  std::set<UnsignedInt> seenIndices;

  const Vector2ui size{max - min};
  for (std::size_t y = 0; y < size.y(); ++y) {
    for (std::size_t x = 0; x < size.x(); ++x) {
      const Int index = pixels[y][x];

      // -1 means no vertex
      if (index > -1) {
        seenIndices.insert(static_cast<UnsignedInt>(index));
      }
    }
  }

  for (const auto index : seenIndices)
    m_system->setPinnedParticle(index, true);
}

void App::mouseMoveEvent(MouseMoveEvent &event) {
  if (m_ui.handleMouseMoveEvent(event))
    redraw();
}

void App::rotateCamera(const Vector2i offset) {
  const Float cameraMovementSpeed = 0.005f;
  m_cameraTrackballAngles[0] +=
      static_cast<Float>(offset[0]) * cameraMovementSpeed;

  // Restrict the up-down angle
  if ((m_cameraTrackballAngles[1] < Constants::pi() * 0.5f && offset[1] > 0) ||
      (m_cameraTrackballAngles[1] > -Constants::pi() * 0.5f && offset[1] < 0)) {

    m_cameraTrackballAngles[1] +=
        static_cast<Float>(offset[1]) * cameraMovementSpeed;
    m_cameraObject->rotate(
        Math::Rad(-static_cast<Float>(offset[1]) * cameraMovementSpeed),
        m_camera->cameraMatrix().inverted().right().normalized());
  }

  m_cameraObject->rotate(
      Math::Rad(-static_cast<Float>(offset[0]) * cameraMovementSpeed),
      Vector3(0.f, 0.f, 1.f));
}

void App::mouseReleaseEvent(MouseEvent &event) {
  if (m_ui.handleMouseReleaseEvent(event))
    redraw();
}

void App::textInputEvent(TextInputEvent &event) {
  if (m_ui.handleTextInputEvent(event))
    redraw();
}

void App::keyPressEvent(KeyEvent &event) {
  if (m_ui.handleKeyPressEvent(event))
    redraw();
}

void App::keyReleaseEvent(KeyEvent &event) {
  if (m_ui.handleKeyReleaseEvent(event))
    redraw();
}

void App::setVertexMarkersVisibility(bool show) {
  m_system->drawVertexMarkers(show);
}

const std::unique_ptr<System> &App::getSystem() { return m_system; }

void App::setStepLength(const Float stepLength) { m_stepLength = stepLength; }

void App::setStepsPerFrame(const UnsignedInt steps) { m_stepsPerFrame = steps; }
} // namespace clothsim

MAGNUM_APPLICATION_MAIN(clothsim::App)