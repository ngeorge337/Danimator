#pragma once

class ShaderManager : public ResourceManager
{
public:
	ShaderManager(std::string directory) : ResourceManager(directory)
	{
	}
	virtual bool Precache(std::string fileName);
	virtual bool Precache(std::string vertexShader, std::string fragmentShader);
	virtual void Reload(std::string fileName);
	virtual void Unload(std::string fileName);
	virtual void ReloadAll();
	virtual void UnloadAll();
	std::shared_ptr<sf::Shader> GetShader(std::string name);
	std::shared_ptr<sf::Shader> GetShaderSet(std::string vertexShader, std::string fragmentShader);
private:
	sf::Shader::Type DetermineType(std::string filename);
	std::map<std::string, Resource_t<sf::Shader>> shadermap;
};