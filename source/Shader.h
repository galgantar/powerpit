#pragma once

#include <string>
#include <glew.h>

#include "SafeCall.h"

class Shader {
private:
	unsigned int id;

	void ParseFile(const char* filename, std::string& out);
	unsigned int CompileShader(const char* sourceFile, GLenum type);

public:
	Shader(const char* vertexSource, const char* fragmentSource);
	~Shader();
	void Bind();
	void Unbind();
};