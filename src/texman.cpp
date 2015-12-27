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

	for(auto itx = compmap.begin(); itx != compmap.end(); ++itx)
	{
		itx->second->GetFinalTexture()->setSmooth(enable);
	}
}

void TextureManager::UnloadAll()
{
	texmap.clear();
	compmap.clear();
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
	if(texmap.find(fileName) != texmap.end())
		texmap.erase(fileName);
	else if(compmap.find(fileName) != compmap.end())
		compmap.erase(fileName);
	
	// Recompile all current texture images in case an image is now missing
	if(!compmap.empty())
	{
		for(auto it = compmap.begin(); it != compmap.end(); ++it)
		{
			it->second->CreateComposite();
		}
	}
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
			t->setSmooth(r_bilinear);
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
	// Search composite textures first, since we don't precache those as they are user-generated
	if(compmap.find(name) != compmap.end())
		return compmap[name]->GetFinalTexture();

	else if(texmap.find(name) == texmap.end())
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
	if(name == newname || name.empty() || newname.empty())
		return;
	if(compmap.find(name) != compmap.end())
	{
		compmap[newname] = compmap[name];
		compmap.erase(name);
	}
	else if(texmap.find(name) != texmap.end())
	{
		texmap[newname] = texmap[name];
		texmap.erase(name);
	}
}

void TextureManager::CreateEmptyTexture(const std::string &texName)
{
	Resource_t<sf::Texture> resrc;
	resrc.ptr = std::shared_ptr<sf::Texture>(new sf::Texture());
	resrc.file_name = texName;
	resrc.file_dir = "";
	this->texmap.insert(std::pair<std::string, Resource_t<sf::Texture>>(texName, resrc));
}

std::shared_ptr<CompositeTexture> TextureManager::GetCompositeTexture(const std::string &name)
{
	if(compmap.find(name) == compmap.end())
		compmap.insert(std::pair<std::string, std::shared_ptr<CompositeTexture>>(name, std::shared_ptr<CompositeTexture>(new CompositeTexture)));

	return compmap[name];
}

bool TextureManager::IsComposite(const std::string &name)
{
	return compmap.find(name) != compmap.end();
}

void TextureManager::Duplicate(const std::string &name, const std::string &dupname)
{
	if(name == dupname || name.empty() || dupname.empty())
		return;

	if(compmap.find(name) != compmap.end())
	{
		std::shared_ptr<CompositeTexture> ct = std::shared_ptr<CompositeTexture>(new CompositeTexture(CompositeTexture(*compmap[name])));
		ct->CreateComposite();
		this->compmap.insert(std::pair<std::string, std::shared_ptr<CompositeTexture>>(dupname, ct));
	}
	else if(texmap.find(name) != texmap.end())
	{
		Resource_t<sf::Texture> resrc;
		resrc.file_name = dupname;
		resrc.file_dir = "";
		resrc.ptr = std::shared_ptr<sf::Texture>(new sf::Texture(*texmap[name].ptr));
		this->texmap.insert(std::pair<std::string, Resource_t<sf::Texture>>(dupname, resrc));
	}
	else
		return;
}
