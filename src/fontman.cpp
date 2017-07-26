#include "libs.h"
#include "util.h"
#include "static.h"
#include "RTFText.h"
#include "file.h"
#include "configfile.h"
#include "ccmd.h"
#include "cvar.h"
#include "console.h"
#include "token.h"
#include "strmanip.h"
#include "resrcman.h"
#include "fontman.h"

void FontManager::UnloadAll()
{
	fontmap.clear();
}

void FontManager::ReloadAll()
{
	for(auto it = fontmap.begin(); it != fontmap.end(); ++it)
	{
		Reload(it->first);
	}
}

void FontManager::Unload( std::string fileName )
{
	fontmap.erase(fileName);
}

void FontManager::Reload( std::string fileName )
{
	Unload(fileName);
	Precache(fileName);
}

bool FontManager::Precache( std::string fileName )
{
	bool success = false;
	std::string origname = fileName;
	std::shared_ptr<sf::Font> f(new sf::Font);

	if(fontmap.find(fileName) != fontmap.end())
		return true;

	for(int i = 0; i < m_searchDirs.size(); i++)
	{
		if(!HasDirectory(origname))
			fileName = m_searchDirs[i] + origname;

		if(f->loadFromFile(fileName))
		{
			success = true;
			break;
		}
	}

	if(!success)
	{
		//t.reset();
		Console->LogMessage("Could not load font '%s'", fileName);
		//return false;
	}

	Resource_t<sf::Font> resrc;
	resrc.ptr = f;
	resrc.file_name = GetStringFilename(fileName);
	resrc.file_dir = GetStringDirectory(fileName);

	this->fontmap.insert(std::pair<std::string, Resource_t<sf::Font>>(origname, resrc));

	return true;
}

std::shared_ptr<sf::Font> FontManager::GetFont(std::string name)
{
	if(fontmap.find(name) == fontmap.end())
		Precache(name);
	//if(!Precache(name))
	//	return nullptr;

	return fontmap[name].ptr;
}

void FontManager::Acquire( sf::Font &fnt, std::string fileName )
{
	Resource_t<sf::Font> resrc;
	resrc.ptr = std::shared_ptr<sf::Font>(new sf::Font(fnt));
	resrc.file_name = GetStringFilename(fileName);
	resrc.file_dir = GetStringDirectory(fileName);
	this->fontmap.insert(std::pair<std::string, Resource_t<sf::Font>>(resrc.file_name, resrc));
}