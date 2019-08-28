#include <random>

#include "ssaoPass.h"
#include "util.h"

SSAOPass::SSAOPass(std::shared_ptr<FrameBuffer> frameBuffer,
                   std::shared_ptr<ShaderManager> &shaderManager,
                   std::shared_ptr<TextureManager> textureManager)
    : RenderPass{frameBuffer}, textureManager{textureManager} {
  createSSAOKernel();
  ssaoProgram = shaderManager->create("directionalLightProgram",
                                      "res/Shaders/screenQuad.vs",
                                      "res/Shaders/ssao/ssao.fs");
  ssaoProgram->use();
  ssaoProgram->setInt("gPosition", 0);
  ssaoProgram->setInt("gNormalOccl", 1);
  ssaoProgram->setInt("uNoiseSampler", 2);
  ssaoProgram->setVec3Arr("uFilterKernel", ssaoKernel);
}

void SSAOPass::render(const RenderData &renderData,
                      std::shared_ptr<Scene> &scene) {
  RenderPass::begin();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ssaoProgram->use();
  ssaoProgram->setVec2("uNoiseScale", glm::vec2(renderData.width / 4.0,
                                                renderData.height / 4.0));
  ssaoProgram->setFloat("uBias", bias);
  ssaoProgram->setFloat("uRadius", radius);
  ssaoProgram->setMat4("uProjection", renderData.proj);

  glActiveTexture(GL_TEXTURE0);
  textureManager->getTextureByName("gPosition")->bind();
  glActiveTexture(GL_TEXTURE1);
  textureManager->getTextureByName("gNormalOccl")->bind();
  glActiveTexture(GL_TEXTURE2);
  textureManager->getTextureByName("ssaoNoiseTexture")->bind();

  renderData.screenQuad->draw();
}

void SSAOPass::createSSAOKernel() {
  // Creates a hemisphere kernel and pseudo random rotation vectors to be used
  // in the ssao shader
  const std::uniform_real_distribution<float> randomValues(0.0, 1.0);
  std::default_random_engine generator;

  const int KERNEL_SIZE = 16;
  for (unsigned int i = 0; i < KERNEL_SIZE; ++i) {
    glm::vec3 sample(randomValues(generator) * 2.0 - 1.0,
                     randomValues(generator) * 2.0 - 1.0,
                     randomValues(generator));
    sample = glm::normalize(sample);
    sample *= randomValues(generator);
    float scale = static_cast<float>(i) / KERNEL_SIZE;
    scale = lerp(0.1f, 1.0f, scale * scale);
    sample *= scale;
    ssaoKernel.push_back(sample);
  }

  std::vector<glm::vec3> ssaoNoise;
  for (unsigned int i = 0; i < 16; i++) {
    const glm::vec3 noise(randomValues(generator) * 2.0 - 1.0,
                          randomValues(generator) * 2.0 - 1.0, 0.0f);
    ssaoNoise.push_back(noise);
  }

  std::shared_ptr<Texture> noiseTexture =
      textureManager->createEmpty("ssaoNoiseTexture");
  noiseTexture->texImage(0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT,
                         &ssaoNoise.at(0));
  noiseTexture->texParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  noiseTexture->texParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  noiseTexture->texParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  noiseTexture->texParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
}
