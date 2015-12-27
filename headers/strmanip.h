#pragma once

struct splitstring_t
{
	std::string first;
	std::string second;

	void ClearMe() { first = ""; second = ""; }
	std::string GetPre() { return first; }
	std::string GetPost() { return second; }
	std::string GetFirst() { return GetPre(); }
	std::string GetSecond() { return GetPost(); }
};

std::wstring StringToWideString(const std::string &s);
std::string GetStringExtension(std::string &str);
std::string StripStringExtension(std::string &str);
std::string StripStringDirectory(std::string &str);
std::string GetStringDirectory(std::string &str);
std::string StripStringFilename(std::string &str);
std::string GetStringFilename(std::string &str);
std::string StripOuterQuotes(std::string &str);
std::string RemoveWhiteSpace(const std::string &str);
bool HasExtension(std::string &str);
bool HasDirectory(std::string &str);
std::string FormatString(std::string pmsg, ...);
std::string FormatString(std::string pmsg, va_list vlz);

// blah blah using the wrong names for class functions
// I'd like to see you not get annoyed with typing out
// StringManipulator every time you seek to use it >:(

// Therefore -- Scanner: reads and manipulates string data
// in various ways
class Scanner
{
public:
	Scanner() {}
	Scanner(const char *mem) { ret_str = mem; temp = new char[ret_str.length() + 1]; strcpy(temp, mem); }
	Scanner(const std::string mem) { ret_str = mem; temp = new char[mem.length() + 1]; strcpy(temp, mem.c_str()); }
	~Scanner() { delete[] temp; }

	inline int GetPostPosition() const { return currentpos + wordlength; }
	inline int GetPosition() const { return currentpos; }

	inline std::string GetResult() const { return ret_str; }
	inline std::string GetOriginal() const { std::string ret = temp; return ret; }

	inline void SetString(const char *mem) { if(temp) { delete[] temp; temp = nullptr; } ret_str = mem; temp = new char[ret_str.length() + 1]; strcpy(temp, mem); split.ClearMe(); }

	splitstring_t SplitAtPosition(int position);

	bool FindWord(std::string word);
	bool FindWordReverse(std::string word);

	void ReplaceWord(std::string word, std::string replacement);
	void ReplaceWordReverse(std::string word, std::string replacement);

	int ReplaceAll(std::string word, std::string replacement);

	void EraseAfter(int position);
	void EraseUntil(int position);

	inline void SetCaseInsensitive() { caseinsensitive = true; }
	inline void UnsetCaseInsensitive() { caseinsensitive = false; }
	inline bool IsCaseInsensitive() const { return caseinsensitive; }

private:
	bool caseinsensitive;

	char *temp;				// Holds an unmodified copy of what we passed in
	std::string ret_str;	// Contains the modified string result
	splitstring_t split;	// Used only if Split*() is called
	int pos;				// temporary pos storage for find, etc.


	int currentpos;
	int wordlength;
};