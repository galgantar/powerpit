#pragma once

#include <glew.h>
#include <vector>

struct VertexAttribute
{
	int location;
	int count;
	GLenum type;
	unsigned int normalized;
	
	/* distance from the beginnig of the single vertex data to the current attribute */
	int offset;
};

class VertexArrayObject {
private:
	unsigned int id;
	
	/* distance to next attribute of the same type */
	int stride;
	std::vector<VertexAttribute>* attributes;
	bool isBuilt;

	void Build();

public:
	/* Binds VAO on initialization */
	VertexArrayObject();
	~VertexArrayObject();

	void AddVertexAttribute(const int location, const int count, GLenum type, GLenum normalized); 
	
	/* Bind MUST be called after last call of AddVertexAttribute() */
	void Bind();
	void Unbind();
};