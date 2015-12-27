#include "libs.h"
#include "animator.h"
#include "batchaction.h"
#include "mainFrame.h"

extern DanFrame *theFrame;

wxBEGIN_EVENT_TABLE(BatchActionDialog, wxDialog)
EVT_CHOICE(ID_MODCHOICE, BatchActionDialog::OnDataChange)
EVT_SPINCTRL(ID_XOFF, BatchActionDialog::OnXoffSpin)
EVT_SPINCTRL(ID_YOFF, BatchActionDialog::OnYoffSpin)
EVT_SPINCTRL(ID_TICKOFF, BatchActionDialog::OnTicoffSpin)
wxEND_EVENT_TABLE()

BatchActionDialog::BatchActionDialog(wxWindow *parent) : wxDialog(parent, wxID_ANY, _T("Batch Action"), wxDefaultPosition, wxDefaultSize)
{
	batchMainSizer = new wxBoxSizer(wxVERTICAL);

	batchPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

	// Sizers
	batchSizer = new wxBoxSizer(wxVERTICAL);
	offsetsSizer = new wxStaticBoxSizer(wxHORIZONTAL, batchPanel, _T("Offsets"));
	ticsSizer = new wxStaticBoxSizer(wxHORIZONTAL, batchPanel, _T("Tics"));

	// Labels
	//xChangeLabel = new wxStaticText(offsetsSizer->GetStaticBox(), wxID_ANY, _T("X:"), wxDefaultPosition, wxDefaultSize);
	//yChangeLabel = new wxStaticText(offsetsSizer->GetStaticBox(), wxID_ANY, _T("Y:"), wxDefaultPosition, wxDefaultSize);
	xChangeLabel = new wxCheckBox(offsetsSizer->GetStaticBox(), wxID_ANY, _T("X:"), wxDefaultPosition, wxDefaultSize);
	yChangeLabel = new wxCheckBox(offsetsSizer->GetStaticBox(), wxID_ANY, _T("Y:"), wxDefaultPosition, wxDefaultSize);
	xChangeLabel->SetValue(true);
	yChangeLabel->SetValue(true);
	framesLabel = new wxStaticText(batchPanel, wxID_ANY, _T("Frames:"), wxDefaultPosition, wxDefaultSize);
	thruLabel = new wxStaticText(batchPanel, wxID_ANY, _T("thru"), wxDefaultPosition, wxDefaultSize);
	modifyLabel = new wxStaticText(batchPanel, wxID_ANY, _T("Modify:"), wxDefaultPosition, wxDefaultSize);

	// Controls
	//timelineSelection = new wxSlider(batchPanel, ID_TIMESELECTION, 1, 1, 3, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_MIN_MAX_LABELS | wxSL_SELRANGE);
	//timelineSelection->SetMax(theFrame->timelineSlider->GetMax());
	//timelineSelection->SetSelection(1, 2);

	wxArrayString modChoices;
	modChoices.Add("Offsets");
	modChoices.Add("Tics");
	dataChoice = new wxChoice(batchPanel, ID_MODCHOICE, wxDefaultPosition, wxSize(104, 24), modChoices);
	dataChoice->SetSelection(0);

	frameStartEntry = new wxSpinCtrl(batchPanel, wxID_ANY, "1", wxDefaultPosition, wxSize(50, 24), wxTE_PROCESS_ENTER | wxALIGN_RIGHT | wxSP_ARROW_KEYS, 1, 65535, 0);
	frameEndEntry = new wxSpinCtrl(batchPanel, wxID_ANY, "2", wxDefaultPosition, wxSize(50, 24), wxTE_PROCESS_ENTER | wxALIGN_RIGHT | wxSP_ARROW_KEYS, 1, 65535, 0);
	//frameStartEntry = new wxTextCtrl(batchPanel, wxID_ANY, "1", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
	//frameStartEntry->SetValue(std::to_string(timelineSelection->GetSelStart()));
	//frameEndEntry = new wxTextCtrl(batchPanel, wxID_ANY, "2", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
	//frameEndEntry->SetValue(std::to_string(timelineSelection->GetSelEnd()));
	frameEndEntry->SetValue(theFrame->timelineSlider->GetMax());
	frameEndEntry->SetMax(theFrame->timelineSlider->GetMax());
	frameStartEntry->SetMax(theFrame->timelineSlider->GetMax());

	xEntry = new wxSpinCtrl(offsetsSizer->GetStaticBox(), ID_XOFF, "0", wxDefaultPosition, wxSize(50, 24), wxTE_PROCESS_ENTER | wxALIGN_RIGHT | wxSP_ARROW_KEYS, -65535, 65535, 0);
	yEntry = new wxSpinCtrl(offsetsSizer->GetStaticBox(), ID_YOFF, "0", wxDefaultPosition, wxSize(50, 24), wxTE_PROCESS_ENTER | wxALIGN_RIGHT | wxSP_ARROW_KEYS, -65535, 65535, 0);
	ticsEntry = new wxSpinCtrl(ticsSizer->GetStaticBox(), ID_TICKOFF, "0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxALIGN_RIGHT | wxSP_ARROW_KEYS, -65535, 65535, 0);

	wxArrayString applyAs;
	applyAs.Add("Relative");
	applyAs.Add("Absolute");
	changeRadio = new wxRadioBox(batchPanel, wxID_ANY, "Apply As", wxDefaultPosition, wxDefaultSize, applyAs, 2, wxRA_SPECIFY_COLS);

	//batchSizer->Add(timelineSelection, 1, wxEXPAND | wxALL, 4);

	wxBoxSizer *framesSizer = new wxBoxSizer(wxHORIZONTAL);
	framesSizer->Add(framesLabel, 0, wxALIGN_CENTER_VERTICAL);
	framesSizer->AddSpacer(4);
	framesSizer->Add(frameStartEntry, 0, wxALIGN_CENTER_VERTICAL);
	framesSizer->AddSpacer(2);
	framesSizer->Add(thruLabel, 0, wxALIGN_CENTER_VERTICAL);
	framesSizer->AddSpacer(2);
	framesSizer->Add(frameEndEntry, 0, wxALIGN_CENTER_VERTICAL);

	batchSizer->Add(framesSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 4);

	wxBoxSizer *dataSizer = new wxBoxSizer(wxHORIZONTAL);
	dataSizer->Add(modifyLabel, 0, wxALIGN_CENTER_VERTICAL);
	dataSizer->AddSpacer(4);
	dataSizer->Add(dataChoice, 1, wxEXPAND);

	batchSizer->Add(dataSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 4);

	offsetsSizer->Add(xChangeLabel, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 4);
	offsetsSizer->AddSpacer(4);
	offsetsSizer->Add(xEntry, 0, wxALIGN_CENTER_VERTICAL);
	offsetsSizer->AddSpacer(8);
	offsetsSizer->Add(yChangeLabel, 0, wxALIGN_CENTER_VERTICAL);
	offsetsSizer->AddSpacer(4);
	offsetsSizer->Add(yEntry, 0, wxALIGN_CENTER_VERTICAL);

	batchSizer->Add(offsetsSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxBOTTOM, 4);

	ticsSizer->Add(ticsEntry, 1, wxEXPAND | wxALL, 4);
	batchSizer->Add(ticsSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxBOTTOM, 4);

	wxBoxSizer *radioSizer = new wxBoxSizer(wxVERTICAL);
	radioSizer->Add(changeRadio, 1, wxALIGN_CENTER_HORIZONTAL);
	batchSizer->Add(radioSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 4);

	// Finish
	batchPanel->SetSizerAndFit(batchSizer);
	batchMainSizer->Add(batchPanel, 1, wxEXPAND);
	batchMainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_RIGHT | wxLEFT | wxBOTTOM | wxRIGHT, 4);

	this->SetSizerAndFit(batchMainSizer);

	//ticsEntry->Disable();
	ticsSizer->Show(false);

	this->Layout();
	this->Fit();
}

void BatchActionDialog::OnDataChange(wxCommandEvent &event)
{
	switch(dataChoice->GetSelection())
	{
	case 0:
		//xEntry->Enable();
		//yEntry->Enable();
		//ticsEntry->Disable();
		offsetsSizer->Show(true);
		ticsSizer->Show(false);
		break;
	case 1:
		//xEntry->Disable();
		//yEntry->Disable();
		//ticsEntry->Enable();
		offsetsSizer->Show(false);
		ticsSizer->Show(true);
		break;
	}
	this->Layout();
	this->Fit();
	return;
}
