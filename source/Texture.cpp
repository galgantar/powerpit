#include "PCH.h"

#include "Texture.h"
#include "SafeCall.h"


Texture::Texture(const char* filename, bool needAlpha)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // minifying
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // magnifying

	int soilLoad, textureType;
	if (needAlpha) {
		soilLoad = SOIL_LOAD_RGBA;
		textureType = GL_RGBA;
	}
	else {
		soilLoad = SOIL_LOAD_RGB;
		textureType = GL_RGB;
	}

	int width, height;
	unsigned char* data = SOIL_load_image(filename, &width, &height, nullptr, soilLoad);
	if (!data) {
		std::cout << "Texture source for " << filename << " not found!" << std::endl;
	}


	GLcall(glGenTextures(1, &id));
	GLcall(glBindTexture(GL_TEXTURE_2D, id));
	GLcall(glTexImage2D(GL_TEXTURE_2D, 0, textureType, width, height, 0, textureType, GL_UNSIGNED_BYTE, data));
	GLcall(glGenerateMipmap(GL_TEXTURE_2D));

	SOIL_free_image_data(data);
	Unbind();
}

Texture::~Texture()
{
	GLcall(glDeleteTextures(1, &id));
}

void Texture::Bind(unsigned int unit)
{
	GLcall(glActiveTexture(GL_TEXTURE0 + unit));
	GLcall(glBindTexture(GL_TEXTURE_2D, id));
}

void Texture::Unbind()
{
	GLcall(glBindTexture(GL_TEXTURE_2D, 0));
}
