#pragma once

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
	static FILE *CreateFile(std::string name, eFileMode _mode);

	static std::string GetFileLocation(std::string name, int flags = FF_NONE);	// returns a full directory string to a file, ex "./textures/mytexture.png"
	static std::string GetFileLocationRecursive(std::string name, int flags = FF_NONE);

private:
	static DIR *dir;
	static struct dirent *ent;
	static std::string dirnamecopy;

	static std::stack<DIR *> m_directorystack;
};

class File
{
public:
	File(std::string name);
	File();
	~File() { Close(); }

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

	void SuggestExtension(std::string extension) { ext = extension; }
	std::string GetExtension() const { return ext; }
	std::string GetFileName() const { return fname; }
	std::string GetPath() const { return fpath; }
	inline void SetMode(eFileMode m) { mode = m; }
	std::string ReadFile();

protected:
	FILE *src;
	eFileMode mode;
	eSeekType seektype;
	std::string ext;
	std::string fname;
	std::string fpath;

	long offset;
};