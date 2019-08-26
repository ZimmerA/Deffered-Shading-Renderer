#ifndef VBO_H
#define VBO_H

#include <vector>

#include "glTraits.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "openGLObjectBase.h"

struct Vertex {
  glm::vec3 position;
  glm::vec2 uv;
};

enum ElementSemantic {
  POSITION = 0,
  TEXCOORD = 1,
  NORMAL = 2,
  TANGENT = 3,
  DIFFUSE,
  SEMANTIC_COUNT // Used to tell how many semantics exist
};

enum ElementType { ET_FLOAT2, ET_FLOAT3, ET_FLOAT4, ET_USHORT2, ET_USHORT4 };

struct VertexElementDescriptor {
  ElementSemantic semantic;
  ElementType type;
};

typedef std::vector<VertexElementDescriptor> VertexElementDescriptorVec;

class VertexBuffer : public OpenGLObjectBase<VertexBufferObjectTraits> {
public:
  VertexBuffer(size_t elementCount, size_t stride = 0,
               VertexElementDescriptorVec elementDescriptor =
                   VertexElementDescriptorVec());
  void buffer(GLenum target, GLsizeiptr size, const GLvoid *data,
              GLenum mode) noexcept;

  VertexElementDescriptorVec elementDescriptor;
  size_t elementCount;
  size_t stride;
};

#endif // VBO_H
