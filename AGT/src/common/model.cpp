#define STBI_MSC_SECURE_CRT
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>

#include "glm/gtc/type_ptr.hpp"
#include "model.h"
#include "shader.h"

Transform::Transform(glm::mat4 localTransform, glm::mat4 globalTransform)
    : localTransform(localTransform), globalTransform(globalTransform) {}

Mesh::Mesh(VertexArray vao, std::shared_ptr<PBRMaterial> mat)
    : vao(std::move(vao)), material(mat) {}

Model::Model() : parentTransform(glm::mat4(1), glm::mat4(1)) {}