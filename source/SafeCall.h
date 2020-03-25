#include <glew.h>
#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLcall(function) GLClearError(); function; ASSERT(GLLogCall(#function, __FILE__, __LINE__));

void GLClearError();
bool GLLogCall(const char* function, const char* file, const int line);