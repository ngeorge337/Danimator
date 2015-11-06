#include "libs.h"
#include "animator.h"
#include "codewindow.h"
#include "mainFrame.h"

extern DanFrame *theFrame;

DanCode::DanCode(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(parent, wxID_ANY, title, pos, size)
{
	codePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	codeSizer = new wxBoxSizer(wxVERTICAL);
	codeText = new wxTextCtrl(codePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);
	codeText->SetValue(theFrame->GetDecorateCode(theFrame->StateListCtrl->GetStringSelection().ToStdString()));
	codeSizer->Add(codeText, 1, wxEXPAND | wxALL, 8);
	codePanel->SetSizerAndFit(codeSizer);
	this->SetSize(wxSize(300, 450));
	this->Show();
}

void DanCode::OnExit(wxCommandEvent& event)
{
	Close(true);
}