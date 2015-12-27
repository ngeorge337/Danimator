#include "libs.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "textualPanel.h"
#include "codewindow.h"
#include "mainFrame.h"

extern DanFrame *theFrame;

wxBEGIN_EVENT_TABLE(DanCode, wxFrame)
EVT_CLOSE(DanCode::OnClose)
wxEND_EVENT_TABLE()

DanCode::DanCode(wxString &code, wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(parent, wxID_ANY, title, pos, size)
{
	codePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	codeSizer = new wxBoxSizer(wxVERTICAL);
	codeText = new wxTextCtrl(codePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);
	codeText->SetValue(code);
	codeSizer->Add(codeText, 1, wxEXPAND | wxALL, 8);
	codePanel->SetSizerAndFit(codeSizer);
	this->SetSize(size);
	this->Show();
}

void DanCode::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void DanCode::OnClose(wxCloseEvent& event)
{
	theFrame->codeView = nullptr;
	Destroy();
}
