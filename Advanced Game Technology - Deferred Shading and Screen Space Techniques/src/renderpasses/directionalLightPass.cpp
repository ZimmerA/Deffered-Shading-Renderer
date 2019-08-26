#include <iostream>

#include "directionalLightPass.h"

DirectionalLightPass::DirectionalLightPass(
    std::shared_ptr<FrameBuffer> frameBuffer,
    std::shared_ptr<ShaderManager> &shaderManager,
    std::shared_ptr<TextureManager> textureManager)
    : RenderPass(frameBuffer), textureManager{textureManager} {
  directionalPBRProgram = shaderManager->create(
      "directionalLightProgram", "res/Shaders/screenQuad.vs",
      "res/Shaders/directionalLight.fs");
  directionalPBRProgram->use();
  directionalPBRProgram->setInt("gWorldPos", 0);
  directionalPBRProgram->setInt("gNormalOccl", 1);
  directionalPBRProgram->setInt("gAlbedoMetal", 2);
  directionalPBRProgram->setInt("gEmissiveRoughness", 3);
  directionalPBRProgram->setInt("uSSAOMap", 4);
  directionalPBRProgram->setInt("uShadowMap", 5);
}

void DirectionalLightPass::render(const RenderData &renderData,
                                  const std::shared_ptr<Scene> scene) {
  RenderPass::begin();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  const glm::vec4 dir(scene->lightDirection, 0);
  const glm::vec3 newDir = renderData.mv * dir;

  directionalPBRProgram->use();
  directionalPBRProgram->setVec3("uLightColor", scene->lightColor);
  directionalPBRProgram->setVec3("uLightDirection", newDir);
  directionalPBRProgram->setInt("uSSAOOn", renderData.ssaoOn);
  directionalPBRProgram->setInt("uShadowOn", renderData.shadowOn);
  directionalPBRProgram->setFloat("uExposure", scene->toneMappingExposure);
  directionalPBRProgram->setMat4("uLightViewProjMat", renderData.lightViewProj);
  directionalPBRProgram->setMat4("uViewMat", renderData.view);
  directionalPBRProgram->setInt("uDebugSSAO", debugSSAO);
  glActiveTexture(GL_TEXTURE0);
  textureManager->getTextureByName("gPosition")->bind();
  glActiveTexture(GL_TEXTURE1);
  textureManager->getTextureByName("gNormalOccl")->bind();
  glActiveTexture(GL_TEXTURE2);
  textureManager->getTextureByName("gAlbedoMetal")->bind();
  glActiveTexture(GL_TEXTURE3);
  textureManager->getTextureByName("gEmmisiveRoughness")->bind();
  glActiveTexture(GL_TEXTURE4);
  textureManager->getTextureByName("ssaoBlur1")->bind();
  glActiveTexture(GL_TEXTURE5);
  textureManager->getTextureByName("depthMap")->bind();
  renderData.screenQuad->draw();
}
