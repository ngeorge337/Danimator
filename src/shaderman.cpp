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
#include "shaderman.h"

void ShaderManager::UnloadAll()
{
	shadermap.clear();
}

void ShaderManager::ReloadAll()
{
	for(auto it = shadermap.begin(); it != shadermap.end(); ++it)
	{
		Reload(it->first);
	}
}

void ShaderManager::Unload( std::string fileName )
{
	shadermap.erase(fileName);
}

void ShaderManager::Reload( std::string fileName )
{
	Unload(fileName);
	Precache(fileName);
}

bool ShaderManager::Precache( std::string fileName )
{
	std::string origname = fileName;

	if(shadermap.find(fileName) != shadermap.end())
		return true;

	if(!HasDirectory(fileName))
		fileName = m_searchDirs.front() + fileName;

	std::shared_ptr<sf::Shader> t(new sf::Shader);
	if(!t->loadFromFile(fileName, DetermineType(fileName)))
	{
		Console->LogMessage("Could not load Shader '%s'", fileName);
		return false;
	}

	Resource_t<sf::Shader> resrc;
	resrc.ptr = t;
	resrc.file_name = GetStringFilename(fileName);
	resrc.file_dir = GetStringDirectory(fileName);

	this->shadermap.insert(std::pair<std::string, Resource_t<sf::Shader>>(origname, resrc));

	return true;
}

bool ShaderManager::Precache(std::string vertexShader, std::string fragmentShader)
{
	std::string origname = vertexShader;
	std::string origname2 = fragmentShader;

	if(shadermap.find(vertexShader) != shadermap.end() && shadermap.find(fragmentShader) != shadermap.end())
		return true;

	if(!HasDirectory(vertexShader))
		vertexShader = m_searchDirs.front() + vertexShader;
	if(!HasDirectory(fragmentShader))
		fragmentShader = m_searchDirs.front() + fragmentShader;

	std::shared_ptr<sf::Shader> t(new sf::Shader);
	if(!t->loadFromFile(vertexShader, fragmentShader))
	{
		Console->LogMessage("Could not load Shader pair '%s' and '%s'", vertexShader, fragmentShader);
		return false;
	}

	std::string realname = vertexShader + ", " + fragmentShader;
	Resource_t<sf::Shader> resrc;
	resrc.ptr = t;
	resrc.file_name = GetStringFilename(vertexShader) + ", " + GetStringFilename(fragmentShader);
	resrc.file_dir = GetStringDirectory(vertexShader);

	this->shadermap.insert(std::pair<std::string, Resource_t<sf::Shader>>(realname, resrc));

	return true;
}

std::shared_ptr<sf::Shader> ShaderManager::GetShader(std::string name)
{
	if(shadermap.find(name) == shadermap.end())
		if(!Precache(name))
			return nullptr;

	return shadermap[name].ptr;
}

std::shared_ptr<sf::Shader> ShaderManager::GetShaderSet(std::string vertexShader, std::string fragmentShader)
{
	std::string realname = vertexShader + ", " + fragmentShader;
	if(shadermap.find(realname) == shadermap.end())
		if(!Precache(vertexShader, fragmentShader))
			return nullptr;

	return shadermap[realname].ptr;
}

sf::Shader::Type ShaderManager::DetermineType( std::string filename )
{
	std::string ext = GetStringExtension(filename);
	if(CompNoCase(ext, ".vert"))
		return sf::Shader::Vertex;
	else if(CompNoCase(ext, ".frag"))
		return sf::Shader::Fragment;
	else
	{
		Console->LogMessage("Unable to determine shader type in '%s'", filename);
		return sf::Shader::Fragment;
	}
}
