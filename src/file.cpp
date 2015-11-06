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

FILE *FileSystem::CreateFile(std::string name, eFileMode _mode)
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
	src = nullptr;
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
	src = nullptr;
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

std::string File::ReadFile()
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
