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
#include "texman.h"


void TextureManager::SetFiltering(bool enable)
{
	for(auto it = texmap.begin(); it != texmap.end(); ++it)
	{
		it->second.ptr->setSmooth(enable);
	}
}

void TextureManager::UnloadAll()
{
	texmap.clear();
}

void TextureManager::ReloadAll()
{
	for(auto it = texmap.begin(); it != texmap.end(); ++it)
	{
		Reload(it->first);
	}
}

void TextureManager::Unload( std::string fileName )
{
	texmap.erase(fileName);
}

void TextureManager::Reload( std::string fileName )
{
	Unload(fileName);
	Precache(fileName);
}

bool TextureManager::Precache( std::string fileName )
{
	bool success = false;
	std::string origname = fileName;
	std::shared_ptr<sf::Texture> t(new sf::Texture);

	if(texmap.find(fileName) != texmap.end())
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
		Console->LogMessage("TextureManager::Precache() -- Could not load texture '%s'", fileName);
	}

	Resource_t<sf::Texture> resrc;
	resrc.ptr = t;
	resrc.file_name = GetStringFilename(fileName);
	resrc.file_dir = GetStringDirectory(fileName);

	this->texmap.insert(std::pair<std::string, Resource_t<sf::Texture>>(origname, resrc));

	return true;
}

std::shared_ptr<sf::Texture> TextureManager::GetTexture(std::string name)
{
	if(texmap.find(name) == texmap.end())
		Precache(name);
		//if(!Precache(name))
		//	return nullptr;

	return texmap[name].ptr;
}

void TextureManager::Acquire( sf::Texture &tex, std::string fileName )
{
	Resource_t<sf::Texture> resrc;
	resrc.ptr = std::shared_ptr<sf::Texture>(new sf::Texture(tex));
	resrc.file_name = GetStringFilename(fileName);
	resrc.file_dir = GetStringDirectory(fileName);
	this->texmap.insert(std::pair<std::string, Resource_t<sf::Texture>>(resrc.file_name, resrc));
}

void TextureManager::Remap(const std::string &name, const std::string &newname)
{
	if(name == newname)
		return;
	texmap[newname] = texmap[name];
	texmap.erase(name);
}

void TextureManager::CreateEmptyTexture(const std::string &texName)
{
	Resource_t<sf::Texture> resrc;
	resrc.ptr = std::shared_ptr<sf::Texture>(new sf::Texture());
	resrc.file_name = texName;
	resrc.file_dir = "";
	this->texmap.insert(std::pair<std::string, Resource_t<sf::Texture>>(texName, resrc));
}
