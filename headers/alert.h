#pragma once

#include "libs.h"

// Static alerting class that creates alert messages
// also sets a modal value to block hotkeys
class DanAlert
{
public:
	inline static bool InAlert()
	{
		return inAlert;
	}
	inline static void Alert(const wxString &msg, const wxString &title = "Warning")
	{
		inAlert = true;
		wxMessageBox(msg, title, wxOK | wxICON_WARNING);
		inAlert = false;
	}
	inline static bool AlertIf(bool testValue, const wxString &msg, const wxString &title = "Warning")
	{
		if(testValue)
		{
			wxString newmsg = msg;
			newmsg += "\n\n(This warning and others can be disabled in the settings dialog)";
			inAlert = true;
			if(wxMessageBox(newmsg, title, wxOK | wxCANCEL | wxICON_WARNING) == wxCANCEL)
			{
				inAlert = false;
				return true;
			}
			inAlert = false;
		}
		return false;
	}
	inline static void AlertIfNoAbort(bool testValue, const wxString &msg, const wxString &title = "Warning")
	{
		if(testValue)
		{
			wxString newmsg = msg;
			newmsg += "\n\n(This warning and others can be disabled in the settings dialog)";
			inAlert = true;
			wxMessageBox(newmsg, title, wxOK | wxICON_WARNING);
			inAlert = false;
		}
	}
private:
	static bool inAlert;
};
