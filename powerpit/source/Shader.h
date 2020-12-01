#pragma once

#include "PCH.h"


class Shader
{
private:
	unsigned int id;
	std::unordered_map<std::string, int> uniforms;

	void ParseFile(const std::string& filename, std::string& out);
	unsigned int CompileShader(const std::string& sourceFile, GLenum type);
	int FindUniformLocation(const std::string& name);
	int GetUniformLocation(const std::string& name);

public:
	Shader(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource="");
	Shader(Shader&& old) noexcept;
	~Shader();


	void Bind();
	void Unbind();

	bool HasUniform(const std::string& name);

	void SetUniform4f(const std::string& name, const gm::vec4& value);
	void SetUniform3f(const std::string& name, const gm::vec3& value);
	void SetUniform1f(const std::string& name, const float f);
	void SetUniform1i(const std::string& name, const int value);
	void SetUniformMat4f(const std::string& name, const gm::mat4& value);

	void LinkToUniformBufferBinding(const unsigned int bindingPoint, const std::string& name);

	// temporary 
	inline unsigned int GetID() { return id; }
};
