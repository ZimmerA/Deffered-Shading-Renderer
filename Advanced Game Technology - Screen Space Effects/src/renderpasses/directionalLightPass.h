#ifndef DIRECTIONALLIGHTPASS_H
#define DIRECTIONALLIGHTPASS_H

#include "managers/shaderManager.h"
#include "managers/textureManager.h"
#include "renderPass.h"
#include "scene.h"
#include "shader.h"
#include "vertexArray.h"

class DirectionalLightPass : public RenderPass {
public:
  DirectionalLightPass(std::shared_ptr<FrameBuffer> frameBuffer,
                       std::shared_ptr<ShaderManager> &shaderManager,
                       std::shared_ptr<TextureManager> textureManager);
  void render(const RenderData &renderData, const std::shared_ptr<Scene> scene);

  bool debugSSAO;

private:
  std::shared_ptr<ShaderProgram> directionalPBRProgram;
  std::shared_ptr<TextureManager> textureManager;
};
#endif // DIRECTIONALLIGHTPASS_H
