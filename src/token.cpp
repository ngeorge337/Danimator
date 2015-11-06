#include "libs.h"
#include "util.h"
#include "static.h"
#include "file.h"
#include "console.h"
#include "configfile.h"
#include "token.h"
#undef max

void set_uppercase(std::string &s)
{
	std::string::iterator i = s.begin();
	std::string::iterator end = s.end();

	while(i != end)
	{
		*i = toupper((unsigned char) *i);
		++i;
	}
}

void set_lowercase(std::string &s)
{
	std::string::iterator i = s.begin();
	std::string::iterator end = s.end();

	while(i != end)
	{
		*i = tolower((unsigned char) *i);
		++i;
	}
}

bool CompNoCase(std::string a, std::string b)
{
	std::string::iterator i = a.begin();
	std::string::iterator end = a.end();

	while (i != end)
	{
		*i = toupper((unsigned char) *i);
		++i;
	}

	std::string::iterator i2 = b.begin();
	std::string::iterator end2 = b.end();

	while (i2 != end2)
	{
		*i2 = toupper((unsigned char) *i2);
		++i2;
	}

	if(TextCompare(a, b))
		return true;

	else
		return false;
}

bool CompCase(std::string a, std::string b)
{
	if(TextCompare(a, b))
		return true;

	else
		return false;
}

scriptblock_t *scriptblock_t::GetBlock(std::string blockname)
{
	scriptblock_t *retblock = nullptr;

	for(unsigned int i = 0; i < this->childblocks.size(); i++)
	{
		if(TextCompare(this->childblocks.at(i).GetName(), blockname))
		{
			*retblock = this->childblocks.at(i);
			break;
		}
	}

	return retblock;
}

Parser::Parser()
{
	current = nullptr;
	start = nullptr;
	ctoken = nullptr;
	filelength = 0;
	special = "{};,:|=/";	// Default special characters
	name = "nothing";
	line = 1;
	t_start = 0;
	t_end = 0;
	lasttoken = "";
	this->SetState(PS_NOT_OPEN);
	readfullspecials = false;
	//isscript = false;
	//linear = false;
	//blocklevel = 0;
}

Parser::~Parser()
{
	if(start != NULL)
		delete[] start;
	//if(ctoken)
	//	delete[] ctoken;
	//if(current)
	//	delete[] current;
}

bool Parser::OpenFile(std::string filename, unsigned int offset, unsigned int length)
{
	if(start != NULL)
		delete[] start;
	// Open the file
	FILE *file;

	char *cfilename = new char[filename.size()+1];

	strcpy(cfilename, filename.c_str());

	file = fopen(cfilename, "r");

	if(file == NULL)
	{
		// TO Put error message stuff here
		fclose(file);
		return false;
	}

	fseek(file, 0, SEEK_END);
	unsigned int fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	name = filename;

	// If length isn't specified or exceeds the file length,
	// only read to the end of the file
	if(offset + length > fsize || length == 0)
		length = fsize - offset;

	// Setup variables & allocate memory
	filelength = length;
	position = 0;
	start = current = new char[filelength];
	line = 1;
	t_start = 0;
	t_end = 0;

	// Read the file portion
	fseek(file, offset, SEEK_SET);
	fread(start, filelength, length, file);

	// Unsure if this is needed or will cause problems
	// but the information should already be copied in so...
	fclose(file);

	this->SetState(PS_GOOD);

	return true;
}

bool Parser::OpenString(std::string text, unsigned int offset, unsigned int length)
{
	if(start != NULL)
		delete[] start;
	// If length isn't specified or exceeds the std::string's length,
	// only copy to the end of the std::string
	if(offset + length > (unsigned int) text.length() || length == 0)
		length = (unsigned int) text.length() - offset;

	name = text;

	// Setup variables & allocate memory
	filelength = length;
	position = 0;
	line = 1;
	t_start = 0;
	t_end = 0;
	start = current = new char[filelength];

	// Copy the std::string portion
	memcpy(start, ((char*) text.c_str()) + offset, filelength);

	this->SetState(PS_GOOD);

	return true;
}

bool Parser::OpenBlock(scriptblock_t *block, unsigned int offset, unsigned int length)
{
	if(start != NULL)
		delete[] start;
	// If length isn't specified or exceeds the std::string's length,
	// only copy to the end of the std::string
	if(offset + length > (unsigned int) block->contents.length() || length == 0)
		length = (unsigned int) block->contents.length() - offset;

	name = block->name;

	// Setup variables & allocate memory
	filelength = length;
	position = 0;
	line = block->linestart + 1;
	t_start = 0;
	t_end = 0;
	start = current = new char[filelength];

	// Copy the std::string portion
	memcpy(start, ((char*) block->contents.c_str()) + offset, filelength);

	this->SetState(PS_GOOD);

	return true;
}

bool Parser::OpenMem(const char *mem, unsigned int length, std::string source)
{
	if(start != NULL)
		delete[] start;
	// Length must be specified
	if(length == 0)
	{
		Console->LogMessage("Parser::OpenMem() -- Length must be greater than zero!");
		return false;
	}

	name = source;

	// Setup variables & allocate memory
	filelength = length;
	position = 0;
	line = 1;
	t_start = 0;
	t_end = 0;
	start = current = new char[filelength];

	// Copy the data
	memcpy(start, mem, filelength);

	this->SetState(PS_GOOD);

	return true;
}

bool Parser::IsWhitespace(char p)
{
	// Whitespace is either a newline, tab character or space
	if(p == '\n' || p == 13 || p == ' ' || p == '\t')
		return true;
	else
		return false;
}

bool Parser::IsSpecialCharacter(char p)
{
	// Check through special tokens std::string
	if(special.size() > 0)
	{
		for(unsigned a = 0; a < special.size(); a++)
		{
			if(special[a] == p)
				return true;
		}
	}

	return false;
}

bool Parser::IncrementCurrent()
{
	if(position >= filelength - 1)
	{
		// At end of text, set state to PS_EOF to signify the end has been reached
		this->SetState(PS_EOF);
		//position = len;
		//position = 0;
		//current = start;
		return false;
	}
	else
	{
		// Check for newline
		if(current[0] == '\n')
			line++;

		// Increment position & current pointer
		position++;
		current++;
		t_end++;
		return true;
	}
}

bool Parser::SkipToken()
{
	std::string hastoken = GetToken();

	if(TextCompare(hastoken, "") || this->position == this->filelength)
	{
		this->SetState(PS_EOF);
		return false;
	}

	else
		return true;
}

void Parser::SkipLineComment()
{
	// Increment position until a newline or end is found
	while(current[0] != '\n' && current[0] != 13)
	{
		if(!IncrementCurrent())
			return;
	}

	// Skip the newline character also
	IncrementCurrent();
}

void Parser::SkipMultilineComment()
{
	// Increment position until '*/' or end is found
	while(!(current[0] == '*' && current[1] == '/'))
	{
		if(!IncrementCurrent())
			return;
	}

	// Skip the ending '*/'
	IncrementCurrent();
	IncrementCurrent();
}

void Parser::SkipToSemicolon()
{
	while(GetState() == PS_GOOD && TextNotCompare(lasttoken, ";"))
	{
		GetToken();
	}
}

std::string Parser::ReadToSemicolon()
{
	std::string ret_str;
	bool inQuotes = false;
	while(GetState() == PS_GOOD)
	{
		if(PeekChar() == ';' && inQuotes == false)
			break;

		if(PeekChar() == '\"')
			inQuotes = !inQuotes;

		ret_str += GetChar();
	}
	SkipChar();
	return ret_str;
}

std::string Parser::PeekToSemicolon()
{
	std::string ret_str;
	char *c = current;
	unsigned int p = position;
	unsigned int ln = line;
	BYTE oldstate = GetState();
	bool inQuotes = false;

	while(GetState() == PS_GOOD)
	{
		if(PeekChar() == ';' && inQuotes == false)
			break;

		if(PeekChar() == '\"')
			inQuotes = !inQuotes;

		ret_str += GetChar();
	}
	SkipChar();

	SetState(oldstate);

	// Go back to original position
	current = c;
	position = p;
	line = ln;
	return ret_str;
}

std::string Parser::GetToken()
{
	std::string ret_str = "";
	bool ready = false;
	qstring = false;

	// Increment pointer to next token
	while(!ready)
	{
		ready = true;

		// Increment pointer until non-whitespace is found
		while(IsWhitespace(current[0]))
		{
			// Return if end of text found
			if(!IncrementCurrent())
			{
				this->SetState(PS_EOF);
				lasttoken = ret_str;
				return ret_str;
			}
		}

		// Check if we have a line comment
		if(current[0] == '/' && current[1] == '/')
		{
			SkipLineComment(); // Skip it
			ready = false;
		}

		// Check if we have a multiline comment
		if(current[0] == '/' && current[1] == '*')
		{
			SkipMultilineComment(); // Skip it
			ready = false;
		}


		// Check for end of text
		if(position == filelength)
		{
			this->SetState(PS_EOF);
			lasttoken = ret_str;
			return ret_str;
		}
	}

	// Init token delimiters
	t_start = position;
	t_end = position;

	// If we're at a special character, it's our token
	if(readfullspecials && IsSpecialCharacter(current[0]))
	{
		do
		{
			ret_str += current[0];
			t_end = position + 1;
			IncrementCurrent();
		} while(IsSpecialCharacter(current[0]) && !IsEOF());

		lasttoken = ret_str;
		return ret_str;
	}

	else if(IsSpecialCharacter(current[0]))
	{
		ret_str += current[0];
		t_end = position + 1;
		IncrementCurrent();

		lasttoken = ret_str;
		return ret_str;
	}

	// Now read the token
	if(current[0] == '\"') // If we have a literal std::string (enclosed with "")
	{ 
		qstring = true;

		// Skip opening "
		IncrementCurrent();

		// Read literal std::string (include whitespace)
		while(current[0] != '\"')
		{
			ret_str += current[0];

			if(!IncrementCurrent())
			{
				this->SetState(PS_EOF);
				lasttoken = ret_str;
				return ret_str;
			}
		}

		// Skip closing "
		IncrementCurrent();
	}
	else
	{
		// Read token (don't include whitespace)
		while(!IsWhitespace(current[0]))
		{
			// Return if special character found
			if(IsSpecialCharacter(current[0]))
			{
				lasttoken = ret_str;
				return ret_str;
			}

			// Add current character to the token
			ret_str += current[0];

			// Return if end of text found
			if (!IncrementCurrent())
			{
				this->SetState(PS_EOF);
				lasttoken = ret_str;
				return ret_str;
			}
		}
	}

	// Return the token
	lasttoken = ret_str;
	return ret_str;
}

// Previews the nth token from the current position, then moves back to the current position
// count_ahead is 1-based, so parameter 0 or 1 will just preview the next token, 2 will preview
// the one after that, etc.
std::string Parser::PeekToken(unsigned int count_ahead)
{
	// Backup current position
	char *c = current;
	unsigned int p = position;
	unsigned int ln = line;
	BYTE oldstate = GetState();

	count_ahead = std::max(count_ahead, (unsigned int) 1);

	if(count_ahead > 1)
	{
		for(unsigned int i = 1; i < count_ahead; i++)
			SkipToken();
	}

	// Read the next token
	std::string token = GetToken();

	SetState(oldstate);

	// Go back to original position
	current = c;
	position = p;
	line = ln;

	// Return the token
	return token;
}

char Parser::PeekChar(int count_ahead)
{
	// Backup current position
	char *c = current;
	unsigned int p = position;
	unsigned int ln = line;
	char rc;

	if(count_ahead > 1)
	{
		for(int i = 1; i < count_ahead; i++)
			SkipChar();
	}

	rc = current[0];
	IncrementCurrent();

	// Go back to original position
	current = c;
	position = p;
	line = ln;

	return rc;
}

char Parser::GetChar()
{
	char rc;
	rc = current[0];
	IncrementCurrent();
	return rc;
}

std::string Parser::GetLine()
{
	std::string retstr;

	while(current[0] != '\n' && current[0] != 13 && GetState() == PS_GOOD)
	{
		//retstr += current[0];
		//IncrementCurrent();
		retstr += GetChar();
	}
	if(current[0] == '\n')
		SkipChar();

	lasttoken = retstr;
	return retstr;
}

void Parser::SkipLine()
{
	while(current[0] != '\n' && current[0] != 13 && GetState() == PS_GOOD)
	{
		//retstr += current[0];
		//IncrementCurrent();
		SkipChar();
	}
	if(current[0] == '\n')
		SkipChar();
}

bool Parser::IsEOF()
{
	return (this->position >= this->filelength - 1);
}

bool Parser::SeekToken(std::string tok)
{
	std::string ret_str = "";
	std::string errormsg;

	// In case we can't find the token, we can restore the old positions
	char *c = current;
	unsigned int p = position;
	unsigned int ln = line;

	while(TextNotCompare(PeekToken(), tok) && this->position < this->filelength - 1)
		SkipToken();

	ret_str = PeekToken();

	if(this->position >= this->filelength - 1 && TextNotCompare(ret_str, tok))
	{
		current = c;
		position = p;
		line = ln;

		errormsg = "Error: could not find token value: " + tok;
		Console->LogString(errormsg);

		return false;
	}
	else
		return true;
}

// FindTokenWithinBlock()
// Searches a block for a specific token
// Makes sure the token is on the same block level as the
// block being searched
bool Parser::FindTokenWithinBlock(std::string tok)
{
	std::string ret_str = "";
	bool enteredblock = false;
	unsigned int bl = 1;

	// In case we can't find the token, we can restore the old positions
	char *c = current;
	unsigned int p = position;
	//unsigned int bl = blocklevel;
	unsigned int ln = line;

	while(TextNotCompare(PeekToken(), tok) && this->position < this->filelength - 1)
	{
		SkipToken();

		if(TextCompare(lasttoken, "{")) // We entered our block
			++bl;

		if(TextCompare(lasttoken, "}")) // We entered our block
			--bl;

		// If we end up exiting the current block, abort the process
		if(bl < 1)
		{
			current = c;
			position = p;
			line = ln;
			return false;
		}
	}

	ret_str = GetToken();

	if(this->position >= this->filelength - 1 && TextNotCompare(ret_str, tok))
	{
		current = c;
		position = p;
		line = ln;
		return false;
	}
	else
		return true;
}

bool Parser::CheckToken(std::string check)
{
	return !(GetToken().compare(check));
}

int Parser::GetInteger()
{
	// Get token
	std::string token = GetToken();

	ctoken = new char[token.size()];

	strcpy(ctoken, token.c_str());


	// Return integer value
	return atoi(ctoken);
}

float Parser::GetFloat()
{
	// Get token
	std::string token = GetToken();

	ctoken = new char[token.size()];

	strcpy(ctoken, token.c_str());

	// Return float value
	return (float) atof(ctoken);
}

double Parser::GetDouble()
{
	// Get token
	std::string token = GetToken();

	ctoken = new char[token.size()];

	strcpy(ctoken, token.c_str());

	// Return double value
	return atof(ctoken);
}

bool Parser::GetBool()
{
	// Get token
	std::string token = GetToken();


	// If the token is a std::string "no" or "false", the value is false
	//if(_stricmp(ctoken, "no") || _stricmp(ctoken, "false"))
	if(CompareNoCase("no", token) || CompareNoCase("false", token))
		return false;

	ctoken = new char[token.size()];
	strcpy(ctoken, token.c_str());

	// Returns true ("1") or false ("0")
	return !!atoi(ctoken);
}

void Parser::MoveToBeginning()
{
	this->line = 1;
	this->position = 0;
	this->current = start;
	//this->blocklevel = 0;
	this->t_start = 0;
	this->t_end = 0;
	SetState(PS_GOOD);
}

bool Parser::ExitBlock()
{
	std::string ret_str = "";

	// In case we can't find the token, we can restore the old positions
	char *c = current;
	unsigned int p = position;
	unsigned int ln = line;

	while(TextNotCompare(ret_str, "}") && this->position < this->filelength - 1)
		ret_str = GetToken();

	if(this->position >= this->filelength - 1 && TextNotCompare(ret_str, "}"))
	{
		current = c;
		position = p;
		line = ln;
		return false;
	}
	else
		return true;
}

//todo make this properly revert line counts as well
void Parser::RewindToken()
{
	position -= TokenEnd() - TokenStart();
	current -= TokenEnd() - TokenStart();
	PeekToken(); // To change t_start and t_end
}

void Parser::SkipCodeBlock(std::string delimiter, std::string closer)
{
	int skippedblocks = 0;
	std::string oldspecialchars = this->special;
	std::string newspecialchars = delimiter + closer;
	SetSpecialCharacters(newspecialchars);

	while(TextNotCompare(PeekToken(), delimiter) && GetState() == PS_GOOD)
	{
		if(!SkipToken())
			return;
	}

	while(TextNotCompare(PeekToken(), closer) && skippedblocks != 0 && GetState() == PS_GOOD)
	{
		if(TextCompare(PeekToken(), delimiter))
			++skippedblocks;

		else if(TextCompare(PeekToken(), closer))
			--skippedblocks;

		if(skippedblocks < 0)
		{
			Console->LogMessage("Parser::SkipCodeBlock() -- invalid block; mismatched delimiters");
			SetSpecialCharacters(oldspecialchars);
			return;
		}

		if(!SkipToken())
		{
			SetState(PS_UNEXPECTED_END);
			Console->LogMessage("Parser::SkipCodeBlock() -- Unexpected end of file");
			SetSpecialCharacters(oldspecialchars);
			return;
		}
	}

	// Skip the closing bracket
	//SkipToken();
	SetSpecialCharacters(oldspecialchars);
}

std::string Parser::GetCodeBlock()
{
	unsigned int skippedblocks = 0;
	std::string return_contents = "";

	while(TextNotCompare(PeekToken(), "{"))
	{
		if(!SkipToken())
			return return_contents;
	}

	while(TextNotCompare(PeekToken(), "}") && skippedblocks != 0)
	{
		if(TextCompare(PeekToken(), "{"))
			++skippedblocks;

		else if(TextCompare(PeekToken(), "}"))
			--skippedblocks;

		if(skippedblocks < 0)
		{
			Console->LogMessage("Parser::GetCodeBlock() -- invalid block; mismatched delimiters");
			return "";
		}

		return_contents += GetToken();
	}

	// Add the last }
	return_contents += GetToken();

	return return_contents;
}

bool Parser::MustGetToken(std::string tok)
{
	if(TextNotCompare(GetToken(), tok))
	{
		itoa(LineNo(), errorline, 10);
		ret_errmsg = "Error: " + name + " Line " + errorline + " - Expected '" + tok + "' but got " + lasttoken;
		Console->LogString(ret_errmsg);
		return false;
	}
	else
		return true;
}

bool Parser::MustPeekToken(std::string tok)
{
	if(TextNotCompare(PeekToken(), tok))
	{
		itoa(LineNo(), errorline, 10);
		ret_errmsg = "Error: " + name + " Line " + errorline + " - Expected '" + tok + "' but got " + PeekToken();
		Console->LogString(ret_errmsg);
		return false;
	}
	else
		return true;
}

bool Parser::CompareNoCase(std::string x, std::string y)
{
	set_uppercase(x);
	set_uppercase(y);

	if(TextCompare(x,y))
		return true;
	else
		return false;
}

// Preparse code blocks in memory; just a simple test to make sure block openings/closings match up.
// Makes the parser a little more strict but a lot of parsing code will likely rely on there being matched up block counts.
// At least, map parsing certainly will.
bool Parser::PreParseBlocks()
{
	unsigned int blockcount = 0;
	std::string test = "";

	if(this->GetState() == PS_NOT_OPEN)
		return false;

	std::string oldspecialchars = this->special;
	SetSpecialCharacters("{}");

	while(this->GetState() == PS_GOOD)
	{
		test = GetToken();
		if(TextCompare(test, "{"))
			++blockcount;

		else if(TextCompare(test, "}"))
			--blockcount;
	}

	// Move back to the start and restore the special chars
	this->MoveToBeginning();
	SetSpecialCharacters(oldspecialchars);

	if(blockcount == 0)
		return true;

	else return false;
}

void Parser::RememberPosition()
{
	rem_current = current;
	rem_line = line;
	rem_position = position;
}

void Parser::RestorePosition()
{
	current = rem_current;
	line = rem_line;
	position = rem_position;
}

int Parser::CountTokens( std::string str )
{
	Parser parser;
	int i = 0;
	if(str.empty())
		return 0;
	parser.OpenString(str);

	while(parser.GetState() == PS_GOOD)
	{
		i++;
		parser.SkipToken();
	}

	return i;
}

std::string Parser::GetWhiteSpace()
{
	std::string ret_str = "";
	while(IsWhitespace(current[0]))
	{
		ret_str += current[0];

		// Return if end of text found
		if(!IncrementCurrent())
		{
			this->SetState(PS_EOF);
			lasttoken = ret_str;
			return ret_str;
		}
	}

	return ret_str;
}

bool Parser::HasSpecialCharacters( std::string tok )
{
	if(special.empty())
		return false;
	for(int i = 0; i < tok.size(); i++)
	{
		if(IsSpecialCharacter(tok[i]))
			return true;
	}
	return false;
}
