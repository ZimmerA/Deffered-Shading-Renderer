#include "gAlphaCutoutPass.h"
#include "glTimerQuery.h"

GAlphaCutoutPass::GAlphaCutoutPass(
    std::shared_ptr<FrameBuffer> frameBuffer,
    std::shared_ptr<ShaderManager> &shaderManager)
    : RenderPass(frameBuffer) {
  gAlphaCutoutProgram =
      shaderManager->create("gProgram", "res/shaders/deffered/gShader.vs",
                            "res/shaders/deffered/gShaderAlphaCutout.fs");
  gAlphaCutoutProgram->use();
  gAlphaCutoutProgram->setInt("uAlbedoMap", 0);
  gAlphaCutoutProgram->setInt("uMetalRoughnessMap", 1);
  gAlphaCutoutProgram->setInt("uNormalMap", 2);
  gAlphaCutoutProgram->setInt("uEmmisiveMap", 3);
  gAlphaCutoutProgram->setInt("uOcclusionMap", 4);
}

void GAlphaCutoutPass::render(const RenderData &renderData,
                              std::shared_ptr<Scene> &scene) {
  GLTimerQuery timer(result);
  RenderPass::begin();
  gAlphaCutoutProgram->use();
  gAlphaCutoutProgram->setMat4("uMV", renderData.mv);
  gAlphaCutoutProgram->setMat4("uMVP", renderData.mvp);
  gAlphaCutoutProgram->setMat4("uModel", renderData.model);

  for (auto &mesh : scene->alphaCutoutMeshes) {
    mesh.material->prepare();
    mesh.vao.draw();
  }
}
