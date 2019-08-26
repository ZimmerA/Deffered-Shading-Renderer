#include "ssaoBlurPass.h"
#include "glTimerQuery.h"

SSAOBlurPass::SSAOBlurPass(std::shared_ptr<FrameBuffer> frameBuffer,
                           std::shared_ptr<ShaderManager> &shaderManager,
                           std::shared_ptr<TextureManager> textureManager)
    : RenderPass{frameBuffer}, textureManager{textureManager} {

  ssaoBlurProgram =
      shaderManager->create("ssaoBlurProgram", "res/Shaders/screenQuad.vs",
                            "res/Shaders/ssao/ssaoBlur.fs");
  ssaoBlurProgram->use();
  ssaoBlurProgram->setInt("uSSAOMap", 0);
  ssaoBlurProgram->setInt("gPosition", 1);
}

void SSAOBlurPass::render(const RenderData &renderData,
                          std::shared_ptr<Scene> &scene) {
  GLTimerQuery timer(result);
  RenderPass::begin();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ssaoBlurProgram->use();

  if (blurMode == GAUSS_4x4) {
    ssaoBlurProgram->setInt("uFilterMode", 1);
    glActiveTexture(GL_TEXTURE0);
    textureManager->getTextureByName("ssaoColor")->bind();
    renderData.screenQuad->draw();
  } else if (blurMode == CROSS_BILATERAL_8x8) {
    ssaoBlurProgram->setInt("uFilterMode", 2);

    glActiveTexture(GL_TEXTURE1);
    textureManager->getTextureByName("gPosition")->bind();

    // x Blur
    ssaoBlurProgram->setInt("horizontal", true);
    glActiveTexture(GL_TEXTURE0);
    textureManager->getTextureByName("ssaoColor")->bind();
    std::vector<unsigned int> ssaoBufferAttachments = {GL_COLOR_ATTACHMENT1};
    frameBuffer->setDrawBuffers(1, &ssaoBufferAttachments[0]);
    renderData.screenQuad->draw();

    // y Blur
    glActiveTexture(GL_TEXTURE0);
    textureManager->getTextureByName("ssaoBlur2")->bind();
    ssaoBlurProgram->setInt("uHorizontal", false);
    ssaoBufferAttachments = {GL_COLOR_ATTACHMENT0};
    frameBuffer->setDrawBuffers(1, &ssaoBufferAttachments[0]);
    renderData.screenQuad->draw();

  } else if (blurMode == NONE) {
    ssaoBlurProgram->setInt("uFilterMode", 3);
    glActiveTexture(GL_TEXTURE0);
    textureManager->getTextureByName("ssaoColor")->bind();
    renderData.screenQuad->draw();
  }
}
