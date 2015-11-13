#pragma once

class DanCode : public wxFrame
{
public:
	DanCode(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size);

	wxBoxSizer *codeSizer;
	wxPanel *codePanel;
	wxTextCtrl *codeText;

	wxDECLARE_EVENT_TABLE();

private:
	void OnExit(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
};