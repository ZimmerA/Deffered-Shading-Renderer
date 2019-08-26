#include <algorithm>

#include "vertexArray.h"

VertexArray::VertexArray(VertexBuffer vbo, VertexBuffer ebo, GLenum drawMode)
    : drawMode(drawMode), indexed{true} {
  bind();
  buffers.push_back(std::move(vbo));
  buffers.push_back(std::move(ebo));

  glBindBuffer(GL_ARRAY_BUFFER, buffers.at(0).handle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.at(1).handle);
  descriptorToAttribPointer(buffers.at(0).elementDescriptor);
  unbind();
}

VertexArray::VertexArray(VertexBuffer vbo, GLenum drawMode)
    : drawMode(drawMode), indexed(false) {
  bind();
  buffers.push_back(std::move(vbo));
  glBindBuffer(GL_ARRAY_BUFFER, buffers.at(0).handle);
  descriptorToAttribPointer(buffers.at(0).elementDescriptor);
  unbind();
}

void VertexArray::bind() noexcept { glBindVertexArray(handle); }

void VertexArray::unbind() noexcept { glBindVertexArray(0); }

void VertexArray::draw() noexcept {
  bind();
  if (indexed) {
    glDrawElements(drawMode, static_cast<GLsizei>(buffers.at(1).elementCount),
                   GL_UNSIGNED_SHORT, 0);
  } else {
    glDrawArrays(drawMode, 0, static_cast<GLsizei>(buffers.at(0).elementCount));
  }
  unbind();
}

void VertexArray::vertexAttribPointer(GLuint index, GLint size, GLenum type,
                                      GLboolean normalized, size_t stride,
                                      size_t offset) noexcept {
  enableVertexAttributeArray(index);
  glVertexAttribPointer(index, size, type, normalized,
                        static_cast<GLsizei>(stride),
                        reinterpret_cast<const GLvoid *>(offset));
}

void VertexArray::enableVertexAttributeArray(int index) noexcept {
  glEnableVertexAttribArray(index);
}

void VertexArray::disableVertexAttributeArray(int index) noexcept {
  glDisableVertexAttribArray(index);
}

void VertexArray::descriptorToAttribPointer(
    const VertexElementDescriptorVec &descriptor) {
  size_t offset = 0;
  for (auto &part : descriptor) {
    switch (part.type) {
    case ET_FLOAT2:
      vertexAttribPointer(part.semantic, 2, GL_FLOAT, GL_FALSE,
                          buffers.at(0).stride, offset);
      offset += sizeof(float) * 2;
      break;
    case ET_FLOAT3:
      vertexAttribPointer(part.semantic, 3, GL_FLOAT, GL_FALSE,
                          buffers.at(0).stride, offset);
      offset += sizeof(float) * 3;
      break;
    case ET_FLOAT4:
      vertexAttribPointer(part.semantic, 4, GL_FLOAT, GL_FALSE,
                          buffers.at(0).stride, offset);
      offset += sizeof(float) * 4;
      break;
    case ET_USHORT2:
      vertexAttribPointer(part.semantic, 2, GL_UNSIGNED_SHORT, GL_FALSE,
                          buffers.at(0).stride, offset);
      offset += sizeof(unsigned short) * 2;
      break;
    case ET_USHORT4:
      vertexAttribPointer(part.semantic, 4, GL_UNSIGNED_SHORT, GL_FALSE,
                          buffers.at(0).stride, offset);
      offset += sizeof(unsigned short) * 4;
      break;
    }
  }
}
