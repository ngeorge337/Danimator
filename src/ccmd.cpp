#include "libs.h"
#include "util.h"
#include "static.h"
#include "strmanip.h"
#include "token.h"
#include "file.h"
#include "configfile.h"
#include "console.h"
#include "cvar.h"
#include "ccmd.h"

STATIC_IMPLEMENT(CCmdManager)

CCMD(alias, "alias [name] [commands] -- creates an alternate name for a (set of) console command(s)", "%s%s")
{
	std::string arguments = self.in_args;
	Parser parser;
	parser.OpenString(arguments);

	std::string aname = parser.GetToken();
	if(parser.PeekToken().length() == 0)
	{
		CCMDMan->RemoveAlias(aname);
		return;
	}
	else
	{
		std::string batchlist = parser.GetLine();
		batchlist = StripOuterQuotes(batchlist);
		CCMDMan->CreateAlias(aname, batchlist);
	}
}

ConsoleCommand::ConsoleCommand(std::string name, std::string argv, std::string helpmsg, void (*callback)(ConsoleCommand &))
{
	set_lowercase(name);
	Name = name;
	args = argv;
	helptext = helpmsg;
	m_Callback = callback;
	argc = 0;

	Parser msgparser;
	char testch;
	int valid_args_size;

	if(!valid_args)
	{
		valid_args_size = strlen(VALID_ARGS);
		valid_args = new char[valid_args_size+1];
		valid_args = VALID_ARGS;
	}

	if(argv.length() > 0)
	{
		msgparser.OpenString(argv);

		while(msgparser.GetState() == PS_GOOD)
		{
			testch = msgparser.GetChar();

			if(testch != ' ' || testch != '\n' || testch != '\0')
			{
				for(int i = 0; i < valid_args_size; i++)
				{
					if(testch == valid_args[i])
					{
						argc++;
					}
				}
			}
		}
	}

	CCMDMan->StoreCCMD(name, this);
}

std::string ConsoleCommand::GetHelpText()
{
	return helptext;
}

void ConsoleCommand::SetHelpText( std::string msg )
{
	helptext = msg;
}

void ConsoleCommand::RunCommand(std::string argv)
{
	in_args = argv;
	if(m_Callback) m_Callback(*this);
	in_args.clear();
}

ConsoleCommand::~ConsoleCommand()
{
	//CCMD->RemoveCommand(this);
}

void CCmdManager::StoreCCMD( std::string name, ConsoleCommand *cmd )
{
	set_lowercase(name);
	if(FindCCMD(name))
	{
		Console->LogMessage("Tried to register Console Command '%s' more than once", name);
		return;
	}

	ccmdlist.push_back(name);
	ccmds.insert(std::pair<std::string, ConsoleCommand *>(name, cmd));
}

void CCmdManager::CreateAlias( std::string name, std::string cmdlistbatch )
{
	set_lowercase(name);
	alias_t Alias(name, cmdlistbatch);
	if(FindCCMD(name))
	{
		//Engine->Error("'%s' is an existing console command and cannot be used as an alias!", name);
		Console->LogMessage("'%s' is an existing console command and cannot be used as an alias!", name);
		return;
	}
	else if(Cvars->FindCvar(name))
	{
		//Engine->Error("'%s' is an existing console variable and cannot be used as an alias!", name);
		Console->LogMessage("'%s' is an existing console variable and cannot be used as an alias!", name);
		return;
	}
	else
	{
		if(aliases.find(name) != aliases.end())
			aliases.erase(name);

		aliaslist.push_back(name);
		aliases.insert(std::pair<std::string, alias_t>(name, Alias));
	}
}

bool CCmdManager::FindCCMD( std::string name )
{
	set_lowercase(name);
	if(ccmds.find(name) == ccmds.end())
		return false;

	return true;
}

bool CCmdManager::FindAlias( std::string name )
{
	set_lowercase(name);
	if(aliases.find(name) == aliases.end())
		return false;

	return true;
}

ConsoleCommand * CCmdManager::GetCCMD( std::string name )
{
	if(FindCCMD(name))
		return ccmds[name];

	else return nullptr;
}

alias_t * CCmdManager::GetAlias( std::string name )
{
	if(FindAlias(name))
		return &aliases[name];

	else return nullptr;
}

void CCmdManager::RemoveCommand( ConsoleCommand *cmd )
{
	/*
	if(FindCCMD(cmd->GetName()))
	{
	delete ccmds.at(cmd->GetName());
	ccmds.erase(cmd->GetName());
	}
	*/
	RemoveCommand(cmd->GetName());
}

void CCmdManager::RemoveCommand( std::string name )
{
	if(FindCCMD(name))
	{
		delete ccmds.at(name);
		ccmds.erase(name);
	}
}

void CCmdManager::RemoveAlias( std::string name )
{
	if(FindAlias(name))
	{
		aliases.erase(name);
		VectorErase(aliaslist, name);
	}
}

void CCmdManager::ReadCCMDs( ConfigFile &cfg )
{
	Parser parser;
	std::string tok;

	if(!cfg.IsOpen())
		return;

	parser.OpenString(cfg.GetConfig());

	while(parser.GetState() == PS_GOOD)
	{
		tok = parser.GetToken();
		if(Cvars->FindCvar(tok))
		{
			parser.SkipLine();
		}
		else if(FindCCMD(tok))
		{
			GetCCMD(tok)->RunCommand(parser.GetLine());
		}
		else
		{
			if(!parser.IsEOF())
			{
				parser.SkipLine();
			}
		}
	}
}
