#pragma once


#include <fstream>
#include <unordered_map>
#include <gtc/type_ptr.hpp>

#include "Shader.h"


void Shader::ParseFile(const char* filename, std::string& out)
{
	std::ifstream stream(filename);
	if (stream.fail())
		std::cout << "Filename : " << filename << "does not exist" << std::endl;

	std::string line;

	while (std::getline(stream, line)) {
		out += line;
		out += '\n';
	}
}

unsigned int Shader::CompileShader(const char* sourceFile, GLenum type)
{
	unsigned int shader;
	std::string source;
	
	shader = glCreateShader(type);
	
	ParseFile(sourceFile, source);
	const char* pointer = source.c_str();
	
	glShaderSource(shader, 1, &pointer, nullptr);
	GLcall(glCompileShader(shader));

	int success;
	GLcall(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
	
	if (!success) {
		char infoLog[512];
		const char* s_type;
		GLcall(glGetShaderInfoLog(shader, 512, nullptr, infoLog));
		
		switch (type) {
			case GL_VERTEX_SHADER: s_type = "VertexShader"; break;
			case GL_FRAGMENT_SHADER: s_type = "FragmentShader"; break;
			default: s_type = "UnknownType";
		}
		
		std::cout << "Compilation of type " << s_type << " failed : " << infoLog << std::endl;
	}

	return shader;
}

int Shader::GetUniformLocation(const std::string& name)
{	
	std::unordered_map<std::string, int>::iterator it;
	
	if ((it = uniforms.find(name)) != uniforms.end())
		return it->second;

	int location = glGetUniformLocation(id, name.c_str());
	
	if (location == -1)
		std::cout << "Uniform " << name << " not found!" << std::endl;
 
	uniforms[name] = location;
	
	return location;
}

Shader::Shader(const char* vertexSource, const char* fragmentSource)
{
	unsigned int vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
	unsigned int fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
	
	id = glCreateProgram();
	GLcall(glAttachShader(id, vertexShader));
	GLcall(glAttachShader(id, fragmentShader));

	// Link output of vertex to fragment shader
	GLcall(glLinkProgram(id));

	int success;
	GLcall(glGetProgramiv(id, GL_LINK_STATUS, &success));
	if (!success) {
		char infoLog[512];
		GLcall(glGetProgramInfoLog(id, 512, nullptr, infoLog));
		std::cout << "Shader linking failed " << infoLog << std::endl;
	}

	GLcall(glDeleteShader(vertexShader));
	GLcall(glDeleteShader(fragmentShader));
}

Shader::~Shader()
{
	Unbind();
	GLcall(glDeleteProgram(id));
}

void Shader::Bind()
{
	GLcall(glUseProgram(id));
}

void Shader::Unbind()
{
	GLcall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, const glm::vec4& value)
{
	int location = GetUniformLocation(name);
	if (location != -1)
		GLcall(glUniform4f(location, value.x, value.y, value.z, value.w));
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& value)
{
	int location = GetUniformLocation(name);
	if (location != -1)
		GLcall(glUniform3f(location, value.x, value.y, value.z));
}

void Shader::SetUniform1i(const std::string& name, const int value)
{
	int location = GetUniformLocation(name);
	if (location != -1)
		GLcall(glUniform1i(location, value));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& value)
{
	int location = GetUniformLocation(name);
	if (location != -1)
		GLcall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value)));
}
