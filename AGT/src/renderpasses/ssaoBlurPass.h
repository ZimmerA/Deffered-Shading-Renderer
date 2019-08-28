#ifndef SSAO_BLURPASS_H
#define SSAO_BLURPASS_H

#include <memory>

#include "managers/shaderManager.h"
#include "managers/textureManager.h"
#include "renderPass.h"
#include "scene.h"

class SSAOBlurPass : public RenderPass {
public:
  SSAOBlurPass(std::shared_ptr<FrameBuffer> frameBuffer,
               std::shared_ptr<ShaderManager> &shaderManager,
               std::shared_ptr<TextureManager> textureManager);
  void render(const RenderData &renderData, std::shared_ptr<Scene> &scene);

  enum BlurMode {
	  GAUSS_4x4,
	  CROSS_BILATERAL_8x8,
	  NONE
  };

  BlurMode blurMode = CROSS_BILATERAL_8x8;

private:
  std::shared_ptr<ShaderProgram> ssaoBlurProgram;
  std::shared_ptr<TextureManager> textureManager;

};

#endif // SSAO_BLURPASS_H