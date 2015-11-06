#pragma once

class TextureManager : public ResourceManager
{
	friend class DanFrame;
public:
	TextureManager(std::string directory) : ResourceManager(directory)
	{
		CreateEmptyTexture("TNT1A0");
	}
	virtual bool Precache(std::string fileName);
	virtual void Reload(std::string fileName);
	virtual void Unload(std::string fileName);
	virtual void ReloadAll();
	virtual void UnloadAll();
	void SetFiltering(bool enable);
	void CreateEmptyTexture(const std::string &texName);
	void Acquire(sf::Texture &tex, std::string fileName);
	void Remap(const std::string &name, const std::string &newname);
	std::shared_ptr<sf::Texture> GetTexture(std::string name);
private:
	std::map<std::string, Resource_t<sf::Texture>> texmap;
};