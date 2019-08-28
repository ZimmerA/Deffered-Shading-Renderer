#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <vector>

#include "material.h"
#include "tiny_gltf.h"
#include "vertexArray.h"
#include "vertexBuffer.h"

class Transform {
public:
  Transform() = default;
  Transform(glm::mat4 localTransform, glm::mat4 globalTransform);

  glm::mat4 localTransform;
  glm::mat4 globalTransform;
};

class Mesh {
public:
  Mesh(VertexArray vao, std::shared_ptr<PBRMaterial> mat);
  std::shared_ptr<PBRMaterial> material;
  Transform transform;
  GLsizei vertexCount;
  VertexArray vao;
};

class Model {
public:
  Model();
  Transform parentTransform;
  std::vector<Mesh> meshes;
  int meshCount = 0;
};

#endif // MODEL_H
