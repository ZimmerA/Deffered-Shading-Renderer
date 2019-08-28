#ifndef SCENE_H
#define SCENE_H

#include "glm/glm/glm.hpp"
#include "model.h"
#include "vertexArray.h"

struct Scene {
  std::vector<Mesh> opaqueMeshes;
  std::vector<Mesh> alphaCutoutMeshes;
  glm::vec3 lightDirection = glm::vec3(0.0, -1.0, .1);
  glm::vec3 lightColor = glm::vec3(1, 1, 1);
  float toneMappingExposure = 15.0f;
};

struct RenderData {
  int width, height;
  glm::mat4 model{1};
  glm::mat4 view{1};
  glm::mat4 proj{1};
  glm::mat4 mv{1};
  glm::mat4 mvp{1};
  glm::vec3 camPos{0};
  std::shared_ptr<VertexArray> screenQuad;

  // Shadow mapping
  glm::mat4 lightViewProj;
  glm::mat4 lightProj;
  bool shadowOn = true;

  // SSAO
  bool ssaoOn = true;
};

#endif