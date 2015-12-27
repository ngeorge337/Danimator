#include "libs.h"
#include "token.h"
#include "strmanip.h"

std::wstring StringToWideString(const std::string &s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::string GetStringExtension(std::string &str)
{
	std::string temp = str;
	splitstring_t _ext;
	Scanner sc(temp.c_str());
	if(!sc.FindWordReverse("."))
		return "";
	_ext = sc.SplitAtPosition(sc.GetPosition());
	return _ext.GetSecond();
}

std::string StripStringExtension(std::string &str)
{
	std::string temp = str;
	splitstring_t _ext;
	Scanner sc(temp.c_str());
	if(!sc.FindWordReverse("."))
		return str;

	_ext = sc.SplitAtPosition(sc.GetPosition());
	return _ext.GetFirst();
}

std::string StripStringDirectory(std::string &str)
{
	std::string temp = str;
	splitstring_t _ext;
	Scanner sc(temp.c_str());
	if(!sc.FindWordReverse("/"))
		if(!sc.FindWordReverse("\\"))
			return str;

	_ext = sc.SplitAtPosition(sc.GetPostPosition());
	return _ext.GetSecond();
}

std::string GetStringDirectory(std::string &str)
{
	std::string temp = str;
	splitstring_t _ext;
	Scanner sc(temp.c_str());
	if(!sc.FindWordReverse("/"))
		if(!sc.FindWordReverse("\\"))
			return "";

	_ext = sc.SplitAtPosition(sc.GetPostPosition());
	return _ext.GetFirst();
}

std::string StripStringFilename(std::string &str)
{
	std::string temp = str;
	splitstring_t _ext;
	Scanner sc(temp.c_str());
	if(!sc.FindWordReverse("/"))
		if(!sc.FindWordReverse("\\"))
			return "";

	_ext = sc.SplitAtPosition(sc.GetPostPosition());
	return _ext.GetFirst();
}

std::string GetStringFilename(std::string &str)
{
	std::string temp = str;
	splitstring_t _ext;
	Scanner sc(temp.c_str());
	if(!sc.FindWordReverse("/"))
		if(!sc.FindWordReverse("\\"))
			if(!sc.FindWordReverse("\\\\"))
				return str;

	_ext = sc.SplitAtPosition(sc.GetPostPosition());
	return _ext.GetSecond();
}

bool HasExtension(std::string &str)
{
	Scanner sc(str.c_str());
	return sc.FindWordReverse(".");
}

bool HasDirectory(std::string &str)
{
	Scanner sc(str.c_str());

	if(sc.FindWordReverse("/"))
		return true;
	if(sc.FindWordReverse("\\"))
		return true;
	return false;
}

std::string FormatString(std::string pmsg, ...)
{
	std::string ret = "";
	va_list vl;
	va_start(vl, pmsg); 
	ret = FormatString(pmsg, vl);
	va_end(vl);
	return ret;
}

std::string FormatString(std::string pmsg, va_list vl)
{
	Parser msgparser;
	//int count;
	std::string ret_str;
	char testch;
	char *tempchar;
	//va_list vl = vlz;
	//va_start(vl, pmsg);

	// %i or %d = Integer
	// %s = String
	// %f = Float
	// %p = memory address

	msgparser.OpenString(pmsg);
	msgparser.SetSpecialCharacters("%");

	while(msgparser.GetState() == PS_GOOD)
	{
		if(msgparser.PeekChar() == '%')
		{
			msgparser.SkipChar();
			testch = msgparser.GetChar();
			if(testch == 'd' || testch == 'i')
			{
				tempchar = new char[(sizeof(int)*8) + 1];
				//vsprintf(tempchar, "%d", vl);
				int temp_int = va_arg(vl, int);
				sprintf(tempchar, "%d", temp_int);
				ret_str += tempchar;
				delete[] tempchar;
			}

			else if(testch == 'f')
			{
				tempchar = new char[(sizeof(float)*8) + 1];
				//vsprintf(tempchar, "%f", vl);
				float temp_float = va_arg(vl, float);
				sprintf(tempchar, "%f", temp_float);
				ret_str += tempchar;
				delete[] tempchar;
			}

			else if(testch == 'c')
			{
				//tempchar = new char[(sizeof(float)*8) + 1];
				char c;
				//vsprintf(tempchar, "%f", vl);
				//float temp_float = va_arg(vl, float);
				sprintf(&c, "%c", va_arg(vl, char));
				ret_str += c;
				c = NULL;
				//delete[] tempchar;
			}

			else if(testch == 'p')
			{
				tempchar = new char[(sizeof(float)*8) + 1];
				//vsprintf(tempchar, "%f", vl);
				//float temp_float = va_arg(vl, float);
				sprintf(tempchar, "%p", va_arg(vl, int));
				ret_str += tempchar;
				delete[] tempchar;
			}

			else if(testch == 's')
			{
				std::string temp_str = va_arg(vl, std::string);
				ret_str += temp_str;
			}

			else
			{
				ret_str += "%" + testch;
			}
		}

		else
		{
			ret_str += msgparser.GetChar();
		}
	}

	//va_end(vl);
	return ret_str;
}

bool Scanner::FindWord(std::string word)
{
	if(caseinsensitive)
	{
		std::string newstr = ret_str;
		set_lowercase(newstr);
		set_lowercase(word);

		this->pos = newstr.find(word);
		if(pos == newstr.npos)
			return false;

		else
		{
			currentpos = pos;
			wordlength = word.length();
			return true;
		}
	}

	else
	{
		this->pos = this->ret_str.find(word);
		if(pos == ret_str.npos)
			return false;

		else
		{
			currentpos = pos;
			wordlength = word.length();
			return true;
		}
	}
}

bool Scanner::FindWordReverse(std::string word)
{
	if(caseinsensitive)
	{
		std::string newstr = ret_str;
		set_lowercase(newstr);
		set_lowercase(word);

		this->pos = newstr.rfind(word);
		if(pos == newstr.npos)
			return false;

		else
		{
			currentpos = pos;
			wordlength = word.length();
			return true;
		}
	}

	else
	{
		this->pos = this->ret_str.rfind(word);
		if(pos == ret_str.npos)
			return false;

		else
		{
			currentpos = pos;
			wordlength = word.length();
			return true;
		}
	}
}

splitstring_t Scanner::SplitAtPosition(int position)
{
	if(position > ret_str.length())
		position = ret_str.length() - 1;

	for(int i = 0; i < position; i++)
	{
		split.first += ret_str[i];
	}

	for(int i = position; i < ret_str.length(); i++)
	{
		split.second += ret_str[i];
	}

	return split;
}

void Scanner::ReplaceWord(std::string word, std::string replacement)
{
	if(FindWord(word))
	{
		std::string ztempstr = ret_str;
		ret_str.clear();

		for(int i = 0; i < currentpos; i++)
		{
			ret_str += ztempstr[i];
		}

		ret_str += replacement;
		currentpos += word.length();

		for(int i = currentpos; i < ztempstr.length(); i++)
		{
			ret_str += ztempstr[i];
		}

		ret_str.shrink_to_fit();
	}
}

int Scanner::ReplaceAll(std::string word, std::string replacement)
{
	int rcount = 0;

	while(FindWord(word))
	{
		rcount++;

		std::string ztempstr = ret_str;
		ret_str.clear();

		for(int i = 0; i < currentpos; i++)
		{
			ret_str += ztempstr[i];
		}

		ret_str += replacement;
		currentpos += word.length();

		for(int i = currentpos; i < ztempstr.length(); i++)
		{
			ret_str += ztempstr[i];
		}
	}

	ret_str.shrink_to_fit();
	return rcount;
}

void Scanner::EraseUntil(int position)
{
	std::string newstr = ret_str;
	ret_str.clear();

	for(int i = 0; i < position; i++)
	{
		ret_str += newstr[i];
	}

	ret_str.shrink_to_fit();
}

void Scanner::EraseAfter(int position)
{
	std::string newstr = ret_str;
	ret_str.clear();

	for(int i = position; i < newstr.length(); i++)
	{
		ret_str += newstr[i];
	}

	ret_str.shrink_to_fit();
}

std::string StripOuterQuotes( std::string str )
{
	std::string temp = str;
	splitstring_t _ext;
	Scanner sc(temp.c_str());
	if(!sc.FindWord("\""))
		return str;

	_ext = sc.SplitAtPosition(sc.GetPosition() + 1);
	temp = _ext.GetSecond();

	sc.SetString(temp.c_str());

	if(!sc.FindWordReverse("\""))
		return temp;

	_ext = sc.SplitAtPosition(sc.GetPosition());
	temp = _ext.GetFirst();

	return temp;
}

std::string StripOuterQuotes( std::string &str )
{
	std::string temp = str;
	splitstring_t _ext;
	Scanner sc(temp.c_str());
	if(!sc.FindWordReverse("\""))
		return str;

	_ext = sc.SplitAtPosition(sc.GetPosition());
	sc.SetString(_ext.GetFirst().c_str());
	if(!sc.FindWord("\""))
		return str;

	_ext = sc.SplitAtPosition(sc.GetPosition());
	return _ext.GetSecond();
}

std::string RemoveWhiteSpace(const std::string &str)
{
	Parser parser;
	parser.OpenString(str);
	std::string ret = "";
	while(parser.GetState() == PS_GOOD)
	{
		ret += parser.GetToken();
	}
	return ret;
}
