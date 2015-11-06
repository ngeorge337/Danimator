#include "libs.h"
#include "animator.h"
#include "mainFrame.h"
#include "mainWindow.h"

DanFrame *theFrame;

bool DanApp::OnInit()
{
	// Initialization
	wxImage::AddHandler(new wxPNGHandler);
	ThreadPool::Init();
	Locator::Init();
	Audio::Init();
	Camera::Init(sf::FloatRect(0, 0, 320, 240));

	cfg = new ConfigFile("config.cfg", FM_READ);
	cfg->ReadConfig();
	Cvars->ReadCVars(*cfg);
	CCMDMan->ReadCCMDs(*cfg);

	// Make the initial frame (toplevel window)
	theFrame = new DanFrame("Danimator", wxPoint(0, 0), wxSize(1096, 600));
	theFrame->SetSize(wxSize(1096, 600));
	//theFrame->SetMinSize(wxSize(1088, 600));

	//rw.create(theFrame->glCanvas->GetHandle(), sf::ContextSettings(24, 8, 0, 2, 1));

	// Show the window, we are finished making it
	theFrame->Show(true);
	
	return true;
}

int DanApp::OnExit()
{
	cfg->Clear();
	cfg->Add(Cvars->ReadCvarList());
	cfg->WriteConfig();
	return wxApp::OnExit();
}