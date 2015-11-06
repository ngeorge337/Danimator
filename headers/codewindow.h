#pragma once

class DanCode : public wxFrame
{
public:
	DanCode(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size);

	wxBoxSizer *codeSizer;
	wxPanel *codePanel;
	wxTextCtrl *codeText;

private:
	void OnExit(wxCommandEvent& event);
};