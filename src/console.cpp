#include "libs.h"
#include "util.h"
#include "static.h"
#include "token.h"
#include "strmanip.h"
#include "file.h"
#include "configfile.h"
#include "ccmd.h"
#include "cvar.h"
#include "console.h"

#define OUTPUT_TO_STANDARD 1

STATIC_IMPLEMENT(ConsoleMain)

CCMD(echo, "echo [string] -- Prints a message to the console", "%s")
{
	std::string arguments = self.in_args;
	Parser parser;
	parser.OpenString(arguments);

	Console->LogMessage(parser.GetToken());
}

CCMD(clear, "", "")
{
	Console->Clear();
}

CCMD(logfile, "", "")
{
	if(Console->LogFile())
	{
		Console->LogMessage("\n----- Log Ended");
		Console->CloseLog();
	}
	else
	{
		std::string arguments = self.in_args;
		if(arguments.length() == 0)
			Console->CreateLog("logfile.log");
		else
		{
			Parser parser;
			parser.OpenString(arguments);
			Console->CreateLog(parser.GetToken());
		}
		std::time_t result = std::time(NULL);
		char *tm = std::asctime(std::localtime(&result));
		std::string timestr = tm;

		Console->LogMessage("----- Log Started %s\n", timestr);
	}
}

CCMD(ccmdlist, "", "")
{
	std::vector<std::string> cmdlist = CCMDMan->GetCCMDListing();
	std::sort(cmdlist.begin(), cmdlist.end());
	Console->LogMessage("Commands:\n   ---------");

	for(int i = 0; i < cmdlist.size(); i++)
		Console->LogMessage("%s", cmdlist.at(i));
}

CCMD(aliaslist, "", "")
{
	std::vector<std::string> aliaslists = CCMDMan->GetAliasListing();
	std::sort(aliaslists.begin(), aliaslists.end());
	Console->LogMessage("Aliases:\n   ---------");

	for(int i = 0; i < aliaslists.size(); i++)
		Console->LogMessage("%s", aliaslists.at(i));
}

CCMD(cvarlist, "", "")
{
	std::vector<std::string> cvarlists = Cvars->GetCvarListing();
	std::sort(cvarlists.begin(), cvarlists.end());
	Console->LogMessage("Console Vars:\n   -------------");

	for(int i = 0; i < cvarlists.size(); i++)
		Console->LogMessage("%s", cvarlists.at(i));
}

bool ConsoleLogFile::Open(std::string name, eFileMode md)
{
	mode = md;
	if(!FileSystem::OpenDir("."))
		return false;

	if(!HasExtension(name))
	{
		SuggestExtension(".log");
		name = name + ".log";
	}
	src = FileSystem::CreateFileLegacy(name, md);
	if(src == nullptr)
	{
		Console->LogMessage("Unable to write log file!");
		return false;
	}

	return true;
}

void ConsoleLogFile::Write(std::string data)
{
	fwrite(data.c_str(), 1, data.length(), src);
	Flush();
}

std::string ConsoleMain::DumpLog()
{
	std::string ret = "";

	for (size_t a = 0; a < log.size(); a++)
		ret += log.at(a);

	return ret;
}

void ConsoleMain::WriteLog(std::string log_name)
{
	ConsoleLogFile lf(log_name);
	lf.Write(this->DumpLog());
	//lf.Flush();
}

void ConsoleMain::LogString(std::string message)
{
	if (message.at(message.length() - 1) != '\n')
		message.append("\n");

	log.push_back(message);
	lastmsg = message;

#if OUTPUT_TO_STANDARD
	std::cout << message;
#endif

	if(LogFile() && logFile != nullptr)
	{
		logFile->Write(message);
	}
}

void ConsoleMain::LogMessage(std::string pmsg, ...)
{
	/*if(log.size() > MAX_LOG_LINES)
	{
	std::vector<std::string>::iterator it;
	it = log.begin();
	log.erase(it);
	}*/
	va_list vl;
	va_start(vl, pmsg);
	LogString(FormatString(pmsg, vl));
	va_end(vl);
}

void ConsoleMain::DoConsoleMessage( std::string msg )
{
	std::string teststr;
	std::string newcmd;
	int argc = 0;
	//char testch;
	Parser parser;
	parser.OpenString(msg);

	while(parser.GetState() == PS_GOOD)
	{
		teststr = parser.GetToken();
		argc = parser.CountTokens(parser.PeekToSemicolon());
		// first search for CVars matching the input
		set_lowercase(teststr);

		if(Cvars->FindCvar(teststr))
		{
			if(/*parser.PeekToken().empty() || TextCompare(parser.PeekToken(), ";") ||*/ argc == 0)
			{
				LogMessage("%s -- %s   Current Value: %s\n   Default Value: %s",
					Cvars->GetCvar(teststr)->GetName(),
					Cvars->GetCvar(teststr)->GetDescription(),
					Cvars->GetCvar(teststr)->ToString(Cvars->GetCvar(teststr)->Get(Cvars->GetCvar(teststr)->GetRealType()), Cvars->GetCvar(teststr)->GetRealType()),
					Cvars->GetCvar(teststr)->ToString(Cvars->GetCvar(teststr)->GetDefault(Cvars->GetCvar(teststr)->GetRealType()), Cvars->GetCvar(teststr)->GetRealType()));
				parser.SkipToSemicolon();
				//break;
			}
			else if(Cvars->GetCvar(teststr)->Flags & CVAR_SECRET)
			{
				LogMessage("CVAR '%s' cannot be set from the console directly", Cvars->GetCvar(teststr)->GetName());
				parser.SkipToSemicolon();
			}
			else
			{
				eCvarType type = Cvars->GetCvar(teststr)->GetRealType();
				Cvars->GetCvar(teststr)->SetRep(BaseCvar::FromString(parser.GetToken().c_str(), type), type);
				parser.SkipToSemicolon();
			}
		}

		else if(CCMDMan->FindCCMD(teststr))
		{
			if(/*parser.PeekToken().empty() || TextCompare(parser.PeekToken(), ";") ||*/ argc < CCMDMan->GetCCMD(teststr)->GetArgumentCount())
			{
				if(CCMDMan->GetCCMD(teststr)->GetHelpText().length() > 0)
					LogMessage("Usage: %s", CCMDMan->GetCCMD(teststr)->GetHelpText());
				parser.SkipToSemicolon();
				//break;
			}

			else
				CCMDMan->GetCCMD(teststr)->RunCommand(parser.ReadToSemicolon());
		}
		else if(CCMDMan->FindAlias(teststr))
		{
			newcmd = CCMDMan->GetAlias(teststr)->cmdbatch;
			DoConsoleMessage(newcmd);
		}
		else
		{
			LogMessage("Unknown Command '%s'", teststr);
			parser.SkipToSemicolon();
		}
	}
}

ConsoleMain::ConsoleMain()
{
	isLogging = false; logFile = nullptr;
}

ConsoleMain::~ConsoleMain()
{
	if(logFile != nullptr)
	{
		logFile->Close();
		delete logFile;
	}
}

void ConsoleMain::CreateLog( std::string logname )
{
	if(LogFile())
		return;

	isLogging = true;
	logFile = new ConsoleLogFile(logname);
}

void ConsoleMain::CloseLog()
{
	if(LogFile())
	{
		isLogging = false;
		logFile->Close();
		delete logFile;
	}
}

/*
sfg::Signal::SignalID ConsoleWindow::PromptEntry::OnTextSent=0;
sfg::Signal::SignalID ConsoleWindow::PromptEntry::OnHistoryUp=0;
sfg::Signal::SignalID ConsoleWindow::PromptEntry::OnHistoryDown=0;

ConsoleWindow::PromptEntry::Ptr ConsoleWindow::PromptEntry::Create(const sf::String& text)
{
	PromptEntry::Ptr ptr(new PromptEntry);
	ptr->SetText(text);
	return ptr;
}

void ConsoleWindow::PromptEntry::HandleKeyEvent(sf::Keyboard::Key key,bool press)
{
	sfg::Entry::HandleKeyEvent(key,press);

	if(!HasFocus()||!press) return;//move that up? hmm nah..

	switch(key)
	{
	case sf::Keyboard::Return:
		GetSignals().Emit(OnTextSent);
		break;
	case sf::Keyboard::Up:
		GetSignals().Emit(OnHistoryUp);
		break;
	case sf::Keyboard::Down:
		GetSignals().Emit(OnHistoryDown);
		break;
	}
}//HandleKeyEvent

ConsoleWindow::ConsoleWindow(float width)
	: GUIObject("Console"), m_MsgCount(0u), m_CacheIndex(0)
{
	m_Box=sfg::Box::Create(sfg::Box::VERTICAL);
	sfg::Table::Ptr tab=sfg::Table::Create();
	m_Prompt=ConsoleWindow::PromptEntry::Create();
	m_Scrolled=sfg::ScrolledWindow::Create();
	//m_PromptChar=sfg::Label::Create();

	tab->SetColumnSpacings(1.f);
	tab->SetRowSpacings(1.f);
	//m_Combo=sfg::ComboBox::Create();

	tab->Attach(m_Scrolled,sf::Rect<sf::Uint32>(0,0,3,1),2,3);
	//tab->Attach(m_PromptChar,sf::Rect<sf::Uint32>(1,1,1,1),0,0);
	tab->Attach(m_Prompt,sf::Rect<sf::Uint32>(0,1,3,1),3,0);
	//tab->Attach(m_Combo,sf::Rect<sf::Uint32>(0,1,1,1),2,0);

	m_Scrolled->AddWithViewport(m_Box);

	m_Box->Pack(m_LastLabel=sfg::Label::Create());

	m_Prompt->GetSignal(ConsoleWindow::PromptEntry::OnTextSent).Connect(&ConsoleWindow::onSend,this);
	m_Prompt->GetSignal(ConsoleWindow::PromptEntry::OnHistoryUp).Connect(&ConsoleWindow::onUp,this);
	m_Prompt->GetSignal(ConsoleWindow::PromptEntry::OnHistoryDown).Connect(&ConsoleWindow::onDown,this);

	m_Prompt->SetMaximumLength(255);

	windowptr->SetId("console_window");
	m_Prompt->SetId( "console_prompt" );
	m_LastLabel->SetId( "console_label" );

	//sf::Font my_font;
	//my_font.loadFromFile( "fonts/consola.ttf" );
	//Engine->GUI->desktop.GetEngine().GetResourceManager().AddFont( "console_font", my_font, false ); // false -> do not manage!
	//Engine->GUI->desktop.GetEngine().SetProperties("Entry#console_prompt { FontName: console_font; } Label#console_label { FontName: console_font; }");
	//Engine->GUI->desktop.GetEngine().SetProperty("Label#console_label", "FontName", "console_font");
	//sfg::Context::Get().GetEngine().SetProperty("Entry#console_prompt", "FontName", "console_font");
	//sfg::Context::Get().GetEngine().SetProperty("Label#console_label", "FontName", "console_font");

	//Engine->GUI->desktop.GetEngine().SetProperties("Label { FontName: fonts/consola.ttf; } Window#console_window > * > Entry { FontName: fonts/consola.ttf; }");

	//m_Combo->GetSignal(sfg::ComboBox::OnSelect).Connect(&ConsoleWindow::onChoose,this);

	//m_Combo->SelectItem(0);
	// Write the current log contents to the window when created
	for(int i = 0; i < Console->log.size(); i++)
	{
		message(Console->log.at(i));
	}

	windowptr->Add(tab);
	windowptr->SetStyle(sfg::Window::TOPLEVEL);
	windowptr->SetRequisition(sf::Vector2f(width, width * 0.75f));
	windowptr->SetPosition(sf::Vector2f(200.f, 25.f));
	//setPromptChar();

	m_Prompt->GrabFocus();
}

void ConsoleWindow::Clear()
{
	m_Box->RemoveAll();
	m_Box->Pack(m_LastLabel=sfg::Label::Create());
	m_Text.clear();
	m_MsgCount=0u;
}

void ConsoleWindow::setPromptChar(bool incomplete)
{
	//m_PromptChar->SetText(incomplete?">":"<");//do smt about prompt unfocusing entry or keep ><
}

void ConsoleWindow::onSend()
{
	std::string intext = m_Prompt->GetText();
	std::string logged = "] " + intext;
	int loglen;
	if(intext.length() == 0)
		return;

	Console->LogMessage(logged);
	message(logged);

	if(!m_CachedLine.empty()) m_CachedLine += ' ';//if not empty we must avoid concats accidents
	m_CachedLine+=intext; //m_Prompt->GetText();
	cache();
	loglen = Console->log.size();
	Console->DoConsoleMessage(intext);
	if(loglen != Console->log.size())
	{
		if(Console->log.size() == 0)
			Clear();
		else
			for(int i = loglen; i < Console->log.size(); ++i)
				message(Console->log.at(i));
	}

	m_Prompt->SetText(sf::String());
}

//void ConsoleWindow::onChoose()
//{
//	std::string vmname=m_Combo->GetSelectedText().toAnsiString();
//	auto it=m_States.find(vmname);
//	assert(it!=m_States.end());
//	if(m_Target==it->second) return;
//	message("Sending to:"+vmname);
//	m_Target=it->second;
//	m_CachedLine.clear();//hmm?
//}

void ConsoleWindow::message(const sf::String& msg )
{
	sf::String _msg = msg;
	if(m_MsgCount==MessagesPerLabel)
	{
		m_Box->Pack(m_LastLabel=sfg::Label::Create());
		m_MsgCount=0u;
		m_Text=sf::String();
	}
	//if(!m_Text.isEmpty()) m_Text+='\n';
	std::string str = msg;
	if (str.at(str.length() - 1) != '\n')
	{
		str.append("\n");
		_msg += "\n";
	}
	m_Text+=_msg;
	m_LastLabel->SetText(m_Text);
	++m_MsgCount;
	m_Scrolled->GetVerticalAdjustment()->SetValue(m_Scrolled->GetVerticalAdjustment()->GetUpper() + m_Scrolled->GetVerticalAdjustment()->GetMajorStep());
	m_Scrolled->Update(0.5f);
	m_Scrolled->Refresh();
	m_Scrolled->GetVerticalAdjustment()->SetValue(m_Scrolled->GetVerticalAdjustment()->GetUpper());
	//sfg::Adjustment::Ptr adj=m_Scrolled->GetVerticalAdjustment();
	//maybe(?) scrolling is crapped? check it out?
	//adj->SetValue(adj->GetUpper());//scroll the window totally(?) down(?)
	//m_Scrolled->SetVerticalAdjustment(adj);
}

void ConsoleWindow::cache()
{
	m_Buffer.push_back(m_Prompt->GetText());
	if(m_Buffer.size()>MaxCache) m_Buffer.pop_front();
	m_CacheIndex=m_Buffer.size();
}

void ConsoleWindow::onUp()
{
	if(m_Buffer.empty()) return;
	if(m_CacheIndex>0) --m_CacheIndex;
	m_Prompt->SetText(m_Buffer[m_CacheIndex]);
	m_Prompt->SetCursorPosition(m_Prompt->GetText().getSize());
}

void ConsoleWindow::onDown()
{
	if(m_Buffer.empty()) return;
	if(m_CacheIndex>=m_Buffer.size()) return;
	if(m_CacheIndex<(m_Buffer.size()-1)) ++m_CacheIndex;
	m_Prompt->SetText(m_Buffer[m_CacheIndex]);
	m_Prompt->SetCursorPosition(m_Prompt->GetText().getSize());
}
*/