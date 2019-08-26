#include "vertexBuffer.h"

VertexBuffer::VertexBuffer(size_t elementCount, size_t stride,
                           VertexElementDescriptorVec elementDescriptor)
    : elementCount(elementCount), stride(stride),
      elementDescriptor(std::move(elementDescriptor)){};

void VertexBuffer::buffer(GLenum target, GLsizeiptr size, const GLvoid *data,
                          GLenum mode) noexcept {
  glBindBuffer(target, handle);
  glBufferData(target, size, data, mode);
}