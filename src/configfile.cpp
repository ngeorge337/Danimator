#include "libs.h"
#include "util.h"
#include "static.h"
#include "bimap.h"
#include "token.h"
#include "strmanip.h"
#include "file.h"
#include "configfile.h"
#include "ccmd.h"
#include "cvar.h"
#include "console.h"

bool ConfigFile::Open(std::string name, eFileMode md)
{
	mode = md;
	if(!FileSystem::OpenDir(configdir))
		FileSystem::CreateDir(configdir);
	src = FileSystem::FindFile(name, FF_NONE, md);
	//if(src == nullptr)
	if(!IsOpen())
	{
		Console->Clear();
		WriteDefaultConfig();
		src = FileSystem::FindFile(name, FF_NONE, md);
		if(src == nullptr)
		{
			Console->LogMessage("Unable to read default config file!");
			return false;
		}
	}

	ReadConfig();
	return true;
}

void ConfigFile::WriteDefaultConfig()
{
	std::string outcfg = "";

	outcfg = Cvars->ReadDefaultCvarList();
	//outcfg += GetDefaultControls();

	if(!FileSystem::OpenDir(configdir))
		FileSystem::CreateDir(configdir);
	FILE *src = FileSystem::CreateFileLegacy("config.cfg", FM_WRITE);

	if(src == nullptr)
	{
		Console->LogMessage("Unable to write default config file!");
		return;
	}

	fwrite(outcfg.c_str(), 1, outcfg.length() + 1, src);
	fclose(src);
}

void ConfigFile::ReadConfig()
{
	if(!IsOpen())
		return;

	long length = 0;
	offset = 0;
	char *buffer;

	fseek(src, 0, SEEK_END);
	long fsize = ftell(src);
	fseek(src, 0, SEEK_SET);

	// If length isn't specified or exceeds the file length,
	// only read to the end of the file
	if(offset + length > fsize || length == 0)
		length = fsize - offset;

	buffer = (char *) calloc(length, sizeof(char));

	// Read the file portion
	fseek(src, 0, SEEK_SET);
	fread(buffer, 1, length, src);

	config = buffer;

	delete[] buffer;
}

void ConfigFile::WriteConfig()
{
	FileSystem::OpenDir("./config/");
	Close();
	config.shrink_to_fit();
	src = FileSystem::FindFile("config.cfg", FF_NONE, FM_WRITE);

	if(src == nullptr)
	{
		Console->LogMessage("Unable to write to config file!");
		return;
	}

	fwrite(config.c_str(), 1, config.length(), src);
	Close();
}