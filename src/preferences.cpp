#include "libs.h"
#include "animator.h"
#include "preferences.h"
#include "textualPanel.h"
#include "mainFrame.h"

CVAR(Bool, b_embedSprites, true, false, true, "Embed Sprite Resources into Project Files", CVAR_ARCHIVE);
CVAR(Bool, b_embedSounds, true, false, true, "Embed Sound Resources into Project Files", CVAR_ARCHIVE);
CVAR(Bool, dan_showSpritePreview, true, false, true, "Display a preview image of the selected sprite resource", CVAR_ARCHIVE);


PreferencesDialog::PreferencesDialog(wxWindow *parent) : wxDialog(parent, wxID_ANY, _T("Settings"), wxDefaultPosition, wxDefaultSize)
{
	prefsNotebook = new wxNotebook(this, wxID_ANY);

	prefSizer = new wxBoxSizer(wxVERTICAL);

	// Embedding
	embeddingPanel = new wxPanel(prefsNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
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
	renderingPanel = new wxPanel(prefsNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	renderingSizer = new wxBoxSizer(wxVERTICAL);
	bilinearCheckbox = new wxCheckBox(renderingPanel, wxID_ANY, _T("Use bilinear filtering"));
	bilinearCheckbox->SetValue(r_bilinear);
	previewCheckbox = new wxCheckBox(renderingPanel, wxID_ANY, _T("Show sprite preview above resource list"));
	previewCheckbox->SetValue(dan_showSpritePreview);
	renderingSizer->Add(bilinearCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 8);
	renderingSizer->Add(previewCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 8);
	renderingPanel->SetSizer(renderingSizer);

	// Warnings
	warningsPanel = new wxPanel(prefsNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	warningsSizer = new wxBoxSizer(wxVERTICAL);
	warnIfTxt = new wxStaticText(warningsPanel, wxID_ANY, _T("Display a warning message when:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	warnDeleteSpriteCheckbox	= new wxCheckBox(warningsPanel, wxID_ANY, _T("Deleting a sprite resource"));
	warnDeleteSoundCheckbox		= new wxCheckBox(warningsPanel, wxID_ANY, _T("Deleting a sound resource"));
	warnBadSpriteNamesCheckbox	= new wxCheckBox(warningsPanel, wxID_ANY, _T("Importing sprites with improper names"));
	warnBadTextureNamesCheckbox	= new wxCheckBox(warningsPanel, wxID_ANY, _T("Creating/renaming custom textures with improper names"));
	warnBadExportCheckbox		= new wxCheckBox(warningsPanel, wxID_ANY, _T("Exported animations contain bad code"));
	//warnBakeCheckbox			= new wxCheckBox(warningsPanel, wxID_ANY, _T("Baking internal offsets into a custom texture"));
	//warnImportTexturesCheckbox	= new wxCheckBox(warningsPanel, wxID_ANY, _T("Imported TEXTURES entries reference missing resources"));
	warnDeleteSpriteCheckbox->SetValue(warn_deletesprite);
	warnDeleteSoundCheckbox->SetValue(warn_deletesound);
	warnBadSpriteNamesCheckbox->SetValue(warn_badspritenames);
	warnBadTextureNamesCheckbox->SetValue(warn_badtexturename);
	warnBadExportCheckbox->SetValue(warn_badexport);
	//warnBakeCheckbox->SetValue(warn_bakesprite);
	//warnImportTexturesCheckbox->SetValue(warn_importtextures);
	warningsSizer->Add(warnIfTxt, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 8);
	warningsSizer->Add(warnDeleteSpriteCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 5);
	warningsSizer->Add(warnDeleteSoundCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 5);
	warningsSizer->Add(warnBadSpriteNamesCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 5);
	warningsSizer->Add(warnBadTextureNamesCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 5);
	warningsSizer->Add(warnBadExportCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 5);
	//warningsSizer->Add(warnBakeCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 5);
	//warningsSizer->Add(warnImportTexturesCheckbox, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 5);
	warningsPanel->SetSizer(warningsSizer);


	prefsNotebook->AddPage(embeddingPanel, _T("Embedding"), true);
	prefsNotebook->AddPage(renderingPanel, _T("Rendering"), false);
	prefsNotebook->AddPage(warningsPanel, _T("Warnings"), false);
	prefSizer->Add(prefsNotebook, 1, wxEXPAND);
	prefSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_RIGHT | wxBOTTOM | wxRIGHT, 4);
	this->SetSizerAndFit(prefSizer);
	//this->Show();
}
