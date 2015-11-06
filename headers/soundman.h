#pragma once

class SoundManager : public ResourceManager
{
	friend class DanFrame;
public:
	SoundManager(std::string directory) : ResourceManager(directory)
	{
	}
	virtual bool Precache(std::string fileName);
	virtual void Reload(std::string fileName);
	virtual void Unload(std::string fileName);
	virtual void ReloadAll();
	virtual void UnloadAll();
	void Acquire(sf::SoundBuffer &snd, std::string fileName);
	std::shared_ptr<sf::SoundBuffer> GetSound(std::string name);

	void Remap(const std::string &name, const std::string &newname);
private:
	std::map<std::string, Resource_t<sf::SoundBuffer>> sndmap;
};