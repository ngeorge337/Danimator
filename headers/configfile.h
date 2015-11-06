#pragma once

class ConfigFile : public File
{
public:
	ConfigFile(std::string filename, eFileMode _mode)
		: File(filename)
	{
		configdir = "./config/";
		Open(filename, _mode);
	}
	//~ConfigFile() { Close(); }

	void WriteConfig();
	void ReadConfig();
	void WriteDefaultConfig();

	void Add(std::string data) { config += data; }
	void Clear() { config = ""; }

	std::string GetConfig() { return config; }

private:
	virtual bool Open(std::string name, eFileMode md);
	//ConfigFile();

	std::string config;
	std::string configdir;
};