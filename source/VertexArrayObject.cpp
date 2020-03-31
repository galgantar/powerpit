#include "VertexArrayObject.h"
#include "SafeCall.h"

#include <glew.h>

void VertexArrayObject::Build()
{
	GLcall(glBindVertexArray(id));

	for (const VertexAttribute& att : *attributes) {
		GLcall(glVertexAttribPointer(att.location, att.count, att.type, att.normalized, stride, (void*)att.offset));
		
		GLcall(glEnableVertexAttribArray(att.location));
	}

	delete attributes;
	attributes = nullptr;

	isBuild = true;
}

VertexArrayObject::VertexArrayObject()
	: stride(0), isBuild(false)
{
	attributes = new std::vector<VertexAttribute>();
	
	glGenVertexArrays(1, &id);
	
	GLcall(glBindVertexArray(id));
}

VertexArrayObject::~VertexArrayObject()
{
	GLcall(glDeleteVertexArrays(1, &id));
	Unbind();
}

void VertexArrayObject::AddVertexAttribute(const int location, const int count, GLenum type, GLenum normalized)
{
	int offset = stride;
	
	switch (type)
	{
		case GL_FLOAT: stride += sizeof(float) * count; break;
		
		default: ASSERT(false);
	}

	attributes->push_back({ location, count, type, normalized, offset});
}

void VertexArrayObject::Bind()
{
	if (!isBuild)
		Build();
	
	GLcall(glBindVertexArray(id));
}

void VertexArrayObject::Unbind()
{
	GLcall(glBindVertexArray(0));
}
