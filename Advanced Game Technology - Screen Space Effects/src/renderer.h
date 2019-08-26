#ifndef RENDERER_H
#define RENDERER_H

constexpr auto SCREEN_W = 1920;
constexpr auto SCREEN_H = 1080;

#include "glad.h"
#include "GLFW/glfw3.h"
#include "frameBuffer.h"
#include "glm/glm.hpp"
#include "managers/materialManager.h"
#include "managers/shaderManager.h"
#include "managers/textureManager.h"
#include "material.h"
#include "model.h"
#include "orbitCamera.h"
#include "renderBuffer.h"
#include "renderpasses/depthMapPass.h"
#include "renderpasses/directionalLightPass.h"
#include "renderpasses/gAlphaCutoutPass.h"
#include "renderpasses/gPass.h"
#include "renderpasses/gZPrePass.h"
#include "renderpasses/ssaoBlurPass.h"
#include "renderpasses/ssaoPass.h"
#include "scene.h"
#include "shader.h"
#include "vertexArray.h"

struct State {
  glm::vec4 clearColor{0};
  GLenum DEPTH_TEST;
};

class Renderer {
public:
  Renderer(std::shared_ptr<TextureManager> textureManager,
           std::shared_ptr<MaterialManager> materialManager,
           std::shared_ptr<ShaderManager>);
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  void render();
  void key_callback(GLFWwindow *window, int key, int scancode, int action,
                    int mods) noexcept;
  void mouse_button_callback(GLFWwindow *window, int button, int action,
                             int mods) noexcept;
  void cursor_position_callback(GLFWwindow *window, double xPos, double yPos);
  void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);

  std::shared_ptr<Scene> scene;
  RenderData renderData;

  // Renderpasses
  std::shared_ptr<GZPrePass> gZPrePass;
  std::shared_ptr<GPass> gPass;
  std::shared_ptr<GAlphaCutoutPass> gAlphaCutoutPass;
  std::shared_ptr<DepthMapPass> depthMapPass;
  std::shared_ptr<SSAOPass> ssaoPass;
  std::shared_ptr<SSAOBlurPass> ssaoBlurPass;
  std::shared_ptr<DirectionalLightPass> directionalLightPass;

private:
  void setupFrameBuffers();
  void createRenderPasses();
  void createScreenQuad();
  void setupScene();
  void updateRenderData();

  // Managers
  std::shared_ptr<TextureManager> textureManager;
  std::shared_ptr<MaterialManager> materialManager;
  std::shared_ptr<ShaderManager> shaderManager;

  // Framebuffers
  std::shared_ptr<FrameBuffer> gBuffer;
  RenderBuffer gDepthBuffer;
  std::shared_ptr<FrameBuffer> depthMapBuffer;
  std::shared_ptr<FrameBuffer> ssaoBuffer;
  std::shared_ptr<FrameBuffer> ssaoBlurBuffer;

  // Controls
  OrbitCamera camera;
  bool altPressed{false};
  double lastX{0};
  double lastY{0};
  int mode{0};
};

#endif // RENDERER_H