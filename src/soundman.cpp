#include "libs.h"
#include "util.h"
#include "static.h"
#include "file.h"
#include "configfile.h"
#include "ccmd.h"
#include "cvar.h"
#include "console.h"
#include "token.h"
#include "strmanip.h"
#include "resrcman.h"
#include "soundman.h"
#include "audio.h"

void SoundManager::UnloadAll()
{
	Audio::StopAllSounds();
	Audio::ResetSlots();
	sndmap.clear();
}

void SoundManager::ReloadAll()
{
	Audio::StopAllSounds();
	Audio::ResetSlots();
	for(auto it = sndmap.begin(); it != sndmap.end(); ++it)
	{
		Reload(it->first);
	}
}

void SoundManager::Unload( std::string fileName )
{
	Audio::ResetSlots();
	sndmap.erase(fileName);
}

void SoundManager::Reload( std::string fileName )
{
	Audio::ResetSlots();
	Unload(fileName);
	Precache(fileName);
}

bool SoundManager::Precache( std::string fileName )
{
	bool success = false;
	std::string origname = fileName;
	std::shared_ptr<sf::SoundBuffer> t(new sf::SoundBuffer);

	if(sndmap.find(fileName) != sndmap.end())
		return true;

	for(int i = 0; i < m_searchDirs.size(); i++)
	{
		if(!HasDirectory(origname))
			fileName = m_searchDirs[i] + origname;

		if(t->loadFromFile(fileName))
		{
			success = true;
			break;
		}
	}

	if(!success)
	{
		//t.reset();
		Console->LogMessage("Could not load sound '%s'", fileName);
		//return false;
	}

	Resource_t<sf::SoundBuffer> resrc;
	resrc.ptr = t;
	resrc.file_name = GetStringFilename(fileName);
	resrc.file_dir = GetStringDirectory(fileName);

	this->sndmap.insert(std::pair<std::string, Resource_t<sf::SoundBuffer>>(origname, resrc));

	return true;
}

std::shared_ptr<sf::SoundBuffer> SoundManager::GetSound(std::string name)
{
	if(sndmap.find(name) == sndmap.end())
		Precache(name);
	//if(!Precache(name))
	//	return nullptr;

	return sndmap[name].ptr;
}

void SoundManager::Acquire( sf::SoundBuffer &snd, std::string fileName )
{
	Resource_t<sf::SoundBuffer> resrc;
	resrc.ptr = std::shared_ptr<sf::SoundBuffer>(new sf::SoundBuffer(snd));
	resrc.file_name = GetStringFilename(fileName);
	resrc.file_dir = GetStringDirectory(fileName);
	this->sndmap.insert(std::pair<std::string, Resource_t<sf::SoundBuffer>>(resrc.file_name, resrc));
}

void SoundManager::Remap(const std::string &name, const std::string &newname)
{
	sndmap[newname] = sndmap[name];
	sndmap.erase(name);
}
