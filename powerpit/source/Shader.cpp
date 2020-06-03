#pragma once

#include "PCH.h"

#include "Shader.h"
#include "SafeCall.h"


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
			case GL_GEOMETRY_SHADER: s_type = "GeometryShader"; break;
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

Shader::Shader(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	id = glCreateProgram();
	
	unsigned int vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
	unsigned int fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
	
	GLcall(glAttachShader(id, vertexShader));
	GLcall(glAttachShader(id, fragmentShader));
	GLcall(glDeleteShader(vertexShader));
	GLcall(glDeleteShader(fragmentShader));


	if (geometrySource) {
		unsigned int geometryShader = CompileShader(geometrySource, GL_GEOMETRY_SHADER);
		GLcall(glAttachShader(id, geometryShader));
		GLcall(glDeleteShader(geometryShader));
	}


	// Link output of vertex to fragment shader
	GLcall(glLinkProgram(id));

	int success;
	GLcall(glGetProgramiv(id, GL_LINK_STATUS, &success));
	if (!success) {
		char infoLog[512];
		GLcall(glGetProgramInfoLog(id, 512, nullptr, infoLog));
		std::cout << "Shader linking failed " << infoLog << vertexSource << fragmentSource << std::endl;
	}
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

void Shader::SetUniform4f(const std::string& name, const gm::vec4& value)
{
	Bind();
	int location = GetUniformLocation(name);
	if (location != -1)
		GLcall(glUniform4f(location, value.x, value.y, value.z, value.w));
	Unbind();
}

void Shader::SetUniform3f(const std::string& name, const gm::vec3& value)
{
	Bind();
	int location = GetUniformLocation(name);
	if (location != -1)
		GLcall(glUniform3f(location, value.x, value.y, value.z));
	Unbind();
}

void Shader::SetUniform1f(const std::string& name, const float f)
{
	Bind();
	int location = GetUniformLocation(name);
	if (location != -1)
		GLcall(glUniform1f(location, f));
	Unbind();
}

void Shader::SetUniform1i(const std::string& name, const int value)
{
	Bind();
	int location = GetUniformLocation(name);
	if (location != -1)
		GLcall(glUniform1i(location, value));
	Unbind();
}

void Shader::SetUniformMat4f(const std::string& name, const gm::mat4& value)
{
	Bind();
	int location = GetUniformLocation(name);
	if (location != -1)
		GLcall(glUniformMatrix4fv(location, 1, GL_TRUE, gm::value_ptr(value)));
	Unbind();
}