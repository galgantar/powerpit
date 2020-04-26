#pragma once

#include "PCH.h"

class Shader {
public: // remove later
	unsigned int id;
private:
	std::unordered_map<std::string, int> uniforms;

	void ParseFile(const char* filename, std::string& out);
	unsigned int CompileShader(const char* sourceFile, GLenum type);
	int GetUniformLocation(const std::string& name);

public:
	Shader(const char* vertexSource, const char* fragmentSource);
	~Shader();
	void Bind();
	void Unbind();
	void SetUniform4f(const std::string& name, const glm::vec4& value);
	void SetUniform3f(const std::string& name, const glm::vec3& value);
	void SetUniform1i(const std::string& name, const int value);
	void SetUniformMat4f(const std::string& name, const glm::mat4& value);
};