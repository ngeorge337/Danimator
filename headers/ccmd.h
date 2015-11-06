#pragma once

#define VALID_ARGS "ibdfs"
#define MAX_ARGS 5

struct alias_t
{
	alias_t() {}
	alias_t(std::string Aliasname, std::string Batched) { aliasname = Aliasname; cmdbatch = Batched; }
	std::string aliasname, cmdbatch;
};

class ConsoleCommand
{
public:
	ConsoleCommand(std::string name, std::string argv, std::string helpmsg, void (*callback)(ConsoleCommand &));
	~ConsoleCommand();

	void RunCommand(std::string argv);
	void SetHelpText(std::string msg);
	std::string GetName() { return Name; }
	std::string GetHelpText();
	int GetArgumentCount() { return argc; }

	std::string in_args;	// incoming arguments for a command

private:
	char *valid_args;
	int argc;
	std::string Name;
	std::string helptext;
	std::string args;


	static bool m_UseCallback;

	void (*m_Callback)(ConsoleCommand &);
};

class CCmdManager
{
	MAKE_STATIC_CLASS(CCmdManager)
public:
	bool FindCCMD(std::string name);
	bool FindAlias(std::string name);
	ConsoleCommand *GetCCMD(std::string name);
	alias_t *GetAlias(std::string name);
	void RemoveCommand(ConsoleCommand *cmd);
	void RemoveCommand(std::string name);
	void CreateAlias(std::string name, std::string cmdlistbatch);
	void RemoveAlias(std::string name);

	std::vector<std::string> GetCCMDListing() const { return ccmdlist; }
	std::vector<std::string> GetAliasListing() const { return aliaslist; }

	void StoreCCMD(std::string name, ConsoleCommand *cmd);

	void ReadCCMDs(ConfigFile &cfg);

private:
	CCmdManager() {}
	CCmdManager(const CCmdManager &) {}
	//CvarManager operator =(const CvarManager &) {}
	~CCmdManager() {}

	std::vector<std::string> ccmdlist;
	std::map<std::string, ConsoleCommand *> ccmds;

	std::vector<std::string> aliaslist;
	std::map<std::string, alias_t> aliases;
};

#define CCMDMan CCmdManager::GetInstance()

#define CCMD(name, help, arguments) \
	static void ccmdfunc_##name(ConsoleCommand &); \
	ConsoleCommand name (#name, arguments, help, ccmdfunc_##name); \
	static void ccmdfunc_##name(ConsoleCommand &self)