#include "depthMapPass.h"
#include "glm/glm/gtc/matrix_transform.hpp"

DepthMapPass::DepthMapPass(std::shared_ptr<FrameBuffer> frameBuffer,
                           std::shared_ptr<ShaderManager> &shaderManager)
    : RenderPass{frameBuffer} {
  depthMapProgram = shaderManager->create(
      "depthMapProgram", "res/Shaders/shadowMapping/depthMap.vs",
      "res/Shaders/shadowMapping/depthMap.fs");
}

void DepthMapPass::render(const RenderData &renderData,
                          std::shared_ptr<Scene> &scene) {
  RenderPass::begin();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  depthMapProgram->use();
  const glm::mat4 lightSpaceMatrix =
      renderData.lightViewProj * renderData.model;
  depthMapProgram->setMat4("uLightSpaceMatrix", lightSpaceMatrix);

  for (auto &mesh : scene->opaqueMeshes) {
    mesh.vao.draw();
  }
}
