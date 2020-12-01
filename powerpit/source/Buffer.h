#pragma once

#include "PCH.h"

class Buffer
{
protected:
	size_t size;
	const GLenum type;
	const GLenum drawType;
	unsigned int id;

public:
	Buffer(const GLenum type, const size_t size, const GLenum drawType);
	Buffer(const Buffer& b) = delete;
	Buffer(Buffer&& old) noexcept;
	~Buffer();

	void CopyData(const unsigned int offset, const size_t dataSize, const void* from);

	void SetMat4f(const unsigned int offset, const gm::mat4& mat);

	virtual void Bind();
	void Unbind();
};


class UniformBuffer : public Buffer
{
private:
	const unsigned int bindingBlock;

public:
	UniformBuffer(const unsigned int bindingPoint, const size_t size);
	UniformBuffer(const UniformBuffer& ub) = delete;
	~UniformBuffer() {};

	void BindToBlock(const unsigned int bindingBlock);
	void Bind() override;
};