#ifndef GPASS_H
#define GPASS_H

#include <memory>

#include "managers/shaderManager.h"
#include "model.h"
#include "renderPass.h"
#include "scene.h"
#include "shader.h"

class GPass : public RenderPass {
public:
  GPass(std::shared_ptr<FrameBuffer> frameBuffer,
        std::shared_ptr<ShaderManager> &shaderManager);
  void render(const RenderData &renderData, std::shared_ptr<Scene> &scene);

private:
  std::shared_ptr<ShaderProgram> gProgram;
};

#endif // GPASS_H