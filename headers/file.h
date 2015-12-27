#pragma once

#include "animator.h"

enum eFindFileFlags
{
	FF_NONE = 0,
	FF_CASESENSITIVE = 1,
	FF_IGNOREEXTENSION = 2,
	FF_RETURNFULLPATH = 4,
};

enum eFileMode
{
	FM_READ,
	FM_WRITE,
	FM_READ_BINARY,
	FM_WRITE_BINARY,
	FM_APPEND,
	FM_APPEND_WRITE,
	FM_NOTOPEN,
};

class File
{
public:
	File(std::string name);
	File();
	~File() { if(buffer) delete[] buffer; buffer = NULL; Close(); }

	File &operator <<(const std::string &pData);
	File &operator <<(const int &pData);
	File &operator <<(const BYTE &pData);
	File &operator <<(const bool &pData);
	File &operator <<(const float &pData);
	File &operator <<(const sf::Vector2f &pData);
	File &operator <<(const sf::Vector2i &pData);
	File &operator <<(const sf::Color &pData);
	//File &operator <<(const sf::Uint8 &pData);
	File &operator <<(const sf::Uint16 &pData);
	File &operator <<(const sf::Texture &pData);
	File &operator <<(const CompositeLayer_t &pData);
	File &operator <<(const sf::SoundBuffer &pData);
	File &operator <<(const Frame_t &pData);
	File &operator <<(const FState_t &pData);

	File &operator >>(std::string &pData);
	File &operator >>(int &pData);
	File &operator >>(BYTE &pData);
	File &operator >>(bool &pData);
	File &operator >>(float &pData);
	File &operator >>(sf::Vector2f &pData);
	File &operator >>(sf::Vector2i &pData);
	File &operator >>(sf::Color &pData);
	//File &operator >>(sf::Uint8 &pData);
	File &operator >>(sf::Uint16 &pData);
	File &operator >>(sf::Texture &pData);
	File &operator >>(CompositeLayer_t &pData);
	File &operator >>(sf::SoundBuffer &pData);
	File &operator >>(Frame_t &pData);
	File &operator >>(FState_t &pData);

	enum eSeekType
	{
		SeekSet,
		SeekRelative,
		SeekEnd
	};

	virtual bool Open(std::string name, eFileMode md = FM_READ);
	virtual void Close() { if(IsOpen()) fclose(src); }
	virtual void Flush() { if(IsOpen()) fflush(src); }
	bool IsOpen() { return src != NULL; }

	bool Create(const std::string &pathName, eFileMode md = FM_WRITE);

	int GetVersion() const
	{
		return m_fileVersion;
	}
	void SetVersion(int v)
	{
		m_fileVersion = v;
	}

	bool IsEOF() const
	{
		return feof(src) == 0 ? false : true;
	}

	void SuggestExtension(std::string extension) { ext = extension; }
	std::string GetExtension() const { return ext; }
	std::string GetFileName() const { return fname; }
	std::string GetPath() const { return fpath; }
	inline void SetMode(eFileMode m) { mode = m; }
	std::string ReadText();

	int ReadBytes(void *destBuf, size_t byteCount);

	void WriteBytes(const void *iData, size_t length);

protected:
	FILE *src;
	eFileMode mode;
	eSeekType seektype;
	std::string ext;
	std::string fname;
	std::string fpath;

	int m_fileVersion;

	char *buffer;

	long offset;
};

class FileSystem
{
public:
	FileSystem();
	~FileSystem();

	static bool OpenDir(std::string dirname);
	static bool FindDir(std::string dirname);
	static bool IsDirOpen();
	static void CloseDir();
	static bool CreateDir(std::string dirname);

	static std::string ListDir();
	static std::string ListDirRecursive();
	static bool FileExists(std::string name, int flags = 0);
	static FILE *FindFile(std::string name, unsigned int flags, eFileMode _mode);
	static FILE *FindFileRecursive(std::string name, unsigned int flags, eFileMode _mode);
	static FILE *CreateFileLegacy(std::string name, eFileMode _mode);
	static File CreateNewFile(std::string name, eFileMode _mode);

	static std::string GetFileLocation(std::string name, int flags = FF_NONE);	// returns a full directory string to a file, ex "./textures/mytexture.png"
	static std::string GetFileLocationRecursive(std::string name, int flags = FF_NONE);

private:
	static DIR *dir;
	static struct dirent *ent;
	static std::string dirnamecopy;

	static std::stack<DIR *> m_directorystack;
};