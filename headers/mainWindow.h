#pragma once

class DanApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
private:

	ConfigFile *cfg;
};