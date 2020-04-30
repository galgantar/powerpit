#include "PCH.h"

#include "VertexArrayObject.h"

#include "SafeCall.h"


VertexArrayObject::VertexArrayObject()
	: stride(0), isBuilt(false)
{
	attributes = new std::vector<VertexAttribute>();
	
	glGenVertexArrays(1, &id);
	
	Bind();
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

void VertexArrayObject::Build()
{
	GLcall(glBindVertexArray(id));

	for (const VertexAttribute& att : *attributes) {		
		GLcall(glVertexAttribPointer(att.location, att.count, att.type, att.normalized, stride, (void*)att.offset));

		GLcall(glEnableVertexAttribArray(att.location));
	}

	delete attributes;
	attributes = nullptr;

	isBuilt = true;
}

void VertexArrayObject::Bind()
{	
	GLcall(glBindVertexArray(id));
}

void VertexArrayObject::Unbind()
{
	GLcall(glBindVertexArray(0));
}
