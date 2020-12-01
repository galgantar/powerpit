#include "PCH.h"

#include "Texture.h"
#include "SafeCall.h"


void Texture::Set2DSettings()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); // minifying
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR); // magnifying
}

void Texture::SetCubemapSettings()
{
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Texture::Texture(const std::string& path, const GLenum type, bool needAlpha)
	: 
		type(type)
		
{
	GLcall(glGenTextures(1, &id));

	switch (type) {

		case GL_TEXTURE_2D:
			GLcall(glBindTexture(GL_TEXTURE_2D, id));

			Load2DTextureFromFile(path, needAlpha, GL_TEXTURE_2D);

			GLcall(glGenerateMipmap(GL_TEXTURE_2D));
		
			Set2DSettings();
			Unbind();
		
			break;


		case GL_TEXTURE_CUBE_MAP:
		{
			GLcall(glBindTexture(GL_TEXTURE_CUBE_MAP, id));

			// RGB hardcoded
			const char* faces[] = {
			"right.jpg",
			"left.jpg",
			"top.jpg",
			"bottom.jpg",
			"front.jpg",
			"back.jpg"
			};

			for (int i = 0; i < 6; ++i) {
				Load2DTextureFromFile(path + faces[i], needAlpha, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			}

			SetCubemapSettings(); // important
			Unbind();

			break;
		}
		default:
			const char* faces = nullptr;
			std::cout << "something went wrong " << std::endl;
			ASSERT(false);
	};
}

Texture::Texture(Texture&& old) noexcept
	: 
		id(std::move(old.id)),
		type(std::move(old.type))
{
	old.id = -1;
}

Texture::~Texture()
{
	if (id != -1)
		glDeleteTextures(1, &id);
}

void Texture::Load2DTextureFromFile(const std::string& filename, bool needAlpha, GLenum dest)
{	
	int soilLoadType, channelType;
	if (needAlpha) {
		soilLoadType = SOIL_LOAD_RGBA;
		channelType = GL_RGBA;
	}
	else {
		soilLoadType = SOIL_LOAD_RGB;
		channelType = GL_RGB;
	}

	int width, height, nrChannels;
	unsigned char* data = SOIL_load_image(filename.c_str(), &width, &height, nullptr, soilLoadType);

	if (!data) {
		std::cout << "Texture source for " << filename << " not found!" << std::endl;
	}
	
	GLcall(glTexImage2D(dest, 0, channelType, width, height, 0, channelType, GL_UNSIGNED_BYTE, data));
	
	SOIL_free_image_data(data);
}

void Texture::Bind(unsigned int unit) const
{
	GLcall(glActiveTexture(GL_TEXTURE0 + unit));
	GLcall(glBindTexture(type, id));
}

void Texture::Unbind() const
{
	glBindTexture(type, 0);
}
