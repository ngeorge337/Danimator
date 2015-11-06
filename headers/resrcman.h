#pragma once

template<typename T>
struct Resource_t
{
	std::shared_ptr<T> ptr;
	std::string file_dir;
	std::string file_name;
};

class ResourceManager
{
public:
	ResourceManager(std::string _defaultDir = "")
	{
		AddSearchDirectory(_defaultDir);
	}
	virtual bool Precache(std::string fileName) = 0;
	virtual void Reload(std::string fileName) = 0;
	virtual void Unload(std::string fileName) = 0;
	virtual void ReloadAll() = 0;
	virtual void UnloadAll() = 0;
	virtual void AddSearchDirectory(std::string dir)
	{
		if(!dir.empty())
		{
			if(dir[dir.length() - 1] != '/')
				dir = dir + '/';
			m_searchDirs.push_back(dir);
		}
	}

protected:
	std::vector<std::string> m_searchDirs;
};