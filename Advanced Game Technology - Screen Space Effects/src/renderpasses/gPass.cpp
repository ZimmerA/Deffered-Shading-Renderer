#include <iostream>

#include "gPass.h"
#include "glTimerQuery.h"
#include "glm/glm/gtc/matrix_transform.hpp"

GPass::GPass(std::shared_ptr<FrameBuffer> frameBuffer,
             std::shared_ptr<ShaderManager> &shaderManager)
    : RenderPass(frameBuffer) {
  gProgram =
      shaderManager->create("gProgram", "res/shaders/deffered/gShader.vs",
                            "res/shaders/deffered/gShader.fs");
  gProgram->use();
  gProgram->setInt("uAlbedoMap", 0);
  gProgram->setInt("uMetalRoughnessMap", 1);
  gProgram->setInt("uNormalMap", 2);
  gProgram->setInt("uEmmisiveMap", 3);
  gProgram->setInt("uOcclusionMap", 4);
}

void GPass::render(const RenderData &renderData,
                   std::shared_ptr<Scene> &scene) {
  GLTimerQuery timer(result);
  RenderPass::begin();

  glDepthMask(GL_FALSE);
  glColorMask(1, 1, 1, 1);
  glDepthFunc(GL_LEQUAL);

  gProgram->use();
  gProgram->setMat4("uMV", renderData.mv);
  gProgram->setMat4("uMVP", renderData.mvp);
  gProgram->setMat4("uModel", renderData.model);

  for (auto &mesh : scene->opaqueMeshes) {
    mesh.material->prepare();
    mesh.vao.draw();
  }

  glDepthMask(GL_TRUE);
}