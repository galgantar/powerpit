#pragma once


class Texture {
private:
	unsigned int id;
	std::string aiType;
	std::string path;

	void LoadTextureFromFile(const std::string& file, bool needAlpha);

public:
	Texture(const std::string& path, bool needAlpha, const std::string& type);
	Texture(const Texture& t);
	Texture& operator=(const Texture& t);
	Texture(Texture&& old) noexcept;
	~Texture();

	void Bind(unsigned int unit);
	void Unbind();

	inline unsigned int GetID() { return id; }
};