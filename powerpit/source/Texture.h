#pragma once

class Texture {
private:
	unsigned int id;

public:
	Texture(const char* name, bool needAlpha);
	~Texture();
	void Bind(unsigned int unit);
	void Unbind();
};