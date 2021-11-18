#pragma once

#include <GL/glew.h>
// C++ Macro - these are compiler specific.
// __debugbreak is VS specific.
# define ASSERT(x) if (!(x)) __debugbreak();
# define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));    

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
