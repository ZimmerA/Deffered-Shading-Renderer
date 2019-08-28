#ifndef SSAOPASS_H
#define SSAOPASS_H

#include "managers/shaderManager.h"
#include "managers/textureManager.h"
#include "renderPass.h"
#include "scene.h"

class SSAOPass : public RenderPass {
public:
  SSAOPass(std::shared_ptr<FrameBuffer> frameBuffer,
           std::shared_ptr<ShaderManager> &shaderManager,
           std::shared_ptr<TextureManager> textureManager);
  void render(const RenderData &renderData, std::shared_ptr<Scene> &scene);

  float bias = 2.0f;
  float radius = 40.0f;

private:
  void createSSAOKernel();

  std::shared_ptr<ShaderProgram> ssaoProgram;
  std::shared_ptr<TextureManager> textureManager;
  std::vector<glm::vec3> ssaoKernel;
};

#endif // SSAOPASS_H