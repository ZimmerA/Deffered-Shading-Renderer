#pragma once
#ifndef PROFILING_ENABLED
#define PROFILING_ENABLED 1
#endif
#if PROFILING_ENABLED
#else
#define SCOPED_TIMER_QUERY(x)
#endif
#include "glad.h"

class GLTimerQuery {
public:
  explicit GLTimerQuery(double &_result);
  GLTimerQuery(const GLTimerQuery &) = delete;
  GLTimerQuery(const GLTimerQuery &&) = delete;
  GLTimerQuery &operator=(const GLTimerQuery &) = delete;
  GLTimerQuery &operator=(const GLTimerQuery &&) = delete;
  ~GLTimerQuery();

private:
  double &m_result;
  GLuint m_id;
};