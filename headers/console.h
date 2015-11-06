#pragma once

#define MAX_LOG_LINES 500

	class ConsoleLogFile : public File
	{
	public:
		ConsoleLogFile(std::string logname) : File(logname)
		{
			Open(logname, FM_APPEND_WRITE);
		}
		//~ConsoleLogFile() { Close(); }

		void Write(std::string data);

	private:
		virtual bool Open(std::string name, eFileMode md);
	};

	class ConsoleMain
	{
		//friend class ConsoleWindow;
		MAKE_STATIC_CLASS(ConsoleMain)
	private:
		std::vector<std::string> log;
		std::string lastmsg;

		//ConsoleMain() { initops(); }
		ConsoleMain();
		ConsoleMain(const ConsoleMain &) { }
		ConsoleMain &operator =(const ConsoleMain &) { return *this; }
		~ConsoleMain();

		bool isLogging;
		ConsoleLogFile *logFile;

	public:
		bool LogFile() { return isLogging; }
		void CreateLog(std::string logname);
		void CloseLog();
		void DoConsoleMessage(std::string msg);
		inline std::string GetLastMessage() { return lastmsg; }

		std::string DumpLog();
		void WriteLog(std::string log_name = "console.log");
		void LogString(std::string message);
		void LogMessage(std::string pmsg, ...);
		void Clear() { log.clear(); lastmsg = ""; }
	};

	//#define Console Engine->_Console
	#define Console ConsoleMain::GetInstance()

	/*
	class  ConsoleWindow : public GUIObject
	{
	public:
		ConsoleWindow(float width = 550.f);
		//sfg::Window::Ptr GetWindow() const { return m_Window; }

		void message(const sf::String& msg);
		void Clear();

	private:
		class PromptEntry : public sfg::Entry
		{
		public:
			typedef sfg::SharedPtr<PromptEntry> Ptr;
			typedef sfg::SharedPtr<const PromptEntry> PtrConst;

			static Ptr Create(const sf::String& text = L"");

			virtual void HandleKeyEvent(sf::Keyboard::Key key,bool press);

			//virtual const std::string& GetName()const;//we pose as entry for themes ;)

			static sfg::Signal::SignalID OnTextSent;
			static sfg::Signal::SignalID OnHistoryUp;
			static sfg::Signal::SignalID OnHistoryDown;
		protected:
			PromptEntry(){}
		};


		enum                                            {MaxCache=50,MessagesPerLabel=100};
		void                                            onSend();
		//void                                            onChoose();
		void                                            onUp();
		void                                            onDown();
		void                                            setPromptChar(bool incomplete=false);
		void                                            cache();

		PromptEntry::Ptr                        m_Prompt;
		sfg::Box::Ptr                           m_Box;
		sfg::ScrolledWindow::Ptr        m_Scrolled;
		//sfg::ComboBox::Ptr                      m_Combo;
		sfg::Label::Ptr                         m_LastLabel;
		//sfg::Label::Ptr                         m_PromptChar;


		sf::String                                      m_Text;


		std::string                                     m_CachedLine;

		unsigned                                        m_MsgCount;

		int                                                     m_CacheIndex;


		std::deque<std::string>         m_Buffer;

	};
	*/