#pragma once

// Token Class
// GetToken reads a consistent piece of text, and returns it as a string
// White space breaks the token, but quoted strings ignore this rule
// PeekToken reads the next token, but does not move the reading position

/************************************************************************

A little explanation:

FindToken() moves the cursor AFTER a specified token. Use RewindToken()
and then GetToken() if you would like to return the token it found

Alternatively, use the overloaded FindToken() to specify the block level
the token should be on. This works almost exactly like FindTokenInBlock()'s
overloaded version, but will search all blocks at that level instead of
stopping after it exits the first block. You still have to Rewind and
then Get, however.

MoveToBeginning() rewinds the cursor all the way to the start of the file
chunk. This can be useful for finding new blocks or something, and must be
done if you want the config file to be independent of variable order. The
same applies for other files.

Fortunately, the config is loaded into memory storage prior to parsing,
so it isn't very costly to constantly move to the start and reread everything
just to avert listing order problems.

******************************NEW 1/2/2013******************************

For block parsing (in relation to the maps at the very least) we can recursively
open blocks in memory and parse them separately inside the functions.

Use dynamic callbacks (is that even possible? I think so) to determine
what variable type to obtain from parsed text, using a dictionary I'll
write for specific things (ex. the map loading will have it's own dictionary,
scripting will have another, etc.)

************************************************************************/

#pragma warning(disable: 4996)			// Disables a POSIX warning

// Macro for std:string.compare()
// Because checking for ==0 as true can be somewhat confusing after a while
#define TextCompare(x,y) x##.compare(y)==0
#define TextNotCompare(x,y) x##.compare(y)!=0

#define GLOBALNAME "_ParserGlobalBlockSpace"

void set_uppercase(std::string &s);
void set_lowercase(std::string &s);
bool CompNoCase(std::string a, std::string b);
bool CompCase(std::string a, std::string b);

struct  scriptblock_t
{
	std::string name;
	std::string contents;
	//bool wasquoted;
	unsigned int offset;					// Offset from parent block's beginning that this block begins from source material
	unsigned int linestart;
	unsigned int endpos;					// Ending position of this block from source material

	//unsigned int indexpos;					// A variable used for the sole purpose of parsing, stores this blocks location in a block stack (if any)

	scriptblock_t *parent;					// the parent block e.g. the block that this block belongs to
	std::vector<scriptblock_t> childblocks;	// Sub blocks of this block, with their own children, etc.

	inline void InsertBlock(scriptblock_t newblock) { this->childblocks.push_back(newblock); }
	scriptblock_t *GetBlock(std::string blockname);
	std::vector<scriptblock_t> GetChildren() { return childblocks; }
	inline std::string GetName() { return name; }
	inline std::string GetContent() { return contents; }
	inline scriptblock_t *GetParent() { if(!parent) return nullptr; else return parent; }
	inline bool HasParent() { if(parent == nullptr) return false; else return true; }
	inline bool HasChildren() { return !childblocks.empty(); }
};

// TO convert loop/recursion functions to use these states
enum eParserStates
{
	PS_GOOD = 0,
	PS_EOF = 1,
	PS_FAILURE = 2,
	PS_NOT_OPEN = 3,
	PS_UNEXPECTED_END = 4
};

class Parser
{
protected:
	char				*current;	// Current character
	char				*start;		// Start of text
	unsigned int		filelength;	// Length of File
	unsigned int		position;	// Current position in File
	std::string			special;	// A std::string defining the 'special characters'. These will always be parsed as separate tokens
	std::string			name;		// What file is being tokenized
	bool				qstring;	// True if the last read token was a quoted string
	unsigned int		line;		// The current line number
	unsigned int		t_start;	// The starting position of the last-read token
	unsigned int		t_end;		// The ending position of the last-read token
	char				*ctoken;	// C-Style token holder

	//unsigned int		blocklevel; // Current block level, or how many {'s we have passed, decreased when we pass a }
	// I'll try to just do a local count of this when it is absolutely needed, cuz the current implementation is an ugly hack

	std::string			lasttoken;	// Let's keep a local copy of the last returned string

	// For remembering functions

	unsigned int		rem_position;	// Current position in File
	char				*rem_current;	// Current character
	unsigned int		rem_line;		// The current line number

	std::string ret_errmsg;
	char errorline[10];

	bool readfullspecials;

private:
	BYTE parsestate;
	inline void SetState(BYTE state) { this->parsestate = state; }

public:
	Parser();
	~Parser();

	bool CompareNoCase(std::string x, std::string y);

	// Setting modifiers
	inline void SetSpecialCharacters(std::string special) { this->special = special; }
	inline void SetDefaultSpecialChars() { this->special = "{};,:|=/"; }

	// File Functionality
	inline std::string GetName() { return name; }
	bool OpenFile(std::string filename, unsigned int offset = 0, unsigned int length = 0);
	bool OpenString(std::string text, unsigned int offset = 0, unsigned int length = 0);
	bool OpenBlock(scriptblock_t *block, unsigned int offset = 0, unsigned int length = 0);
	bool OpenMem(const char *mem, unsigned int length, std::string source);

	bool PreParseBlocks();
	int CountTokens(std::string str);

	// Token returns
	void Abort() { SetState(PS_FAILURE); }
	bool SkipToken();
	void SkipToSemicolon();		// For error recovery -- Skips OVER the semicolon character!
	std::string ReadToSemicolon();	// also skips the semicolon!
	std::string PeekToSemicolon();  // reads until the semicolon
	void RewindToken();											// Rewind to the last returned token, allowing one to return it again with GetToken(). NOTE: Can only rewind one token, it cannot rewind past white space etc.
	std::string GetToken();
	std::string PeekToken(unsigned int count_ahead = 1);
	char PeekChar(int count_ahead = 1);
	char GetChar();
	std::string GetLine();
	void SkipLine();
	bool SeekToken(std::string tok);							// locates a token and places cursor before it, so GetToken() will return it
	bool FindTokenWithinBlock(std::string tok);
	bool CheckToken(std::string check);

	void RememberPosition();
	void RestorePosition();

	inline void GroupReadSpecials(bool x) { readfullspecials = x; } // When reading a special character, continue reading until a non-special character or white space is found
	std::string GetWhiteSpace();

	// Cursor shifts/movements
	bool IncrementCurrent();
	inline bool SkipChar() { return IncrementCurrent(); }
	bool ExitBlock();
	void SkipCodeBlock(std::string delimiter = "{", std::string closer = "}");
	void MoveToBeginning();

	std::string GetCodeBlock();

	bool MustGetToken(std::string tok);
	bool MustPeekToken(std::string tok);

	// Skip/Filter tools
	bool IsEOF();
	bool IsWhitespace(char p);
	bool IsSpecialCharacter(char p);
	void SkipLineComment();
	void SkipMultilineComment();

	bool HasSpecialCharacters(std::string tok);

	// Access functions
	inline bool			QuotedString() const { return qstring; }
	inline unsigned int	LineNo() const { return line; }
	inline unsigned int	TokenStart() const { return t_start; }
	inline unsigned int	TokenEnd() const { return t_end; }
	inline unsigned int FileLength() const { return filelength; }
	inline std::string LastToken() const { return lasttoken; }
	inline std::string Name() const { return name; }
	inline char CurrentContent() const { return *start; }

	inline void SetLineNo(unsigned int linenum) { this->line = linenum; }

	// Conversion functions
	int	GetInteger();
	float GetFloat();
	double GetDouble();
	bool GetBool();

	// Parser states
	inline BYTE GetState() const { return parsestate; }
};

