#pragma once

class FontManager : public ResourceManager
{
public:
	FontManager(std::string directory) : ResourceManager(directory)
	{
	}
	virtual bool Precache(std::string fileName);
	virtual void Reload(std::string fileName);
	virtual void Unload(std::string fileName);
	virtual void ReloadAll();
	virtual void UnloadAll();
	void Acquire(sf::Font &fnt, std::string fileName);
	std::shared_ptr<sf::Font> GetFont(std::string name);
private:
	std::map<std::string, Resource_t<sf::Font>> fontmap;
};