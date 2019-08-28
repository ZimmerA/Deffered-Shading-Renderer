#include "gZPrePass.h"

GZPrePass::GZPrePass(std::shared_ptr<FrameBuffer> frameBuffer,
                     std::shared_ptr<ShaderManager> &shaderManager)
    : RenderPass(frameBuffer) {
  gPrePassProgram = shaderManager->create(
      "gPrePassProgram", "res/Shaders/deferred/gShaderPrePass.vs",
      "res/Shaders/deferred/gShaderPrePass.fs");
}

void GZPrePass::render(const RenderData &renderData,
                       std::shared_ptr<Scene> &scene) {
  RenderPass::begin();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);
  glColorMaski(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  gPrePassProgram->use();
  gPrePassProgram->setMat4("uMVP", renderData.mvp);

  for (auto &mesh : scene->opaqueMeshes) {
    mesh.vao.draw();
  }
}
