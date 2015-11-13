#pragma once

class DanApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();

	virtual int FilterEvent(wxEvent& event);
private:

	ConfigFile *cfg;
};