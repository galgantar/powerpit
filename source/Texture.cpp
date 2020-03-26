#include "Texture.h"

#include "SafeCall.h"
#include <glew.h>
#include <stb_image.h>

Texture::Texture(const char* filename, int format)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // minifying
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // magnifying
	

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (!data) {
		std::cout << "Texture source for " << filename << " not found!" << std::endl;
	}


	GLcall(glGenTextures(1, &id));
	GLcall(glBindTexture(GL_TEXTURE_2D, id));
	GLcall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
	GLcall(glGenerateMipmap(GL_TEXTURE_2D));

	stbi_image_free(data);
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
