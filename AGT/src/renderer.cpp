#include <iostream>

#include "glm/glm/gtc/matrix_transform.hpp"
#include "loaders/gltfLoader/gltfLoader.h"
#include "renderer.h"
#include "vertexBuffer.h"

Renderer::Renderer(std::shared_ptr<TextureManager> textureManager,
                   std::shared_ptr<MaterialManager> materialManager,
                   std::shared_ptr<ShaderManager> shaderManager)
    : textureManager{textureManager}, materialManager{materialManager},
      shaderManager{shaderManager} {
  renderData.width = SCREEN_W;
  renderData.height = SCREEN_H;
  setupFrameBuffers();
  createRenderPasses();
  createScreenQuad();
  setupScene();
}

// Set up the Framebuffer for the deferred shading
void Renderer::setupFrameBuffers() {
  /***** GBUFFER *****/
  // TODO: Increase performance + memory usage(rethink texture precision,
  // compute position from depth)
  // r,g,b = Position
  std::shared_ptr<Texture> gPosition = textureManager->createEmpty("gPosition");
  gPosition->texImage(0, GL_RGB32F, renderData.width, renderData.height, 0,
                      GL_RGB, GL_FLOAT, nullptr);
  gPosition->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  gPosition->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  gPosition->texParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  gPosition->texParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // r,g,b = Normal a = Occlusion
  std::shared_ptr<Texture> gNormalOccl =
      textureManager->createEmpty("gNormalOccl");
  gNormalOccl->texImage(0, GL_RGBA32F, renderData.width, renderData.height, 0,
                        GL_RGBA, GL_FLOAT, nullptr);
  gNormalOccl->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  gNormalOccl->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // r,g, b = Albedo, a = Metallness
  std::shared_ptr<Texture> gAlbedoMetal =
      textureManager->createEmpty("gAlbedoMetal");
  gAlbedoMetal->texImage(0, GL_RGBA, renderData.width, renderData.height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  gAlbedoMetal->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  gAlbedoMetal->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // r,g, b = emmisive, a = Roughness
  std::shared_ptr<Texture> gEmmisiveRoughness =
      textureManager->createEmpty("gEmmisiveRoughness");
  gEmmisiveRoughness->texImage(0, GL_RGBA16F, renderData.width,
                               renderData.height, 0, GL_RGBA, GL_FLOAT,
                               nullptr);
  gEmmisiveRoughness->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  gEmmisiveRoughness->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  gBuffer = std::make_shared<FrameBuffer>();
  gBuffer->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition->handle,
                         0);
  gBuffer->attachTexture(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormalOccl->handle, 0);
  gBuffer->attachTexture(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gAlbedoMetal->handle, 0);
  gBuffer->attachTexture(GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D,
                         gEmmisiveRoughness->handle, 0);

  std::vector<unsigned int> gbufferAttachments = {
      GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
      GL_COLOR_ATTACHMENT3};
  gBuffer->setDrawBuffers(4, &gbufferAttachments.at(0));
  gDepthBuffer.renderBufferStorage(GL_DEPTH_COMPONENT, renderData.width,
                                   renderData.height);
  gBuffer->attachRenderBuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              gDepthBuffer.handle);

  /***** SHADOW DEPTH MAP BUFFER *****/
  std::shared_ptr<Texture> depthMap = textureManager->createEmpty("depthMap");
  depthMap->texImage(0, GL_DEPTH_COMPONENT, 2048, 1080, 0, GL_DEPTH_COMPONENT,
                     GL_FLOAT, nullptr);
  depthMap->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  depthMap->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  depthMap->texParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  depthMap->texParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
  depthMap->texParameteri(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
  depthMap->texParameteri(GL_TEXTURE_COMPARE_FUNC, GL_GREATER);
  depthMapBuffer = std::make_shared<FrameBuffer>();
  depthMapBuffer->attachTexture(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                                depthMap->handle, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  /***** SSAO BUFFER *****/
  std::shared_ptr<Texture> ssaoColorTexture =
      textureManager->createEmpty("ssaoColor");
  ssaoColorTexture->texImage(0, GL_RED, renderData.width, renderData.height, 0,
                             GL_RGB, GL_FLOAT, nullptr);
  ssaoColorTexture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  ssaoColorTexture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  ssaoColorTexture->texParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  ssaoColorTexture->texParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  ssaoBuffer = std::make_shared<FrameBuffer>();
  ssaoBuffer->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            ssaoColorTexture->handle, 0);

  /***** SSAO BLUR BUFFER *****/
  std::shared_ptr<Texture> ssaoBlurTexture1 =
      textureManager->createEmpty("ssaoBlur1");
  ssaoBlurTexture1->texImage(0, GL_RED, renderData.width, renderData.height, 0,
                             GL_RGB, GL_FLOAT, nullptr);
  ssaoBlurTexture1->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  ssaoBlurTexture1->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  ssaoBlurTexture1->texParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  ssaoBlurTexture1->texParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  std::shared_ptr<Texture> ssaoBlurTexture2 =
      textureManager->createEmpty("ssaoBlur2");
  ssaoBlurTexture2->texImage(0, GL_RED, renderData.width, renderData.height, 0,
                             GL_RGB, GL_FLOAT, nullptr);
  ssaoBlurTexture2->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  ssaoBlurTexture2->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  ssaoBlurTexture2->texParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  ssaoBlurTexture2->texParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  ssaoBlurBuffer = std::make_shared<FrameBuffer>();
  ssaoBlurBuffer->attachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                ssaoBlurTexture1->handle, 0);
  ssaoBlurBuffer->attachTexture(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                                ssaoBlurTexture2->handle, 0);

  std::vector<unsigned int> ssaoBufferAttachments = {GL_COLOR_ATTACHMENT0};
  ssaoBlurBuffer->setDrawBuffers(1, &ssaoBufferAttachments.at(0));
}

void Renderer::createRenderPasses() {
  gZPrePass = std::make_shared<GZPrePass>(gBuffer, shaderManager);
  gPass = std::make_shared<GPass>(gBuffer, shaderManager);
  gAlphaCutoutPass = std::make_shared<GAlphaCutoutPass>(gBuffer, shaderManager);
  depthMapPass = std::make_shared<DepthMapPass>(depthMapBuffer, shaderManager);
  ssaoPass =
      std::make_shared<SSAOPass>(ssaoBuffer, shaderManager, textureManager);
  ssaoBlurPass = std::make_shared<SSAOBlurPass>(ssaoBlurBuffer, shaderManager,
                                                textureManager);
  directionalLightPass = std::make_shared<DirectionalLightPass>(
      nullptr, shaderManager, textureManager);
}

void Renderer::createScreenQuad() {
  std::vector<Vertex> vertices;
  Vertex vertex;
  vertex.position = glm::vec3(-1.0f, 1.0f, 0.0f); // Top left
  vertex.uv = glm::vec2(0.0f, 1.0f);
  vertices.push_back(vertex);
  vertex.position = glm::vec3(-1.0f, -1.0f, 0.0f); // Bottom left
  vertex.uv = glm::vec2(0.0f, 0.0f);
  vertices.push_back(vertex);
  vertex.position = glm::vec3(1.0f, -1.0f, 0.0f); // Bottom right
  vertex.uv = glm::vec2(1.0f, 0.0f);
  vertices.push_back(vertex);
  vertex.position = glm::vec3(1.0f, 1.0f, 0.0f); // Top right
  vertex.uv = glm::vec2(1.0f, 1.0f);
  vertices.push_back(vertex);

  VertexElementDescriptorVec descriptorVec;
  descriptorVec.push_back({POSITION, ET_FLOAT3});
  descriptorVec.push_back({TEXCOORD, ET_FLOAT2});

  VertexBuffer quadVBO(4, sizeof(Vertex), descriptorVec);
  quadVBO.buffer(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
                 &vertices.at(0), GL_STATIC_DRAW);

  renderData.screenQuad =
      std::make_shared<VertexArray>(std::move(quadVBO), GL_TRIANGLE_FAN);
}

void Renderer::setupScene() {
  // Setup RenderData
  renderData.model = glm::mat4(1);
  renderData.view = camera.get_view_matrix();
  renderData.proj = glm::perspective(70.0f,
                                     static_cast<float>(renderData.width) /
                                         static_cast<float>(renderData.height),
                                     1.0f, 10000.0f);

  const float shadowMapNear = -1550.0f, shadowMapFar = 50.0f;
  renderData.lightProj = glm::ortho(-2000.0f, 2000.0f, -2000.0f, 2000.0f,
                                    shadowMapNear, shadowMapFar);

  // Setup Scene
  scene = std::make_shared<Scene>();

  // Load Sponza Model
  GltfLoader loader(textureManager, materialManager, shaderManager);
  Model sponzaModel = loader.load("res/Models/Sponza/Sponza.gltf");
  for (auto &mesh : sponzaModel.meshes) {
    if (mesh.material->mode == M_OPAQUE) {
      scene->opaqueMeshes.push_back(std::move(mesh));
    } else if (mesh.material->mode == M_MASK) {
      scene->alphaCutoutMeshes.push_back(std::move(mesh));
    }
  }
}

void Renderer::updateRenderData() {
  renderData.view = camera.get_view_matrix();
  renderData.mv = renderData.view * renderData.model;
  renderData.mvp = renderData.proj * renderData.view * renderData.model;
  const glm::mat4 lightView =
      glm::lookAt(-scene->lightDirection, glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));
  renderData.lightViewProj = renderData.lightProj * lightView;
  renderData.camPos = camera.position;
}

void Renderer::render() {
  updateRenderData();
  glEnable(GL_DEPTH_TEST);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glViewport(0, 0, 2048, 1080);
  depthMapPass->render(renderData, scene);
  glViewport(0, 0, renderData.width, renderData.height);
  gZPrePass->render(renderData, scene);
  gPass->render(renderData, scene);
  gAlphaCutoutPass->render(renderData, scene);
  ssaoPass->render(renderData, scene);
  ssaoBlurPass->render(renderData, scene);
  directionalLightPass->render(renderData, scene);
}

void Renderer::key_callback(GLFWwindow *window, int key, int scancode,
                            int action, int mods) noexcept {
  if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
    altPressed = true;
  } else if (key == GLFW_KEY_LEFT_ALT && action == GLFW_RELEASE) {
    altPressed = false;
  }
}

void Renderer::mouse_button_callback(GLFWwindow *window, int button, int action,
                                     int mods) noexcept {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    mode = 1;
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    mode = 2;
  } else {
    mode = 0;
  }
}

void Renderer::cursor_position_callback(GLFWwindow *window, double xPos,
                                        double yPos) {
  const double deltaX = xPos - lastX;
  const double deltaY = yPos - lastY;

  lastX = xPos;
  lastY = yPos;
  if (altPressed && mode == 1) {
    camera.handleMouseMove(deltaX, deltaY);
  } else if (altPressed && mode == 2) {
    camera.movePivotPoint(deltaX, deltaY);
  }
}

void Renderer::scroll_callback(GLFWwindow *window, double xOffset,
                               double yOffset) {
  camera.handleMouseWheel(yOffset);
}
