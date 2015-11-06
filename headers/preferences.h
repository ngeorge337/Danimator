#pragma once

class PreferencesDialog : public wxDialog
{
public:
	PreferencesDialog(wxWindow *parent);

	wxNotebook *prefsNotebook;

	// Embedding options
	wxPanel *embeddingPanel;
	wxCheckBox *embedSpritesCheckbox;
	wxCheckBox *embedSoundsCheckbox;
	wxStaticText *embedWarnTxt;

	// Rendering options
	wxPanel *renderingPanel;
	wxCheckBox *bilinearCheckbox;

	// Sizers
	wxBoxSizer *prefSizer;
	wxBoxSizer *embeddingSizer;
	wxBoxSizer *renderingSizer;

private:
};

