#include "libs.h"
#include "util.h"
#include "static.h"
#include "token.h"
#include "strmanip.h"
#include "file.h"
#include "console.h"

#ifdef __BIG_ENDIAN__
static inline WORD SWAP_WORD(WORD x) { return x; }
static inline DWORD SWAP_DWORD(DWORD x) { return x; }
static inline QWORD SWAP_QWORD(QWORD x) { return x; }
static inline void SWAP_FLOAT(float x) { }
static inline void SWAP_DOUBLE(double &dst, double src) { dst = src; }
#else
#ifdef _MSC_VER
static inline WORD  SWAP_WORD(WORD x)		{ return _byteswap_ushort(x); }
static inline DWORD SWAP_DWORD(DWORD x)		{ return _byteswap_ulong(x); }
static inline QWORD SWAP_QWORD(QWORD x)		{ return _byteswap_uint64(x); }
static inline void SWAP_DOUBLE(double &dst, double &src)
{
	union twiddle { QWORD q; double d; } tdst, tsrc;
	tsrc.d = src;
	tdst.q = _byteswap_uint64(tsrc.q);
	dst = tdst.d;
}
#else
static inline WORD  SWAP_WORD(WORD x)		{ return (((x)<<8) | ((x)>>8)); }
static inline DWORD SWAP_DWORD(DWORD x)		{ return x = (((x)>>24) | (((x)>>8)&0xff00) | (((x)<<8)&0xff0000) | ((x)<<24)); }
static inline QWORD SWAP_QWORD(QWORD x)
{
	union { QWORD q; DWORD d[2]; } t, u;
	t.q = x;
	u.d[0] = SWAP_DWORD(t.d[1]);
	u.d[1] = SWAP_DWORD(t.d[0]);
	return u.q;
}
static inline void SWAP_DOUBLE(double &dst, double &src)
{
	union twiddle { double f; DWORD d[2]; } tdst, tsrc;
	DWORD t;

	tsrc.f = src;
	t = tsrc.d[0];
	tdst.d[0] = SWAP_DWORD(tsrc.d[1]);
	tdst.d[1] = SWAP_DWORD(t);
	dst = tdst.f;
}
#endif
static inline void SWAP_FLOAT(float &x)
{
	union twiddle { DWORD i; float f; } t;
	t.f = x;
	t.i = SWAP_DWORD(t.i);
	x = t.f;
}
#endif

DIR *FileSystem::dir = nullptr;
dirent *FileSystem::ent = nullptr;
std::string FileSystem::dirnamecopy = "";
std::stack<DIR *> FileSystem::m_directorystack = std::stack<DIR *>();

FileSystem::FileSystem()
{
	ent = nullptr;
	dir = nullptr;
	dirnamecopy = "";
}

FileSystem::~FileSystem()
{
	CloseDir();
}

bool FileSystem::OpenDir(std::string dirname)
{
	CloseDir();

	if(dirname.length() == 0) dirname = "./";
	dir = opendir(dirname.c_str());

	if (dir != NULL)
	{
		if(dirname.back() != '/')
			dirname += "/";

		dirnamecopy = dirname;
		return true;
	}
	else
	{
		/* Could not open directory */
		Console->LogMessage("FileSystem::OpenDir() -- Could not open directory '%s'", dirname);
		return false;
	}
}

std::string FileSystem::ListDirRecursive()
{
	if(!IsDirOpen())
	{
		Console->LogMessage("FileSystem::ListDirRecursive() -- No current directory open");
		return "";
	}

	rewinddir(dir);
	std::string retstr;
	char buffer[PATH_MAX + 2];
	char *p = buffer;
	const char *src;
	char *end = &buffer[PATH_MAX];

	src = dirnamecopy.c_str();
	while (p < end  &&  *src != '\0')
	{
		*p++ = *src++;
	}
	*p = '\0';

	/* Print all files and directories within the directory */
	while((ent = readdir(dir)) != NULL)
	{
		char *q = p;
		char c;

		if(buffer < q)
		{
			c = q[-1];
		}
		else
		{
			c = ':';
		}

		/* Append directory separator if not already there */
		if(c != ':'  &&  c != '/'  &&  c != '\\')
		{
			*q++ = '/';
		}

		/* Append file name */
		src = ent->d_name;
		while(q < end  &&  *src != '\0')
		{
			*q++ = *src++;
		}
		*q = '\0';

		/* Decide what to do with the directory entry */
		switch(ent->d_type)
		{
		case DT_REG:
			retstr += ent->d_name;
			retstr += "\n";
			break;

		case DT_DIR:
			/* Scan sub-directory recursively */
			if(strcmp(ent->d_name, ".") != 0 && strcmp (ent->d_name, "..") != 0)
			{
				m_directorystack.push(dir);
				OpenDir(buffer);
				retstr += ListDir();
				retstr += '\n';
				dir = m_directorystack.top();
				m_directorystack.pop();
			}
			break;

		default:
			/* Do not device entries */
			/*NOP*/;
		}
	}

	return retstr;
}

std::string FileSystem::ListDir()
{
	if(!IsDirOpen())
	{
		Console->LogMessage("FileSystem::ListDir() -- No current directory open");
		return "";
	}

	rewinddir(dir);
	std::string retstr = "";

	/* Open directory stream */
	/* Print all files and directories within the directory */
	while((ent = readdir(dir)) != NULL)
	{
		switch(ent->d_type)
		{
		case DT_REG:
			retstr += ent->d_name;
			retstr += "\n";
			break;

		case DT_DIR:
			if(!CompNoCase(ent->d_name, ".") && !CompNoCase(ent->d_name, ".."))
			{
				retstr += ent->d_name;
				retstr += "/\n";
			}
			break;

		default:
			retstr += ent->d_name;
			retstr += "*\n";
			break;
		}
	}

	return retstr;
}

bool FileSystem::IsDirOpen()
{
	return (dir != nullptr && dir->wdirp != nullptr && dir != NULL && dir->wdirp != NULL);
}

void FileSystem::CloseDir()
{
	if(IsDirOpen())
	{
		closedir(dir);
		dir = nullptr;
	}
}

FILE *FileSystem::FindFile(std::string name, unsigned int flags, eFileMode _mode)
{
	std::string mode;
	bool found = false;

	// if a file extension isn't provided, we try to guess the extension with the first
	// matching file name we find. Useful for things like textures, dangerous for
	// some other things.
	bool noext;
	if(flags & FF_IGNOREEXTENSION)
	{
		noext = true;
		name = StripStringExtension(name);
	}
	else
		noext = !HasExtension(name);

	if(!IsDirOpen())
	{
		std::string newdir = GetStringDirectory(name);
		if(!CompNoCase(name, newdir))
			OpenDir(newdir);
		else
		{
			Console->LogMessage("FileSystem::FindFile() -- No current directory open");
			return nullptr;
		}
	}
	FILE *file = nullptr;

	switch(_mode)
	{
	case FM_READ:
		mode = "r";
		break;

	case FM_READ_BINARY:
		mode = "rb";
		break;

	case FM_WRITE:
		mode = "w";
		break;

	case FM_WRITE_BINARY:
		mode = "wb";
		break;

	default:
		Console->LogMessage("FileSystem::FindFile() -- Unknown file mode");
		return nullptr;
		break;
	}

	rewinddir(dir);
	std::string tmp;

	while((ent = readdir(dir)) != NULL && !found)
	{
		switch(ent->d_type)
		{
		case DT_REG:
			if(flags & FF_CASESENSITIVE)
			{
				std::string tempname = ent->d_name;
				if(noext)
					name += GetStringExtension(tempname);
				if(TextCompare(name, tempname))
				{
					tmp = dirnamecopy + ent->d_name;
					file = fopen(tmp.c_str(), mode.c_str());
					if(file == NULL)
					{
						Console->LogMessage("FileSystem::FindFile() -- Cannot open file '%s'", name);
						return nullptr;
					}
					found = true;
				}
				if(noext)
					name = StripStringExtension(name);
			}
			else
			{
				if(noext)
					name += GetStringExtension(std::string(ent->d_name));
				if(CompNoCase(name, ent->d_name))
				{
					tmp = dirnamecopy + ent->d_name;
					file = fopen(tmp.c_str(), mode.c_str());
					if(file == NULL)
					{
						Console->LogMessage("FileSystem::FindFile() -- Cannot open file '%s'", name);
						return nullptr;
					}
					found = true;
				}
				if(noext)
					name = StripStringExtension(name);
			}
			break;

		default:
			break;
		}
	}

	rewinddir(dir);
	return file;
}

FILE *FileSystem::FindFileRecursive(std::string name, unsigned int flags, eFileMode _mode)
{
	std::string mode;
	bool found = false;

	// if a file extension isn't provided, we try to guess the extension with the first
	// matching file name we find. Useful for things like textures, dangerous for
	// some other things.
	bool noext;
	if(flags & FF_IGNOREEXTENSION)
	{
		noext = true;
		name = StripStringExtension(name);
	}
	else
		noext = !HasExtension(name);

	if(!IsDirOpen())
	{
		std::string newdir = GetStringDirectory(name);
		if(!CompNoCase(name, newdir))
			OpenDir(newdir);
		else
		{
			Console->LogMessage("FileSystem::FindFileRecursive() -- No current directory open");
			return nullptr;
		}
	}
	FILE *file = nullptr;

	switch(_mode)
	{
	case FM_READ:
		mode = "r";
		break;

	case FM_READ_BINARY:
		mode = "rb";
		break;

	case FM_WRITE:
		mode = "w";
		break;

	case FM_WRITE_BINARY:
		mode = "wb";
		break;

	default:
		Console->LogMessage("FileSystem::FindFileRecursive() -- Unknown file mode");
		return nullptr;
		break;
	}

	rewinddir(dir);
	std::string tmp;

	while((ent = readdir(dir)) != NULL && !found)
	{
		switch(ent->d_type)
		{
		case DT_REG:
			if(flags & FF_CASESENSITIVE)
			{
				std::string tempname = ent->d_name;
				if(noext)
					name += GetStringExtension(tempname);
				if(CompCase(name, tempname))
				{
					tmp = dirnamecopy + ent->d_name;
					file = fopen(tmp.c_str(), mode.c_str());
					if(file == NULL)
					{
						Console->LogMessage("FileSystem::FindFileRecursive() -- Cannot open file '%s'", name);
						return nullptr;
					}
					found = true;
				}
				if(noext)
					name = StripStringExtension(name);
			}
			else
			{
				if(noext)
					name += GetStringExtension(std::string(ent->d_name));
				if(CompNoCase(name, ent->d_name))
				{
					tmp = dirnamecopy + ent->d_name;
					file = fopen(tmp.c_str(), mode.c_str());
					if(file == NULL)
					{
						Console->LogMessage("FileSystem::FindFileRecursive() -- Cannot open file '%s'", name);
						return nullptr;
					}
					found = true;
				}
				if(noext)
					name = StripStringExtension(name);
			}
			break;

		case DT_DIR:
			/* Scan sub-directory recursively */
			if(strcmp(ent->d_name, ".") != 0 && strcmp (ent->d_name, "..") != 0)
			{
				m_directorystack.push(dir);
				OpenDir(ent->d_name);
				file = FindFileRecursive(name, flags, _mode);
				dir = m_directorystack.top();
				m_directorystack.pop();
				
				if(file != nullptr)
				{
					found = true;
					return file;
				}
			}
			break;

		default:
			break;
		}
	}

	rewinddir(dir);
	return file;
}

std::string FileSystem::GetFileLocation(std::string name, int flags)
{
	std::string fulldir = "";
	bool found = false;

	// if a file extension isn't provided, we try to guess the extension with the first
	// matching file name we find. Useful for things like textures, dangerous for
	// some other things.
	bool noext;
	if(flags & FF_IGNOREEXTENSION)
	{
		noext = true;
		name = StripStringExtension(name);
	}
	else
		noext = !HasExtension(name);

	if(!IsDirOpen())
	{
		if(!OpenDir("./"))
		{
			Console->LogMessage("FileSystem::GetFileLocation() -- No current directory open");
			return "";
		}
	}

	rewinddir(dir);

	while((ent = readdir(dir)) != NULL && !found)
	{
		switch(ent->d_type)
		{
		case DT_REG:
			if(flags & FF_CASESENSITIVE)
			{
				std::string tempname = ent->d_name;
				if(noext)
					name += GetStringExtension(tempname);
				if(TextCompare(name, tempname))
				{
					if(flags & FF_RETURNFULLPATH)
					{
						char buf[256];
						getcwd(buf, sizeof(buf));
						std::string longdir = buf;
						Scanner sc(dirnamecopy);
						splitstring_t split;
						sc.ReplaceAll("/", "\\");
						sc.FindWord(".");
						split = sc.SplitAtPosition(sc.GetPostPosition());
						fulldir = longdir + split.GetPost() + ent->d_name;
					}
					else
						fulldir = dirnamecopy + ent->d_name;
					found = true;
				}
				if(noext)
					name = StripStringExtension(name);
			}
			else
			{
				if(noext)
					name += GetStringExtension(std::string(ent->d_name));
				if(CompNoCase(name, ent->d_name))
				{
					if(flags & FF_RETURNFULLPATH)
					{
						char buf[256];
						getcwd(buf, sizeof(buf));
						std::string longdir = buf;
						Scanner sc(dirnamecopy);
						splitstring_t split;
						sc.ReplaceAll("/", "\\");
						sc.FindWord(".");
						split = sc.SplitAtPosition(sc.GetPostPosition());
						fulldir = longdir + split.GetPost() + ent->d_name;
					}
					else
						fulldir = dirnamecopy + ent->d_name;
					found = true;
				}
				if(noext)
					name = StripStringExtension(name);
			}
			break;

		default:
			break;
		}
	}

	if(!found)
		Console->LogMessage("FileSystem::GetFileLocation() -- Cannot get location for file '%s'", name);

	rewinddir(dir);
	return fulldir;
}

//==============================================================
//
// Proc: FileSystem::GetFileLocationRecursive
//
// Purpose: Returns a relative directory string leading to
//			a file's location
//
//==============================================================
std::string FileSystem::GetFileLocationRecursive(std::string name, int flags)
{
	std::string fulldir = "";
	bool found = false;

	// if a file extension isn't provided, we try to guess the extension with the first
	// matching file name we find. Useful for things like textures, dangerous for
	// some other things.
	bool noext;
	if(flags & FF_IGNOREEXTENSION)
	{
		noext = true;
		name = StripStringExtension(name);
	}
	else
		noext = !HasExtension(name);

	if(!IsDirOpen())
	{
		if(!OpenDir("./"))
		{
			Console->LogMessage("FileSystem::GetFileLocationRecursive() -- No current directory open");
			return "";
		}
	}

	if(HasDirectory(name))
		name = StripStringDirectory(name);

	rewinddir(dir);

	while((ent = readdir(dir)) != NULL && !found)
	{
		switch(ent->d_type)
		{
		case DT_REG:
			if(flags & FF_CASESENSITIVE)
			{
				std::string tempname = ent->d_name;
				if(noext)
					name += GetStringExtension(tempname);
				if(TextCompare(name, tempname))
				{
					if(flags & FF_RETURNFULLPATH)
					{
						char buf[256];
						getcwd(buf, sizeof(buf));
						std::string longdir = buf;
						Scanner sc(dirnamecopy);
						splitstring_t split;
						sc.ReplaceAll("/", "\\");
						sc.FindWord(".");
						split = sc.SplitAtPosition(sc.GetPostPosition());
						fulldir = longdir + split.GetPost() + ent->d_name;
					}
					else
						fulldir = dirnamecopy + ent->d_name;
					found = true;
				}
				if(noext)
					name = StripStringExtension(name);
			}
			else
			{
				if(noext)
					name += GetStringExtension(std::string(ent->d_name));
				if(CompNoCase(name, ent->d_name))
				{
					if(flags & FF_RETURNFULLPATH)
					{
						char buf[256];
						getcwd(buf, sizeof(buf));
						std::string longdir = buf;
						Scanner sc(dirnamecopy);
						splitstring_t split;
						sc.ReplaceAll("/", "\\");
						sc.FindWord(".");
						split = sc.SplitAtPosition(sc.GetPostPosition());
						fulldir = longdir + split.GetPost() + ent->d_name;
					}
					else
						fulldir = dirnamecopy + ent->d_name;
					found = true;
				}
				if(noext)
					name = StripStringExtension(name);
			}
			break;

		case DT_DIR:
			/* Scan sub-directory recursively */
			if(strcmp(ent->d_name, ".") != 0 && strcmp (ent->d_name, "..") != 0)
			{
				m_directorystack.push(dir);
				OpenDir(ent->d_name);
				fulldir = GetFileLocationRecursive(name);
				dir = m_directorystack.top();
				m_directorystack.pop();

				if(!fulldir.empty())
				{
					found = true;
				}
			}
			break;

		default:
			break;
		}
	}

	if(!found)
		Console->LogMessage("FileSystem::GetFileLocationRecursive() -- Cannot get location for file '%s'", name);

	rewinddir(dir);
	return fulldir;
}

FILE *FileSystem::CreateFileLegacy(std::string name, eFileMode _mode)
{
	std::string mode;
	std::string target;
	if(!IsDirOpen())
	{
		if(!OpenDir("./"))
		{
			Console->LogMessage("FileSystem::CreateFile() -- No current directory open");
			return nullptr;
		}
	}
	FILE *file = nullptr;
	target = dirnamecopy + name;

	switch(_mode)
	{
	case FM_APPEND:
		mode = "a";
		break;

	case FM_APPEND_WRITE:
		file = fopen(target.c_str(), "w");
		fclose(file);
		file = nullptr;
		mode = "a";
		break;

	case FM_WRITE:
		mode = "w";
		break;

	case FM_WRITE_BINARY:
		mode = "wb";
		break;

	default:
		Console->LogMessage("FileSystem::CreateFile() -- Invalid writing mode");
		return nullptr;
		break;
	}

	file = fopen(target.c_str(), mode.c_str());

	if(file == NULL)
	{
		Console->LogMessage("FileSystem::CreateFile() -- Cannot create file '%s'", target);
		return nullptr;
	}

	return file;
}

File FileSystem::CreateNewFile(std::string name, eFileMode _mode)
{
	std::string mode;
	std::string target;
	if(!IsDirOpen())
	{
		if(!OpenDir("./"))
		{
			Console->LogMessage("FileSystem::CreateFile() -- No current directory open");
			return nullptr;
		}
	}
	File file;
	target = dirnamecopy + name;

	assert(!(_mode == FM_APPEND || _mode == FM_APPEND_WRITE));

	switch(_mode)
	{
	case FM_WRITE:
		mode = "w";
		break;

	case FM_WRITE_BINARY:
		mode = "wb";
		break;

	default:
		Console->LogMessage("FileSystem::CreateFile() -- Invalid writing mode");
		return nullptr;
		break;
	}

	if(!file.Create(target, _mode))
	{
		Console->LogMessage("FileSystem::CreateFile() -- Cannot create file '%s'", target);
		return nullptr;
	}

	return file;
}

bool FileSystem::CreateDir(std::string dirname)
{
	if(FindDir(dirname))
		return true;

	else
	{
#if _WIN32
		std::wstring w = StringToWideString(dirname);
		LPCWSTR temp =  w.c_str();
		return CreateDirectory( temp, NULL);
#endif
	}
}

bool FileSystem::FindDir(std::string dirname)
{
	FileSystem tfs;
	tfs.OpenDir(dirname);

	if(!tfs.IsDirOpen())
	{
		CloseDir();
		return false;
	}

	else
	{
		CloseDir();
		return true;
	}
}

bool FileSystem::FileExists( std::string name, int flags /*= 0*/ )
{
	bool found = false;

	// if a file extension isn't provided, we try to guess the extension with the first
	// matching file name we find. Useful for things like textures, dangerous for
	// some other things.
	bool noext;
	if(flags & FF_IGNOREEXTENSION)
	{
		noext = true;
		name = StripStringExtension(name);
	}
	else
		noext = !HasExtension(name);

	if(!IsDirOpen())
	{
		if(!OpenDir("./"))
		{
			Console->LogMessage("FileSystem::FileExists() -- No current directory open");
			return "";
		}
	}

	rewinddir(dir);

	while((ent = readdir(dir)) != NULL && !found)
	{
		switch(ent->d_type)
		{
		case DT_REG:
			if(flags & FF_CASESENSITIVE)
			{
				std::string tempname = ent->d_name;
				if(noext)
					name += GetStringExtension(tempname);
				if(TextCompare(name, tempname))
				{
					found = true;
				}
				if(noext)
					name = StripStringExtension(name);
			}
			else
			{
				if(noext)
					name += GetStringExtension(std::string(ent->d_name));
				if(CompNoCase(name, ent->d_name))
				{
					found = true;
				}
				if(noext)
					name = StripStringExtension(name);
			}
			break;

		default:
			break;
		}
	}

	if(!found)
		Console->LogMessage("FileSystem::FileExists() -- Cannot locate file '%s'", name);

	rewinddir(dir);
	return found;
}

File::File(std::string name)
{
	src = NULL;
	//fs = &rfs;
	//fs = Engine->filesystem;
	fname = name;
	if(HasExtension(name))
	{
		std::string temp = name;
		splitstring_t _ext;
		Scanner sc(temp.c_str());
		sc.FindWordReverse(".");
		_ext = sc.SplitAtPosition(sc.GetPosition());
		ext = _ext.GetSecond();
	}
	else ext = "";
}

File::File()
{
	src = NULL;
	//fs = Engine->filesystem;
}

bool File::Open( std::string name, eFileMode md /*= FM_READ*/ )
{
	mode = md;
	src = FileSystem::FindFile(name, FF_NONE, md);
	fpath = FileSystem::GetFileLocation(name, FF_NONE);
	fpath = StripStringFilename(fpath);
	fname = name;
	if(src == nullptr)
	{
		Console->LogMessage("Unable to open file '%s'", name);
		return false;
	}

	return true;
}

std::string File::ReadText()
{
	if(!IsOpen())
		return "";

	std::string r;

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

	r = buffer;

	delete[] buffer;
	return r;
}

int File::ReadBytes(void *destBuf, size_t byteCount)
{
	if(!IsOpen())
		return 0;

	if(!(mode == FM_READ || mode == FM_READ_BINARY))
		return 0;

	int nRead = fread(destBuf, sizeof(unsigned char), byteCount, src);
	

	return nRead;
}

void File::WriteBytes(const void *iData, size_t length)
{
	if(!IsOpen())
		return;

	if(!(mode == FM_WRITE || mode == FM_WRITE_BINARY))
		return;

	fwrite(iData, sizeof(unsigned char), length, src);
}

bool File::Create(const std::string &pathName, eFileMode md)
{
	mode = md;
	std::string fmode;
	if(md == FM_WRITE) fmode = "w";
	else if(md == FM_WRITE_BINARY) fmode = "wb";
	else if(md == FM_READ) fmode = "r";
	else if(md == FM_READ_BINARY) fmode = "rb";
	src = fopen(pathName.c_str(), fmode.c_str());
	fpath = pathName;
	fpath = StripStringFilename(fpath);
	std::string fullFilename = pathName;
	fname = StripStringDirectory(fullFilename);
	if(src == nullptr)
	{
		Console->LogMessage("Unable to open file '%s'", pathName);
		return false;
	}

	return true;
}

File & File::operator>>(FState_t &pData)
{
	operator>>(pData.name);
	operator>>(pData.ending);
	if(pData.ending == END_GOTO)
		operator>>(pData.gotoLabel);
	int frameCount;
	operator>>(frameCount);
	if(frameCount > 0)
	{
		for(int i = 0; i < frameCount; ++i)
		{
			Frame_t frame;
			operator>>(frame);
			pData.m_frames.push_back(frame);
		}
	}
	return *this;
}

File & File::operator>>(Frame_t &pData)
{
	operator>>(pData.spriteName);

	sf::Vector2f tempPos;
	operator>>(tempPos);
	pData.sprite.setPosition(tempPos);

	sf::Color tempColor;
	operator>>(tempColor);
	pData.sprite.setColor(tempColor);

	operator>>(pData.tics);
	operator>>(pData.duration);
	operator>>(pData.hasSound);
	if(pData.hasSound)
		operator>>(pData.soundName);
	return *this;
}

File & File::operator>>(sf::SoundBuffer &pData)
{
	int sz;
	int channelCount;
	int sampleRate;
	int sampleCount;
	sf::Int16 *samples = nullptr;
	ReadBytes(&sz, sizeof(std::size_t));
	ReadBytes(&channelCount, sizeof(int));
	ReadBytes(&sampleRate, sizeof(int));
	ReadBytes(&sampleCount, sizeof(int));
	samples = new sf::Int16[sz];
	ReadBytes(samples, sz);
	pData.loadFromSamples(samples, sampleCount, channelCount, sampleRate);
	delete[] samples;
	return *this;
}

File & File::operator>>(CompositeLayer_t &pData)
{
	operator>>(pData.layerName);
	operator>>(pData.spriteName);

	sf::Vector2f tempPos;
	operator>>(tempPos);
	pData.spr.setPosition(tempPos);

	sf::Color tempColor;
	operator>>(tempColor);
	pData.spr.setColor(tempColor);

	operator>>(pData.isVisible);
	operator>>(pData.renderStyle);
	operator>>(pData.opacityValue);
	operator>>(pData.rotValue);
	operator>>(pData.flipX);
	operator>>(pData.flipY);
	return *this;
}

File & File::operator>>(sf::Texture &pData)
{
	float imgW;
	float imgH;
	sf::Uint8 *imgData = nullptr;
	ReadBytes(&imgW, sizeof(int));
	ReadBytes(&imgH, sizeof(int));
	int len = imgW * imgH * 4;
	imgData = new sf::Uint8[len];
	ReadBytes(imgData, sizeof(sf::Uint8) * len);

	sf::Image image = sf::Image();
	image.create(imgW, imgH, imgData);
	pData.loadFromImage(image);

	delete[] imgData;
	return *this;
}

File & File::operator>>(sf::Uint16 &pData)
{
	ReadBytes(&pData, sizeof(sf::Uint16));
	return *this;
}

/*
File & File::operator>>(sf::Uint8 &pData)
{
	ReadBytes(&pData, sizeof(sf::Uint8));
	return *this;
}
*/

File & File::operator>>(sf::Color &pData)
{
	BYTE outR;
	BYTE outG;
	BYTE outB;
	BYTE outA;
	ReadBytes(&outR, sizeof(BYTE));
	ReadBytes(&outG, sizeof(BYTE));
	ReadBytes(&outB, sizeof(BYTE));
	ReadBytes(&outA, sizeof(BYTE));
	pData = sf::Color(outR, outG, outB, outA);
	return *this;
}

File & File::operator>>(sf::Vector2i &pData)
{
	ReadBytes(&pData.x, sizeof(int));
	ReadBytes(&pData.y, sizeof(int));
	return *this;
}

File & File::operator>>(sf::Vector2f &pData)
{
	ReadBytes(&pData.x, sizeof(float));
	ReadBytes(&pData.y, sizeof(float));
	return *this;
}

File & File::operator>>(float &pData)
{
	ReadBytes(&pData, sizeof(float));
	return *this;
}

File & File::operator>>(bool &pData)
{
	ReadBytes(&pData, sizeof(bool));
	return *this;
}

File & File::operator>>(BYTE &pData)
{
	ReadBytes(&pData, sizeof(BYTE));
	return *this;
}

File & File::operator>>(int &pData)
{
	ReadBytes(&pData, sizeof(int));
	return *this;
}

File & File::operator>>(std::string &pData)
{
	int len;
	ReadBytes(&len, sizeof(int));

	if(len <= 0)
		return *this;

	char *str = new char[len + 1]();
	ReadBytes(str, len);
	str[len] = '\0';
	pData = std::string(str);
	delete[] str;
	return *this;
}

File & File::operator<<(const FState_t &pData)
{
	operator<<(pData.name);
	operator<<(pData.ending);
	if(pData.ending == END_GOTO)
		operator<<(pData.gotoLabel);
	if(!pData.m_frames.empty())
	{
		operator<<(int(pData.m_frames.size()));
		for(int i = 0; i < pData.m_frames.size(); ++i)
			operator<<(pData.m_frames[i]);
	}
	return *this;
}

File & File::operator<<(const Frame_t &pData)
{
	operator<<(pData.spriteName);
	operator<<(pData.sprite.getPosition());
	operator<<(pData.sprite.getColor());
	operator<<(pData.tics);
	operator<<(pData.duration);
	operator<<(pData.hasSound);
	if(pData.hasSound)
		operator<<(pData.soundName);
	return *this;
}

File & File::operator<<(const sf::SoundBuffer &pData)
{
	int sz = sizeof(sf::Int16) * pData.getSampleCount();
	WriteBytes(&sz, sizeof(std::size_t));
	int len = pData.getChannelCount();
	WriteBytes(&len, sizeof(int));
	len = pData.getSampleRate();
	WriteBytes(&len, sizeof(int));
	len = pData.getSampleCount();
	WriteBytes(&len, sizeof(int));
	const sf::Int16 *samples = pData.getSamples();
	WriteBytes(samples, sz);
	return *this;
}

File & File::operator<<(const CompositeLayer_t &pData)
{
	operator<<(pData.layerName);
	operator<<(pData.spriteName);
	operator<<(pData.spr.getPosition());
	operator<<(pData.spr.getColor());
	operator<<(pData.isVisible);
	operator<<(pData.renderStyle);
	operator<<(pData.opacityValue);
	operator<<(pData.rotValue);
	operator<<(pData.flipX);
	operator<<(pData.flipY);
	return *this;
}

File & File::operator<<(const sf::Texture &pData)
{
	sf::Image tempimg = sf::Image();
	tempimg = pData.copyToImage();
	float imgW = pData.getSize().x;
	float imgH = pData.getSize().y;
	const sf::Uint8 *imgData = tempimg.getPixelsPtr();
	int len = pData.getSize().x * pData.getSize().y * 4;
	WriteBytes(&imgW, sizeof(int));
	WriteBytes(&imgH, sizeof(int));
	WriteBytes(imgData, sizeof(sf::Uint8) * len);
	return *this;
}

File & File::operator<<(const sf::Uint16 &pData)
{
	WriteBytes(&pData, sizeof(sf::Uint16));
	return *this;
}

/*
File & File::operator<<(const sf::Uint8 &pData)
{
	WriteBytes(&pData, sizeof(sf::Uint8));
	return *this;
}
*/

File & File::operator<<(const sf::Color &pData)
{
	BYTE outR = pData.r;
	BYTE outG = pData.g;
	BYTE outB = pData.b;
	BYTE outA = pData.a;
	WriteBytes(&outR, sizeof(BYTE));
	WriteBytes(&outG, sizeof(BYTE));
	WriteBytes(&outB, sizeof(BYTE));
	WriteBytes(&outA, sizeof(BYTE));
	return *this;
}

File & File::operator<<(const sf::Vector2i &pData)
{
	int outX = pData.x;
	int outY = pData.y;
	WriteBytes(&outX, sizeof(int));
	WriteBytes(&outY, sizeof(int));
	return *this;
}

File & File::operator<<(const sf::Vector2f &pData)
{
	float outX = pData.x;
	float outY = pData.y;
	WriteBytes(&outX, sizeof(float));
	WriteBytes(&outY, sizeof(float));
	return *this;
}

File & File::operator<<(const float &pData)
{
	WriteBytes(&pData, sizeof(float));
	return *this;
}

File & File::operator<<(const bool &pData)
{
	WriteBytes(&pData, sizeof(bool));
	return *this;
}

File & File::operator<<(const BYTE &pData)
{
	WriteBytes(&pData, sizeof(BYTE));
	return *this;
}

File & File::operator<<(const int &pData)
{
	WriteBytes(&pData, sizeof(int));
	return *this;
}

File & File::operator<<(const std::string &pData)
{
	int len;
	len = pData.length();

	if(len <= 0)
		return *this;

	WriteBytes(&len, sizeof(int));
	len = sizeof(char) * len;
	WriteBytes(pData.c_str(), len);
	return *this;
}
