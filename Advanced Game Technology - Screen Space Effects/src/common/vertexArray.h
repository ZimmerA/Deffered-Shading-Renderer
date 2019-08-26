#ifndef VAO_H
#define VAO_H

#include "glTraits.h"
#include "glad/glad.h"
#include "openGLObjectBase.h"
#include "vertexBuffer.h"

class VertexArray : public OpenGLObjectBase<VertexArrayObjectTraits> {
public:
  VertexArray(VertexBuffer vbo, VertexBuffer ebo, GLenum drawMode);
  VertexArray(VertexBuffer vbo, GLenum drawMode);

  void bind() noexcept;
  void unbind() noexcept;
  void draw() noexcept;
  void vertexAttribPointer(GLuint index, GLint size, GLenum type,
                           GLboolean normalized, size_t stride,
                           size_t offset) noexcept;

private:
  void enableVertexAttributeArray(int index) noexcept;
  void disableVertexAttributeArray(int index) noexcept;
  void descriptorToAttribPointer(const VertexElementDescriptorVec &descriptor);
  std::vector<VertexBuffer> buffers;
  GLenum drawMode;
  bool indexed;
};

#endif // VAO_H
