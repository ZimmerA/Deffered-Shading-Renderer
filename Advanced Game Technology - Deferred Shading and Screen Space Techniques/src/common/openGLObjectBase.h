#ifndef OPENGLOBJECTBASE_H
#define OPENGLOBJECTBASE_H

#include <algorithm>

#include "glad.h"

template <typename T> class OpenGLObjectBase {
public:
  OpenGLObjectBase() : handle(T::create()) {}

  ~OpenGLObjectBase() { T::destroy(handle); }

  OpenGLObjectBase(const OpenGLObjectBase &) = delete;

  OpenGLObjectBase &operator=(const OpenGLObjectBase &) = delete;

  // Move constructor
  OpenGLObjectBase(OpenGLObjectBase &&other) : handle(other.handle) {
    other.handle = 0;
  }

  OpenGLObjectBase &operator=(OpenGLObjectBase &&other) {
    if (this != &other) {
      T::destroy(handle);
      std::swap(handle, other.handle);
    }
  }

  typename T::value_type handle;
};

#endif // OPENGLOBJECTBASE_H
