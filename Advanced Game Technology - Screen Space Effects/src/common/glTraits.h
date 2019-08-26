#ifndef GLTRAITS_H
#define GLTRAITS_H

#include "glad.h"

// TODO: Find a way to use SFINAE to deal with the shadertype in the
// OpenGLObjectBase constructor
/*
struct ShaderTraits {
    typedef GLuint value_type;
    typedef GLenum parameter_type;
    static value_type Create(parameter_type shaderType);
    static void Destroy(value_type handle);
};
*/

struct ShaderProgramTraits {
  typedef GLuint value_type;

  static value_type create() noexcept;

  static void destroy(value_type handle) noexcept;
};

struct TextureTraits {
  typedef GLuint value_type;

  static value_type create() noexcept;

  static void destroy(value_type handle) noexcept;
};

struct VertexArrayObjectTraits {
  typedef GLuint value_type;

  static value_type create() noexcept;

  static void destroy(value_type handle) noexcept;
};

struct VertexBufferObjectTraits {
  typedef GLuint value_type;

  static value_type create() noexcept;

  static void destroy(value_type handle) noexcept;
};

struct FrameBufferTraits {
  typedef GLuint value_type;

  static value_type create() noexcept;

  static void destroy(value_type handle) noexcept;
};

struct RenderBufferTraits {
  typedef GLuint value_type;

  static value_type create() noexcept;

  static void destroy(value_type handle) noexcept;
};

#endif // GLTRAITS_H
