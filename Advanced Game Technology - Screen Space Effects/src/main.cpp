#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "managers/shaderManager.h"
#include "managers/textureManager.h"
#include "renderer.h"

int main() {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(
      SCREEN_W, SCREEN_H,
      "Advanced Game Technology - Deferred Shading And Screen Space Effects",
      nullptr, nullptr);

  glfwMakeContextCurrent(window);

  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    return -1;
  }
  glfwSwapInterval(0);
  std::shared_ptr<TextureManager> textureManager =
      std::make_shared<TextureManager>();

  std::shared_ptr<MaterialManager> materialManager =
      std::make_shared<MaterialManager>();

  std::shared_ptr<ShaderManager> shaderManager =
      std::make_shared<ShaderManager>();

  std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>(
      textureManager, materialManager, shaderManager);

  // Setup GLFW callbacks
  glfwSetWindowUserPointer(window, renderer.get());
  auto keyFunc = [](GLFWwindow *w, int a, int b, int c, int d) {
    static_cast<Renderer *>(glfwGetWindowUserPointer(w))
        ->key_callback(w, a, b, c, d);
  };
  auto mouseButtonFunc = [](GLFWwindow *w, int a, int b, int c) {
    static_cast<Renderer *>(glfwGetWindowUserPointer(w))
        ->mouse_button_callback(w, a, b, c);
  };
  auto cursorPosFunc = [](GLFWwindow *w, double x, double y) {
    static_cast<Renderer *>(glfwGetWindowUserPointer(w))
        ->cursor_position_callback(w, x, y);
  };
  auto mouseScrollFunc = [](GLFWwindow *w, double x, double y) {
    static_cast<Renderer *>(glfwGetWindowUserPointer(w))
        ->scroll_callback(w, x, y);
  };

  glfwSetKeyCallback(window, keyFunc);
  glfwSetMouseButtonCallback(window, mouseButtonFunc);
  glfwSetCursorPosCallback(window, cursorPosFunc);
  glfwSetScrollCallback(window, mouseScrollFunc);

  // Setup Dear ImGui context for debug ui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Main Rendering
    renderer->render();

    // Debug UI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Controller");
    if (ImGui::CollapsingHeader("SSAO")) {
      ImGui::Checkbox("Enable SSAO", &renderer->renderData.ssaoOn);
      ImGui::InputFloat("Bias", &renderer->ssaoPass->bias);
      ImGui::InputFloat("Radius", &renderer->ssaoPass->radius);
    }

    if (ImGui::CollapsingHeader("Directional Light and ShadowMapping")) {
      ImGui::Checkbox("Enable Shadowmapping", &renderer->renderData.shadowOn);
      ImGui::InputFloat3("Light Direction",
                         &renderer->scene->lightDirection[0]);
      ImGui::InputFloat("Tonemapping Exposure",
                        &renderer->scene->toneMappingExposure);
    }

    ImGui::Spacing();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Depthmap pass(Directional light shadow): %f",
                renderer->depthMapPass->result);
    ImGui::Text("Z-Prepass: %f", renderer->gZPrePass->result);
    ImGui::Text("Gpass: %f", renderer->gPass->result);
    ImGui::Text("GAlphaCutoutPass: %f", renderer->gAlphaCutoutPass->result);
    ImGui::Text("SSAO pass: %f ", renderer->ssaoPass->result);
    ImGui::Text("SSAO blur passes: %f ", renderer->ssaoBlurPass->result);
    ImGui::Text("light pass: %f ", renderer->directionalLightPass->result);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
