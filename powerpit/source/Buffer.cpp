#include "PCH.h"

#include "Buffer.h"

#include "SafeCall.h"


Buffer::Buffer(const GLenum type, const size_t size, const GLenum drawType)
	:
	size(size),
	type(type),
	drawType(drawType)
{
	GLcall(glGenBuffers(1, &id));
	GLcall(glBindBuffer(type, id));
	GLcall(glBufferData(type, size, nullptr, drawType));
}

Buffer::Buffer(Buffer&& old) noexcept
	:
	size(std::move(old.size)),
	type(std::move(old.type)),
	drawType(std::move(old.drawType)),
	id(std::move(old.id))
{
	old.id = -1;
}

Buffer::~Buffer()
{
	Unbind();
	if (id != -1)
		glDeleteBuffers(1, &id);
}

void Buffer::CopyData(const unsigned int offset, const size_t dataSize, const void* from)
{
	Bind();
	GLcall(glBufferSubData(type, offset, size, from));
}

void Buffer::SetMat4f(const unsigned int offset, const gm::mat4& mat)
{
	Bind();
	GLcall(glBufferSubData(type, offset, sizeof(gm::mat4), gm::value_ptr(mat)));
}

void Buffer::Bind()
{
	GLcall(glBindBuffer(type, id));
}

void Buffer::Unbind()
{
	glBindBuffer(type, 0);
}


// UNIFORM BUFFER

UniformBuffer::UniformBuffer(const unsigned int bindingPoint, const size_t size)
	:
	Buffer(GL_UNIFORM_BUFFER, size, GL_DYNAMIC_DRAW),
	bindingBlock(bindingPoint)
{

}

void UniformBuffer::Bind()
{
	GLcall(glBindBuffer(GL_UNIFORM_BUFFER, id));

	// automatically binds itself to binding block
	BindToBlock(bindingBlock);
}

void UniformBuffer::BindToBlock(const unsigned int block)
{
	GLcall(glBindBufferBase(GL_UNIFORM_BUFFER, block, id));
}