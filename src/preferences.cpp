#include "libs.h"
#include "animator.h"
#include "preferences.h"
#include "textualPanel.h"
#include "mainFrame.h"

CVAR(Bool, b_embedSprites, true, false, true, "Embed Sprite Resources into Project Files", CVAR_ARCHIVE);
CVAR(Bool, b_embedSounds, true, false, true, "Embed Sound Resources into Project Files", CVAR_ARCHIVE);



PreferencesDialog::PreferencesDialog(wxWindow *parent) : wxDialog(parent, wxID_ANY, _T("Settings"), wxDefaultPosition, wxDefaultSize)
{
	prefsNotebook = new wxNotebook(this, wxID_ANY);

	prefSizer = new wxBoxSizer(wxVERTICAL);

	// Embedding
	embeddingPanel = new wxPanel(prefsNotebook);
	embeddingSizer = new wxBoxSizer(wxVERTICAL);

	embedSpritesCheckbox = new wxCheckBox(embeddingPanel, wxID_ANY, _T("Embed Sprites into Project Files"));
	embedSpritesCheckbox->SetValue(b_embedSprites);
	embedSoundsCheckbox = new wxCheckBox(embeddingPanel, wxID_ANY, _T("Embed Sounds into Project Files"));
	embedSoundsCheckbox->SetValue(b_embedSounds);

	embedWarnTxt = new wxStaticText(embeddingPanel, wxID_ANY, _T("Embedding resources into the project can prevent problems if sprites or sounds were to be moved from their originally loaded location. However, this will bloat the file size, depending on the amount of resources loaded."), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	embedWarnTxt->Wrap(300);

	embeddingSizer->Add(embedSpritesCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 8);
	embeddingSizer->Add(embedSoundsCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 8);
	embeddingSizer->AddSpacer(48);
	embeddingSizer->Add(embedWarnTxt, 0, wxALIGN_LEFT | wxLEFT | wxBOTTOM | wxRIGHT, 8);
	embeddingPanel->SetSizer(embeddingSizer);

	// Rendering
	renderingPanel = new wxPanel(prefsNotebook);
	renderingSizer = new wxBoxSizer(wxVERTICAL);
	bilinearCheckbox = new wxCheckBox(renderingPanel, wxID_ANY, _T("Use bilinear filtering"));
	bilinearCheckbox->SetValue(r_bilinear);
	renderingSizer->Add(bilinearCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 8);
	renderingPanel->SetSizer(renderingSizer);


	prefsNotebook->AddPage(embeddingPanel, _T("Embedding"), true);
	prefsNotebook->AddPage(renderingPanel, _T("Rendering"), false);
	prefSizer->Add(prefsNotebook, 1, wxEXPAND);
	prefSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_RIGHT | wxBOTTOM | wxRIGHT, 4);
	this->SetSizerAndFit(prefSizer);
	//this->Show();
}
