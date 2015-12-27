#pragma once

class BatchActionDialog : public wxDialog
{
public:
	BatchActionDialog(wxWindow *parent);
	
	wxPanel *batchPanel;

	//wxStaticText *xChangeLabel;
	//wxStaticText *yChangeLabel;
	wxCheckBox *xChangeLabel;
	wxCheckBox *yChangeLabel;

	wxSlider *timelineSelection;
	wxStaticText *framesLabel;
	//wxTextCtrl *frameStartEntry;
	wxSpinCtrl*frameStartEntry;
	wxStaticText *thruLabel;
	//wxTextCtrl *frameEndEntry;
	wxSpinCtrl*frameEndEntry;
	wxStaticText *modifyLabel;
	wxChoice *dataChoice;
	wxSpinCtrl *xEntry;
	wxSpinCtrl *yEntry;
	wxSpinCtrl *ticsEntry;

	wxRadioBox *changeRadio;
	
	// Sizers
	wxBoxSizer *batchMainSizer;
	wxBoxSizer *batchSizer;
	wxStaticBoxSizer *offsetsSizer;
	wxStaticBoxSizer *ticsSizer;

	wxDECLARE_EVENT_TABLE();
private:
	void OnDataChange(wxCommandEvent &event);
	void OnXoffSpin(wxSpinEvent &event)
	{
	}
	void OnYoffSpin(wxSpinEvent &event)
	{
	}
	void OnTicoffSpin(wxSpinEvent &event)
	{
	}
};

enum
{
	ID_TIMESELECTION = 5200,
	ID_MODCHOICE,
	ID_XOFF,
	ID_YOFF,
	ID_TICKOFF,	// you tick me off, m9
};