#include "PCH.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLcall(function) GLClearError(); function; GLLogCall(#function, __FILE__, __LINE__);

void GLClearError();
void GLLogCall(const char* function, const char* file, const int line);