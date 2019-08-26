#ifndef DEPTH_MAP_PASS_H
#define DEPTH_MAP_PASS_H

#include <memory>

#include "managers/shaderManager.h"
#include "managers/textureManager.h"
#include "renderPass.h"
#include "scene.h"

class DepthMapPass : public RenderPass {
public:
  DepthMapPass(std::shared_ptr<FrameBuffer> frameBuffer,
               std::shared_ptr<ShaderManager> &shaderManager);
  void render(const RenderData &renderData, std::shared_ptr<Scene> &scene);

private:
  std::shared_ptr<ShaderProgram> depthMapProgram;
  std::shared_ptr<TextureManager> textureManager;
};

#endif // DEPTH_MAP_PASS_H
