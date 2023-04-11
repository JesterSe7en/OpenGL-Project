#pragma once

#include <GL/glew.h>

// this define only works on MSVC compiler
#define ASSERT(x)                                                              \
  if (!(x))                                                                    \
    __debugbreak();

#define DEBUG = 1

#ifdef DEBUG
#define GLCall(x)                                                              \
  GLClearError();                                                              \
  x;                                                                           \
  ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif

void GLClearError();
bool GLLogCall(const char *function, const char *file, int line);
