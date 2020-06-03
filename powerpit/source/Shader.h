#pragma once

#include "PCH.h"

class Shader {
private:
	unsigned int id;
	std::unordered_map<std::string, int> uniforms;

	void ParseFile(const char* filename, std::string& out);
	unsigned int CompileShader(const char* sourceFile, GLenum type);
	int GetUniformLocation(const std::string& name);

public:
	Shader(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
	~Shader();
	void Bind();
	void Unbind();
	void SetUniform4f(const std::string& name, const gm::vec4& value);
	void SetUniform3f(const std::string& name, const gm::vec3& value);
	void SetUniform1f(const std::string& name, const float f);
	void SetUniform1i(const std::string& name, const int value);
	void SetUniformMat4f(const std::string& name, const gm::mat4& value);

	// temporary 
	inline unsigned int GetID() { return id; }
};