#pragma once


class Texture
{
private:
	unsigned int id;
	GLenum type;

	void Load2DTextureFromFile(const std::string& file, bool needAlpha, GLenum dest);
	void Set2DSettings();
	void SetCubemapSettings();


public:
	Texture(const std::string& path, const GLenum type, bool needAlpha);
	Texture(const Texture& t) = delete;
	Texture& operator=(const Texture& t) = delete;
	Texture(Texture&& old) noexcept;
	~Texture();

	void Bind(unsigned int unit) const;
	void Unbind() const;

	// temporary
	inline unsigned int GetID() { return id; }
};