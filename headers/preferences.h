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
	wxCheckBox *previewCheckbox;

	// Editor Warnings options
	wxPanel *warningsPanel;
	wxStaticText *warnIfTxt;
	wxCheckBox *warnDeleteSpriteCheckbox;
	wxCheckBox *warnDeleteSoundCheckbox;
	wxCheckBox *warnBadSpriteNamesCheckbox;
	wxCheckBox *warnBadTextureNamesCheckbox;
	wxCheckBox *warnBadExportCheckbox;
	wxCheckBox *warnBakeCheckbox;
	wxCheckBox *warnImportTexturesCheckbox;

	// Sizers
	wxBoxSizer *prefSizer;
	wxBoxSizer *embeddingSizer;
	wxBoxSizer *renderingSizer;
	wxBoxSizer *warningsSizer;

private:
};

