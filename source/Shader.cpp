#pragma once

#include <fstream>
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

Shader::Shader(const char* vertexSource, const char* fragmentSource)
{
	unsigned int vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
	unsigned int fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
	
	id = glCreateProgram();
	GLcall(glAttachShader(id, vertexShader));
	GLcall(glAttachShader(id, fragmentShader));
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
