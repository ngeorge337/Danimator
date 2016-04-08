#include "libs.h"
#include "alert.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "textualPanel.h"
#include "mainFrame.h"
#include "codewindow.h"
#include "preferences.h"
#include "actionmanager.h"

extern DanFrame *theFrame;

wxBEGIN_EVENT_TABLE(DanFrame, wxFrame)

// Frame-specific Events
EVT_TIMER(ID_STATUS_TIMER, DanFrame::OnTimer)
EVT_IDLE(DanFrame::OnIdle)
EVT_CLOSE(DanFrame::OnClose)

// Frame Menu Events
EVT_MENU(wxID_EXIT, DanFrame::OnExit)
EVT_MENU(ID_SETTINGS, DanFrame::OnSettings)
EVT_MENU(ID_NEWPROJECT, DanFrame::OnNewProject)
EVT_MENU(ID_OPENPROJECT, DanFrame::OnOpenProject)
EVT_MENU(ID_SAVEPROJECT, DanFrame::OnSaveProject)
EVT_MENU(ID_SAVEPROJECTAS, DanFrame::OnSaveProjectAs)
EVT_MENU(ID_EXPORTRESOURCES, DanFrame::OnExportResources)
EVT_MENU(ID_IMPORTRESOURCES, DanFrame::OnImportResources)
EVT_MENU(ID_IMPORTSNDINFO, DanFrame::OnImportSndinfo)
EVT_MENU(ID_UNDO, DanFrame::OnUndo)
EVT_MENU(ID_REDO, DanFrame::OnRedo)
EVT_MENU(wxID_ABOUT, DanFrame::OnAbout)

// Buttons
EVT_BUTTON(ID_APPLYSPRITEBTN, DanFrame::OnApplySprite)
EVT_BUTTON(ID_APPLYSOUNDBTN, DanFrame::OnApplySound)
EVT_BUTTON(ID_CREATETEXTURE, DanFrame::OnCreateTexture)
EVT_BUTTON(ID_NEWSTATE, DanFrame::OnNewState)
EVT_BUTTON(ID_DELSTATE, DanFrame::OnDeleteState)
EVT_BUTTON(ID_ADDSPRITE, DanFrame::OnAddSprite)
EVT_BUTTON(ID_DELSPRITE, DanFrame::OnDeleteSprite)
EVT_BUTTON(ID_EXPORTTEXTURES, DanFrame::OnExportTextures)
EVT_BUTTON(ID_ADDSOUND, DanFrame::OnAddSound)
EVT_BUTTON(ID_DELSOUND, DanFrame::OnDeleteSound)
EVT_BUTTON(ID_ADDFRAME, DanFrame::OnAddFrame)
EVT_BUTTON(ID_DELFRAME, DanFrame::OnDeleteFrame)
EVT_BUTTON(ID_VIEWCODE, DanFrame::OnViewCode)
EVT_BUTTON(ID_EXPORTCODE, DanFrame::OnExportCode)
EVT_BUTTON(ID_NEXTFRAME, DanFrame::OnNextFrame)
EVT_BUTTON(ID_PREVFRAME, DanFrame::OnPrevFrame)
EVT_BUTTON(ID_PLAY, DanFrame::OnPlay)
EVT_BUTTON(ID_PLAYFROMSTART, DanFrame::OnPlayFromStart)
EVT_BUTTON(ID_INSERTFRAME, DanFrame::OnInsertFrame)
EVT_BUTTON(ID_BATCHACTION, DanFrame::OnBatchAction)

// List Events
EVT_LIST_ITEM_SELECTED(ID_STATELIST, DanFrame::OnStateSelection)
EVT_LIST_ITEM_SELECTED(ID_SELECTSPRITE, DanFrame::OnSpriteClicked)
EVT_LIST_ITEM_DESELECTED(ID_SELECTSPRITE, DanFrame::OnSpriteDeselect)
EVT_LIST_ITEM_SELECTED(ID_SELECTSOUND, DanFrame::OnSoundClicked)
EVT_LIST_ITEM_ACTIVATED(ID_SELECTSPRITE, DanFrame::OnSelectSprite)
EVT_LIST_ITEM_ACTIVATED(ID_SELECTSOUND, DanFrame::OnSelectSound)
EVT_LIST_ITEM_DESELECTED(ID_STATELIST, DanFrame::OnStateDeselect)

// Spin Events
EVT_SPINCTRL(ID_XSPIN, DanFrame::OnXSpinChange)
EVT_SPINCTRL(ID_YSPIN, DanFrame::OnYSpinChange)
EVT_SPINCTRL(ID_DURATION, DanFrame::OnDurationSpinChange)

// Slider Events
EVT_SLIDER(ID_TIMESLIDER, DanFrame::OnSliderChange)

// Choice/Combo Box Events
EVT_CHOICE(ID_ENDING, DanFrame::OnFlowGoto)

// Context Menus
EVT_MENU(ID_CONTEXT_STATE_RENAME, DanFrame::OnStateRename)
EVT_MENU(ID_CONTEXT_STATE_DUPLICATE, DanFrame::OnStateDuplicate)
EVT_MENU(ID_CONTEXT_STATE_VIEWCODE, DanFrame::OnViewCode)
EVT_MENU(ID_CONTEXT_STATE_EXPORTGIF, DanFrame::OnExportGIF)
EVT_MENU(ID_CONTEXT_SPRITE_VIEWCODE, DanFrame::OnSpriteViewCode)
EVT_MENU(ID_CONTEXT_SPRITE_EDIT, DanFrame::OnSpriteEdit)
EVT_MENU(ID_CONTEXT_SPRITE_RENAME, DanFrame::OnSpriteRename)
EVT_MENU(ID_CONTEXT_SPRITE_DUPLICATE, DanFrame::OnSpriteDuplicate)
EVT_MENU(ID_CONTEXT_SOUND_RENAME, DanFrame::OnSoundRename)


// Unused
//EVT_LISTBOX_DCLICK(ID_STATELIST, DanFrame::OnStateSelection)
//EVT_RIGHT_UP(DanFrame::OnRightUp)
//EVT_LIST_ITEM_RIGHT_CLICK(ID_STATELIST, DanFrame::OnStateContext)
//EVT_CONTEXT_MENU(DanFrame::OnContextMenu)
//EVT_KEY_DOWN(DanFrame::CaughtKeys)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(DanStatusBar, wxStatusBar)
EVT_SIZE(DanStatusBar::OnSize)
EVT_SLIDER(ID_STATUS_ZOOM, DanStatusBar::OnZoomSliderChange)
wxEND_EVENT_TABLE()

int DanFrame::OnHotkey(wxKeyEvent& event)
{
	if(inModal || DanAlert::InAlert())
		return -1;

	wxChar keyid = event.GetKeyCode();

	if(editorMode == EDITMODE_ANIMATION)
	{
		// Play/Stop -> Space
		if(keyid == WXK_SPACE)
		{
			if(event.GetModifiers() == wxMOD_CONTROL) OnPlayFromStart(wxCommandEvent());
			else OnPlay(wxCommandEvent());
			event.Skip();
			return true;
		}

		else if(!playingMode)
		{
			// Copy Frame -> Ctrl-C
			if(keyid == 'C' && event.GetModifiers() == wxMOD_CONTROL)
			{
				OnCopyFrame(wxCommandEvent());
				event.Skip();
				return true;
			}

			// Paste Frame -> Ctrl-V
			if(keyid == 'V' && event.GetModifiers() == wxMOD_CONTROL)
			{
				OnPasteFrame(wxCommandEvent());
				event.Skip();
				return true;
			}

			// Save -> Ctrl-S
			if(keyid == 'S' && event.GetModifiers() == wxMOD_CONTROL)
			{
				OnSaveProject(wxCommandEvent());
				event.Skip();
				return true;
			}

			// New Project -> Ctrl-N
			else if(keyid == 'N' && event.GetModifiers() == wxMOD_CONTROL)
			{
				OnNewProject(wxCommandEvent());
				event.Skip();
				return true;
			}

			// Open Project -> Ctrl-O
			else if(keyid == 'O' && event.GetModifiers() == wxMOD_CONTROL)
			{
				OnOpenProject(wxCommandEvent());
				event.Skip();
				return true;
			}

			// Undo -> Ctrl-Z
			else if(keyid == 'Z' && event.GetModifiers() == wxMOD_CONTROL)
			{
				Locator::GetActionManager()->Undo();
				event.Skip();
				return true;
			}

			// Redo -> Ctrl-Y
			else if(keyid == 'Y' && event.GetModifiers() == wxMOD_CONTROL)
			{
				Locator::GetActionManager()->Redo();
				event.Skip();
				return true;
			}
		}
	}
	else if(editorMode == EDITMODE_TEXTURES)
	{
		// Undo -> Ctrl-Z
		if(keyid == 'Z' && event.GetModifiers() == wxMOD_CONTROL)
		{
			textualPanel->GetActiveTexture()->GetActionHistory().Undo();
			event.Skip();
			return true;
		}

		// Redo -> Ctrl-Y
		else if(keyid == 'Y' && event.GetModifiers() == wxMOD_CONTROL)
		{
			textualPanel->GetActiveTexture()->GetActionHistory().Redo();
			event.Skip();
			return true;
		}
	}
	return -1;
}

void DanFrame::OnTimer(wxTimerEvent& event)
{
	DanStatus->SetStatusText("Ready");
}

void DanFrame::OnIdle(wxIdleEvent& event)
{
	wxString frm;
	switch(editorMode)
	{
	case EDITMODE_ANIMATION:
		frm = "Frame: ";
		if(playingMode) frm += "Playing";
		else
			frm += timelineSlider->IsEnabled() ? std::to_string(timelineSlider->GetValue()) : "1";
		break;
	case EDITMODE_TEXTURES:
		frm = "Editing Texture: ";
		frm += textualPanel->m_textureName;
		break;
	}
	DanStatus->SetStatusText(frm, 1);	
}

void DanFrame::OnNewProject(wxCommandEvent& event)
{
	if(!saved)
	{
		wxString str = "your project";
		if(!projectName.empty()) str = projectName;
		wxMessageDialog *choice = new wxMessageDialog(this, _T("Would you like to save changes to " + str + "?"), _T("Save?"), wxICON_QUESTION | wxYES_NO | wxCANCEL);
		inModal = true;
		int ret = choice->ShowModal();
		if(ret == wxID_YES)
		{
			if(projectName.empty())
			{
				const wxChar *Filters = _T(
					"Danimator Projects \t(*.danproj)|*.danproj"
					);
				wxFileDialog *fd = new wxFileDialog(this, _T("Save Project As"), wxEmptyString, wxEmptyString, Filters, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
				if(fd->ShowModal() == wxID_OK)
				{
					SaveProject(fd->GetFilename());
					projectName = fd->GetFilename();
					saved = true;
				}
			}
			else
			{
				SaveProject(projectName);
				saved = true;
			}
		}
		else if(ret == wxID_CANCEL)
		{
			inModal = false;
			return;
		}
	}
	inModal = false;
	SetEditorMode(EDITMODE_ANIMATION);
	spritePreviewer->ClearImage();
	spritePreviewer->Refresh();
	textualPanel->ResetControls();
	StateListCtrl->DanDeleteAllItems();
	SpritesListCtrl->DeleteAllItems();
	SoundsListCtrl->DeleteAllItems();
	timelineSlider->SetMax(2);
	timelineSlider->SetValue(1);
	xSpin->SetValue(0);
	ySpin->SetValue(0);
	durationSpin->SetValue(1);
	currentSpriteCtrl->SetValue("");
	currentSoundCtrl->SetValue("");
	animator.ClearStates();
	Locator::GetTextureManager()->UnloadAll();
	Locator::GetTextureManager()->CreateEmptyTexture("TNT1A0");
	Locator::GetActionManager()->Wipe();
	Audio::ResetSlots();
	Locator::GetSoundManager()->UnloadAll();
	SpritesListCtrl->InsertItem(0, wxString("* No Sprite (TNT1A0)"));
	SpritesListCtrl->SetItemData(0, CreateSortData("\0", -9999));
	SoundsListCtrl->InsertItem(0, wxString("* No Sound"));
	SoundsListCtrl->SetItemData(0, CreateSortData("\0", -9999));

	this->SetTitle("Danimator");

	SetStartupMode(true);
	inStartup = true;
	saved = true;
}

void DanFrame::OnOpenProject(wxCommandEvent& event)
{
	if(!saved)
	{
		wxString str = "your project";
		if(!projectName.empty()) str = projectName;
		wxMessageDialog *choice = new wxMessageDialog(this, _T("Would you like to save changes to " + str + "?"), _T("Save?"), wxICON_QUESTION | wxYES_NO | wxCANCEL);
		inModal = true;
		int ret = choice->ShowModal();
		if(ret == wxID_YES)
		{
			if(projectName.empty())
			{
				const wxChar *Filters = _T(
					"Danimator Projects \t(*.danproj)|*.danproj"
					);
				wxFileDialog *fd = new wxFileDialog(this, _T("Save Project As"), wxEmptyString, wxEmptyString, Filters, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
				if(fd->ShowModal() == wxID_OK)
				{
					SaveProject(fd->GetFilename());
					projectName = fd->GetFilename();
					saved = true;
				}
			}
			else
			{
				SaveProject(projectName);
				saved = true;
			}
		}
		else if(ret == wxID_CANCEL)
		{
			inModal = false;
			return;
		}
	}

	const wxChar *Filters = _T(
		"Danimator Projects \t(*.danproj)|*.danproj"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Open Project"), wxEmptyString, wxEmptyString, Filters, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
	if(fd->ShowModal() == wxID_OK)
	{
		// Reset everything to defaults
		StateListCtrl->DanDeleteAllItems();
		SpritesListCtrl->DeleteAllItems();
		SoundsListCtrl->DeleteAllItems();
		timelineSlider->SetMax(2);
		timelineSlider->SetValue(1);
		xSpin->SetValue(0);
		ySpin->SetValue(0);
		durationSpin->SetValue(1);
		currentSpriteCtrl->SetValue("");
		currentSoundCtrl->SetValue("");
		animator.ClearStates();
		Locator::GetTextureManager()->UnloadAll();
		Locator::GetTextureManager()->CreateEmptyTexture("TNT1A0");
		Audio::ResetSlots();
		Locator::GetSoundManager()->UnloadAll();
		Locator::GetActionManager()->Wipe();
		SpritesListCtrl->InsertItem(0, wxString("* No Sprite (TNT1A0)"));
		SpritesListCtrl->SetItemData(0, CreateSortData("\0", -9999));
		SoundsListCtrl->InsertItem(0, wxString("* No Sound"));
		SoundsListCtrl->SetItemData(0, CreateSortData("\0", -9999));

		// Load project information
		LoadProject(fd->GetFilename());
		projectName = fd->GetFilename();
		saved = true;
		this->SetTitle(wxString(StripStringExtension(projectName.ToStdString())) + " -- Danimator");

		if(StateListCtrl->GetItemCount() > 0)
		{
			StateListCtrl->Select(0, true);
			animator.SetState(StateListCtrl->GetItemText(StateListCtrl->GetFirstSelected()).ToStdString());
			// We didn't have any states in this project somehow, so let's abort setting controls up
			if(animator.GetCurrentState() == nullptr)
				return;
			SetStartupMode(false);
		}

		// Set all controls to our current state and frame
		UpdateTimeline();
		UpdateSpins();
		UpdateFrameInfo();
		UpdateFlow();
		timelineSlider->SetValue(animator.GetCurrentState()->frameOffset + 1);
	}
	inModal = false;
}

void DanFrame::OnSaveProject(wxCommandEvent& event)
{
	if(inStartup) return;
	if(projectName.empty())
	{
		inModal = true;
		const wxChar *Filters = _T(
			"Danimator Projects \t(*.danproj)|*.danproj"
			);
		wxFileDialog *fd = new wxFileDialog(this, _T("Save Project As"), wxEmptyString, wxEmptyString, Filters, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
		if(fd->ShowModal() == wxID_OK)
		{
			SaveProject(fd->GetFilename());
			projectName = fd->GetFilename();
			saved = true;
			ActionStatus("Project saved.");
		}
		inModal = false;
	}
	else
	{
		SaveProject(projectName);
		saved = true;
		ActionStatus("Project saved.");
	}
}

void DanFrame::OnSaveProjectAs(wxCommandEvent& event)
{
	if(inStartup) return;
	inModal = true;
	const wxChar *Filters = _T(
		"Danimator Projects \t(*.danproj)|*.danproj"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Save Project As"), wxEmptyString, wxEmptyString, Filters, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
	if(fd->ShowModal() == wxID_OK)
	{
		SaveProject(fd->GetFilename());
		projectName = fd->GetFilename();
		saved = true;
		ActionStatus("Project saved.");
		this->SetTitle(wxString(StripStringExtension(projectName.ToStdString())) + " -- Danimator");
	}
	inModal = false;
}

void DanFrame::OnSettings(wxCommandEvent& event)
{
	PreferencesDialog *pd = new PreferencesDialog(this);
	pd->CenterOnParent();
	inModal = true;
	if(pd->ShowModal() == wxID_OK)
	{
		b_embedSprites = pd->embedSpritesCheckbox->GetValue();
		b_embedSounds = pd->embedSoundsCheckbox->GetValue();
		r_bilinear = pd->bilinearCheckbox->GetValue();
		dan_showSpritePreview = pd->previewCheckbox->GetValue();

		warn_deletesprite = pd->warnDeleteSpriteCheckbox->GetValue();
		warn_deletesound = pd->warnDeleteSoundCheckbox->GetValue();
		warn_badspritenames = pd->warnBadSpriteNamesCheckbox->GetValue();
		warn_badtexturename = pd->warnBadTextureNamesCheckbox->GetValue();
		warn_badexport = pd->warnBadExportCheckbox->GetValue();
		//warn_bakesprite = pd->warnBakeCheckbox->GetValue();
		//warn_importtextures = pd->warnImportTexturesCheckbox->GetValue();


		spritePreviewer->Show(dan_showSpritePreview);
		spritePanel->Layout();
		Locator::GetTextureManager()->SetFiltering(r_bilinear);
	}
	inModal = false;
}

void DanFrame::OnExit(wxCommandEvent& event)
{
	if(!saved)
	{
		wxString str = "your project";
		if(!projectName.empty()) str = projectName;
		wxMessageDialog *choice = new wxMessageDialog(this, _T("Would you like to save changes to " + str + "?"), _T("Save?"), wxICON_QUESTION | wxYES_NO | wxCANCEL);
		int ret = choice->ShowModal();
		if(ret == wxID_YES)
		{
			if(projectName.empty())
			{
				const wxChar *Filters = _T(
					"Danimator Projects \t(*.danproj)|*.danproj"
					);
				wxFileDialog *fd = new wxFileDialog(this, _T("Save Project As"), wxEmptyString, wxEmptyString, Filters, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
				if(fd->ShowModal() == wxID_OK)
				{
					SaveProject(fd->GetFilename());
					projectName = fd->GetFilename();
					saved = true;
				}
			}
			else
			{
				SaveProject(projectName);
				saved = true;
			}
		}
		else if(ret == wxID_CANCEL)
			return;
	}
	Audio::ResetSlots();
	Close(true);
}

void DanFrame::OnAbout(wxCommandEvent& event)
{
	inModal = true;
	wxMessageBox("Danimator\nA friggin' ZDoom Animation Program\n\n© 2015 Nick George\n\nButton graphics by Wartorn\n\nVersion " DANVERSION,
				 "About Danimator", wxOK | wxICON_INFORMATION);
	inModal = false;
}

void DanFrame::OnNewState(wxCommandEvent& event)
{
	wxTextEntryDialog *txt = new wxTextEntryDialog(this, _T("Enter the State Name:"), _T("New State"), wxEmptyString);
	inModal = true;
	if(txt->ShowModal() == wxID_OK)
	{
		if(txt->GetValue().IsEmpty())
		{
			inModal = false;
			return;
		}

		std::string st = RemoveWhiteSpace(txt->GetValue().ToStdString());

		if(animator.IsValidState(st))
		{
			wxMessageBox("Failed to add new state -- Label already exists!",
						 "New State", wxOK | wxICON_WARNING);
			inModal = false;
			return;
		}

		bool selectIt = (animator.m_validStates.empty() || StateListCtrl->GetFirstSelected() == -1 ? true : false);

		Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Create state - %s", st));
		animator.CreateState(st)->AddEmptyFrame();
		StateListCtrl->DanPushBack(wxString(st));

		saved = false;

		if(!animator.m_validStates.empty()) SetStartupMode(false);
		if(selectIt)
		{
			StateListCtrl->Select(0, true);
			animator.SetState(StateListCtrl->GetItemText(StateListCtrl->GetFirstSelected()).ToStdString());
			timelineSlider->SetValue(1);
			ResetFrame();
			playFrame = 0;
		}
		UpdateTimeline();
		UpdateSpins();
		UpdateFrameInfo();
		UpdateFlow();
	}
	inModal = false;
}

void DanFrame::OnDeleteState(wxCommandEvent& event)
{
	if(StateListCtrl->GetFirstSelected() != wxNOT_FOUND)
	{
		if(playingMode) SetPlayingMode(false);
		std::string stateName = StateListCtrl->GetItemText(StateListCtrl->GetFirstSelected()).ToStdString();
		Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Delete state - %s", stateName));
		animator.DeleteState(StateListCtrl->GetItemText(StateListCtrl->GetFirstSelected()).ToStdString());
		StateListCtrl->DanDeleteItem(StateListCtrl->GetFirstSelected());

		saved = false;

		if(animator.m_validStates.empty()) SetStartupMode(true);
		else
			SetStatelessControls(false);
	}
}

void DanFrame::OnStateSelection(wxListEvent& event)
{
	if(StateListCtrl->GetFirstSelected() != wxNOT_FOUND)
	{
		animator.SetState(StateListCtrl->GetItemText(StateListCtrl->GetFirstSelected()).ToStdString());
		timelineSlider->SetValue(1);
		ResetFrame();
		playFrame = 0;
		if(inStartup)
			SetStartupMode(false);
		else
			SetStatelessControls(true);

		UpdateTimeline();
		UpdateSpins();
		UpdateFrameInfo();
		UpdateFlow();
	}
}

void DanFrame::OnStateDeselect(wxListEvent& event)
{
	if(StateListCtrl->GetFirstSelected() == -1)
	{
		UpdateTimeline();
		UpdateSpins();
		UpdateFrameInfo();
		UpdateFlow();

		if(inStartup)
			SetStartupMode(true);
		else
			SetStatelessControls(false);
	}
}

void DanFrame::OnAddSprite(wxCommandEvent& event)
{
	const wxChar *Filters = _T(
		"All Supported Images|*.png;*.bmp;*.tga;*.jpg;*.jpeg;*.gif"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Open Sprites"), wxEmptyString, wxEmptyString, Filters, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE | wxFD_CHANGE_DIR);
	inModal = true;
	if(fd->ShowModal() == wxID_OK)
	{
		bool eflag = false;
		wxArrayString fileFullPaths;
		wxArrayString fileNames;
		fd->GetPaths(fileFullPaths);
		fd->GetFilenames(fileNames);

		for(int i = 0; i < fileFullPaths.size(); i++)
		{
			std::string fn = fileNames[i].ToStdString();
			fn = StripStringExtension(fn);
			fn = RemoveWhiteSpace(fn);
			set_uppercase(fn);

			if(SpritesListCtrl->FindItem(-1, fn) != wxNOT_FOUND)
				continue;
			
			if(Locator::GetTextureManager()->Precache(fileFullPaths[i].ToStdString()))
			{
				if(!ValidateSpriteFormat(fn))
					eflag = true;

				wxString finalstr = fn;
				Locator::GetTextureManager()->Remap(fileFullPaths[i].ToStdString(), finalstr.ToStdString());
				int testloc = -1;
				if(testloc = SpritesListCtrl->FindItem(-1, finalstr) != -1)
					SpritesListCtrl->DeleteItem(testloc);
				int newloc = SpritesListCtrl->InsertItem(SpritesListCtrl->GetItemCount(), finalstr);
				SpritesListCtrl->SetItemData(newloc, CreateSortData(finalstr, ComputeStringHash(SpritesListCtrl->GetItemText(newloc).ToStdString())));
				SpritesListCtrl->SetItem(newloc, COL_SPRITES_SOURCE, fileFullPaths[i]);
				int width = Locator::GetTextureManager()->GetTexture(finalstr.ToStdString())->getSize().x;
				int height = Locator::GetTextureManager()->GetTexture(finalstr.ToStdString())->getSize().y;
				SpritesListCtrl->SetItem(newloc, COL_SPRITES_SIZE, std::to_string(int(4 * width * height)));
				SpritesListCtrl->SetItem(newloc, COL_SPRITES_DIMS, std::to_string(width) + "x" + std::to_string(height));
				
				finalstr = "";

				saved = false;
			}
		}
		SpritesListCtrl->SortItems(ListStringComparison, 0);
		if(eflag)
			DanAlert::AlertIfNoAbort(warn_badspritenames, "One or more sprites being imported contained incorrectly formatted sprite names.\nBe sure to fix this or the resulting code export will also be incorrect.\nDanimator will, however, still function with the improper names.");
	}
	inModal = false;
}

void DanFrame::OnDeleteSprite(wxCommandEvent& event)
{
	if(SpritesListCtrl->GetFirstSelected() != wxNOT_FOUND && SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).CmpNoCase(wxString("* No Sprite (TNT1A0)")) != 0 )
	{
		if(DanAlert::AlertIf(warn_deletesprite, "Warning: You are about to delete a sprite or custom texture, which cannot be undone.\nClick Ok to continue or Cancel to abort.")) return;
		Locator::GetTextureManager()->Unload(SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString());

		for(auto it = animator.m_validStates.begin(); it != animator.m_validStates.end(); ++it)
		{
			for(int i = 0; i < it->second.m_frames.size(); i++)
			{
				if(it->second.GetFrame(i).spriteName == SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString())
				{
					it->second.GetFrame(i).spriteName = "TNT1A0";
					it->second.GetFrame(i).sprite.setTexture(*Locator::GetTextureManager()->GetTexture("TNT1A0"));
				}
			}
		}

		SpritesListCtrl->DeleteItem(SpritesListCtrl->GetFirstSelected());

		saved = false;
	}
}

void DanFrame::OnAddSound(wxCommandEvent& event)
{
	wxString sndAlias = "";
	const wxChar *Filters = _T(
		"All Supported Sounds|*.ogg;*.wav;*.flac;*.aiff;*.au;*.raw;*.voc"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Open Sounds"), wxEmptyString, wxEmptyString, Filters, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE | wxFD_CHANGE_DIR);
	inModal = true;
	if(fd->ShowModal() == wxID_OK)
	{
		wxArrayString fileFullPaths;
		wxArrayString fileNames;
		fd->GetPaths(fileFullPaths);
		fd->GetFilenames(fileNames);

		for(int i = 0; i < fileFullPaths.size(); i++)
		{
			wxTextEntryDialog *txt = new wxTextEntryDialog(this, fileNames[i] + _T("\nEnter the SNDINFO alias as it would appear in the lump\n(Example: weapons/shotgun1)"), _T("SNDINFO Alias"), wxEmptyString);
			if(txt->ShowModal() == wxID_OK)
			{
				if(txt->GetValue().IsEmpty())
				{
					continue;
				}
				sndAlias = txt->GetValue();
			}
			else
				continue;

			if(SoundsListCtrl->FindItem(-1, sndAlias) != wxNOT_FOUND)
				continue;

			if(Locator::GetSoundManager()->Precache(fileFullPaths[i].ToStdString()))
			{
				Locator::GetSoundManager()->Remap(fileFullPaths[i].ToStdString(), sndAlias.ToStdString());
				int testloc = -1;
				if(testloc = SoundsListCtrl->FindItem(-1, sndAlias) != -1)
					SoundsListCtrl->DeleteItem(testloc);
				int newloc = SoundsListCtrl->InsertItem(SoundsListCtrl->GetItemCount(), sndAlias);
				SoundsListCtrl->SetItemData(newloc, CreateSortData(sndAlias, ComputeStringHash(SoundsListCtrl->GetItemText(newloc).ToStdString())));
				SoundsListCtrl->SetItem(newloc, COL_SOUNDS_SOURCE, fileFullPaths[i].ToStdString());
				int sndsz = Locator::GetSoundManager()->GetSound(sndAlias.ToStdString())->getSampleCount() * sizeof(sf::Int16);
				SoundsListCtrl->SetItem(newloc, COL_SOUNDS_SIZE, std::to_string(sndsz));
				
				saved = false;
			}
		}
		SoundsListCtrl->SortItems(ListStringComparison, 0);
	}
	inModal = false;
}

void DanFrame::OnDeleteSound(wxCommandEvent& event)
{
	if(SoundsListCtrl->GetFirstSelected() != wxNOT_FOUND && SoundsListCtrl->GetItemText(SoundsListCtrl->GetFirstSelected()) != wxString("* No Sound"))
	{
		if(DanAlert::AlertIf(warn_deletesound, "Warning: You are about to delete a sound, which cannot be undone.\nClick Ok to continue or Cancel to abort.")) return;
		Locator::GetSoundManager()->Unload(SoundsListCtrl->GetItemText(SoundsListCtrl->GetFirstSelected()).ToStdString());

		for(auto it = animator.m_validStates.begin(); it != animator.m_validStates.end(); ++it)
		{
			for(int i = 0; i < it->second.m_frames.size(); i++)
			{
				if(it->second.GetFrame(i).soundName == SoundsListCtrl->GetItemText(SoundsListCtrl->GetFirstSelected()).ToStdString())
				{
					it->second.GetFrame(i).soundName = "";
					it->second.GetFrame(i).hasSound = false;
				}
			}
		}

		SoundsListCtrl->DeleteItem(SoundsListCtrl->GetFirstSelected());

		saved = false;
	}
}

void DanFrame::OnAddFrame(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Add frame to '%s'", animator.GetCurrentState()->name));
	animator.GetCurrentState()->AddEmptyFrame();
	UpdateTimeline();
	timelineSlider->SetValue(timelineSlider->GetMax());
	ResetFrame();
	UpdateSpins();
	UpdateFrameInfo();
	UpdateFlow();
	saved = false;
	EnableTimelineIfValid();
}

void DanFrame::OnDeleteFrame(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr || animator.GetCurrentState()->m_frames.size() <= 1)
		return;
	Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Delete frame from '%s'", animator.GetCurrentState()->name));
	animator.GetCurrentState()->RemoveFrame(timelineSlider->GetValue() - 1);
	UpdateTimeline();
	ResetFrame();
	UpdateSpins();
	UpdateFrameInfo();
	UpdateFlow();
	saved = false;

	EnableTimelineIfValid();
}

// Add these to prevent crashes with context menus
void DanFrame::OnSpriteClicked(wxListEvent& event)
{
	spritePreviewer->SetImageFromTexture(SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString());
	spritePreviewer->Refresh();
}

void DanFrame::OnSoundClicked(wxListEvent& event)
{
}

void DanFrame::OnSpriteDeselect(wxListEvent& event)
{
	spritePreviewer->ClearImage();
	spritePreviewer->Refresh();
}

void DanFrame::OnSelectSprite(wxListEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Change sprite to '%s'", SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString()));
	if(!SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).CmpNoCase(wxString("* No Sprite (TNT1A0)")))
	{
		animator.GetCurrentState()->GetCurrentFrame().sprite.setTexture(*Locator::GetTextureManager()->GetTexture("TNT1A0"), true);
		animator.GetCurrentState()->GetCurrentFrame().spriteName = "TNT1A0";
	}
	else
	{
		animator.GetCurrentState()->GetCurrentFrame().sprite.setTexture(*Locator::GetTextureManager()->GetTexture(SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString()), true);
		animator.GetCurrentState()->GetCurrentFrame().spriteName = SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString();
	}
	currentSpriteCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().spriteName);
	saved = false;
}

void DanFrame::OnSelectSound(wxListEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Change sound to '%s'", SoundsListCtrl->GetItemText(SoundsListCtrl->GetFirstSelected()).ToStdString()));
	if(SoundsListCtrl->GetItemText(SoundsListCtrl->GetFirstSelected()) == wxString("* No Sound"))
	{
		animator.GetCurrentState()->GetCurrentFrame().soundName = "";
		animator.GetCurrentState()->GetCurrentFrame().hasSound = false;
	}
	else
	{
		animator.GetCurrentState()->GetCurrentFrame().soundName = SoundsListCtrl->GetItemText(SoundsListCtrl->GetFirstSelected()).ToStdString();
		animator.GetCurrentState()->GetCurrentFrame().hasSound = true;
	}

	currentSoundCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().soundName);
	saved = false;
}

void DanFrame::OnSliderChange(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	ResetFrame();
	UpdateSpins();
	UpdateFrameInfo();
	UpdateFlow();
}

void DanFrame::OnXSpinChange(wxSpinEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Changed X offset for %s, frame %d", animator.GetCurrentState()->name, animator.GetCurrentState()->GetOffset() + 1));
	animator.GetCurrentState()->GetCurrentFrame().sprite.setPosition(xSpin->GetValue(), animator.GetCurrentState()->GetCurrentFrame().sprite.getPosition().y);
	saved = false;
}

void DanFrame::OnYSpinChange(wxSpinEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Changed Y offset for %s, frame %d", animator.GetCurrentState()->name, animator.GetCurrentState()->GetOffset() + 1));
	animator.GetCurrentState()->GetCurrentFrame().sprite.setPosition(animator.GetCurrentState()->GetCurrentFrame().sprite.getPosition().x, ySpin->GetValue());
	saved = false;
}

void DanFrame::OnNextFrame(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	timelineSlider->SetValue(timelineSlider->GetValue() + 1);
	ResetFrame();
	UpdateSpins();
	UpdateFrameInfo();
	UpdateFlow();
}

void DanFrame::OnPrevFrame(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	timelineSlider->SetValue(timelineSlider->GetValue() - 1);
	ResetFrame();
	UpdateSpins();
	UpdateFrameInfo();
	UpdateFlow();
}

void DanFrame::OnPlay(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	if(!playingMode) SetPlayingMode(true);
	else SetPlayingMode(false);
}

void DanFrame::OnPlayFromStart(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	if(!playingMode) SetPlayingMode(true, true);
}

void DanFrame::OnDurationSpinChange(wxSpinEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	animator.GetCurrentState()->GetCurrentFrame().tics = durationSpin->GetValue();
	animator.GetCurrentState()->GetCurrentFrame().duration = TICS(durationSpin->GetValue());
	UpdateTimeline();
	saved = false;
}

void DanFrame::OnClose(wxCloseEvent& event)
{
	if(event.CanVeto() && !saved) //&& !inStartup)
	{
		wxString str = "your project";
		if(!projectName.empty()) str = projectName;
		wxMessageDialog *choice = new wxMessageDialog(this, _T("Would you like to save changes to " + str + "?"), _T("Save?"), wxICON_QUESTION | wxYES_NO | wxCANCEL);
		int ret = choice->ShowModal();
		if(ret == wxID_YES)
		{
			if(projectName.empty())
			{
				const wxChar *Filters = _T(
					"Danimator Projects \t(*.danproj)|*.danproj"
					);
				wxFileDialog *fd = new wxFileDialog(this, _T("Save Project As"), wxEmptyString, wxEmptyString, Filters, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
				if(fd->ShowModal() == wxID_OK)
				{
					SaveProject(fd->GetFilename());
					projectName = fd->GetFilename();
					saved = true;
				}
			}
			else
			{
				SaveProject(projectName);
				saved = true;
			}
		}
		else if(ret == wxID_CANCEL)
		{
			event.Veto();
			return;
		}
	}
	Audio::ResetSlots();
	event.Skip();  // you may also do:  event.Skip();
}

void DanFrame::OnFlowGoto(wxCommandEvent& event)
{
	if(endChoice->GetSelection() == END_GOTO)
	{
		wxArrayString ary;
		for(auto it = animator.m_validStates.begin(); it != animator.m_validStates.end(); ++it)
		{
			ary.Add(it->first);
		}
		wxSingleChoiceDialog *choicedlg = new wxSingleChoiceDialog(this, _T("Choose a State Label"), wxEmptyString, ary);
		int ret = choicedlg->ShowModal();
		if(ret == wxID_CANCEL || choicedlg->GetReturnCode() == 0)
		{
			endChoice->SetSelection(0);
			animator.GetCurrentState()->ending = END_NONE;
			animator.GetCurrentState()->gotoLabel = "";
			return;
		}
		else if(ret == wxID_OK)
		{
			animator.GetCurrentState()->ending = END_GOTO;
			animator.GetCurrentState()->gotoLabel = choicedlg->GetStringSelection().ToStdString();
			endChoice->SetString(END_GOTO, "Goto " + choicedlg->GetStringSelection());
		}
	}
	else
	{
		endChoice->SetString(END_GOTO, "Goto...");
		animator.GetCurrentState()->ending = endChoice->GetSelection();
	}
}

void DanFrame::OnViewCode(wxCommandEvent& event)
{
	if(codeView)
		codeView->Close();
	codeView = new DanCode(GetDecorateCode(StateListCtrl->GetItemText(StateListCtrl->GetFirstSelected()).ToStdString()), this, _T("View DECORATE Code"), wxDefaultPosition, wxSize(300, 450));
	errorFlag = false;
}

void DanFrame::OnExportCode(wxCommandEvent& event)
{
	const wxChar *Filters = _T(
		"Text Files \t(*.txt)|*.txt"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Export DECORATE Code"), wxEmptyString, wxEmptyString, Filters, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
	inModal = true;
	if(fd->ShowModal() == wxID_OK)
	{
		wxString code = GetAllDecorateCode();
		wxFFile file(fd->GetFilename(), "w");
		file.Write(code);
		if(errorFlag)
			DanAlert::AlertIfNoAbort(warn_badexport, "One or more states were exported while containing errors (usually in sprite names).\nBe sure to look over the code and correct any improperly formatted frames.");
		errorFlag = false;
	}
	inModal = false;
}

void DanFrame::OnApplySprite(wxCommandEvent &event)
{
	if(animator.GetCurrentState() == nullptr || SpritesListCtrl->GetFirstSelected() == -1)
		return;
	Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Change sprite to '%s'", SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString()));
	if(!SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).CmpNoCase(wxString("* No Sprite (TNT1A0)")))
	{
		animator.GetCurrentState()->GetCurrentFrame().sprite.setTexture(*Locator::GetTextureManager()->GetTexture("TNT1A0"), true);
		animator.GetCurrentState()->GetCurrentFrame().spriteName = "TNT1A0";
	}
	else
	{
		animator.GetCurrentState()->GetCurrentFrame().sprite.setTexture(*Locator::GetTextureManager()->GetTexture(SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString()), true);
		animator.GetCurrentState()->GetCurrentFrame().spriteName = SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString();
	}
	currentSpriteCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().spriteName);
	saved = false;
}

void DanFrame::OnCreateTexture(wxCommandEvent &event)
{
	wxTextEntryDialog *txt = new wxTextEntryDialog(this, _T("Enter a unique name for the new TEXTURES sprite.\nExample: GUNNA0"), _T("Create TEXTURES Entry"), wxEmptyString);
	inModal = true;
	txt->SetMaxLength(8);
	if(txt->ShowModal() == wxID_OK)
	{
		if(txt->GetValue().IsEmpty())
		{
			inModal = false;
			return;
		}
		std::string tname = RemoveWhiteSpace(txt->GetValue().ToStdString());
		set_uppercase(tname);
		wxString texName = tname;
		if(tname.length() < 6 || !isalpha(tname[4]) || !isdigit(tname[5]))
		{
			if(DanAlert::AlertIf(warn_badtexturename, "The texture name entered does not conform to the format needed for Decorate animations.\nImproper names will produce inaccurate code when exported.\nClick Ok to continue or Cancel to abort.")) return;
		}
		if(SpritesListCtrl->FindItem(-1, texName) != wxNOT_FOUND)
		{
			wxMessageBox("Failed to create the Textures entry -- Sprite name is already in use!",
						 "Create TEXTURES Entry", wxOK | wxICON_WARNING);
			inModal = false;
			return;
		}

		saved = false;
		int newloc = SpritesListCtrl->InsertItem(SpritesListCtrl->GetItemCount(), texName);
		// Ensure that TEXTURES entries are sorted near the bottom
		int hashNum = ComputeStringHash(SpritesListCtrl->GetItemText(newloc).ToStdString()) + 500000;
		SpritesListCtrl->SetItemData(newloc, CreateSortData(texName, hashNum));
		textualPanel->m_activeData = SpritesListCtrl->GetItemData(newloc);
		SpritesListCtrl->SortItems(ListStringComparison, 0);
		textualPanel->m_activeTexture = Locator::GetTextureManager()->GetCompositeTexture(texName.ToStdString());
		textualPanel->m_activeTexture->m_hash = hashNum;
		textualPanel->m_activeTexture->m_texname = texName.ToStdString();
		textualPanel->m_textureName = texName.ToStdString();
		SetEditorMode(EDITMODE_TEXTURES);
	}
	inModal = false;
}

void DanFrame::OnStateDuplicate(wxCommandEvent &event)
{
	// No safety check, hopefully HitTest() doesn't somehow fail me
	std::string name = StateListCtrl->GetItemText(hitIndex);

	inModal = true;
	wxTextEntryDialog *txt = new wxTextEntryDialog(this, _T("Enter the name for the duplicate:"), _T("Duplicate"));
	if(txt->ShowModal() == wxID_OK)
	{
		if(txt->GetValue().IsEmpty())
		{
			inModal = false;
			return;
		}
		std::string realString = RemoveWhiteSpace(txt->GetValue().ToStdString());
		if(StateListCtrl->FindItem(-1, wxString(realString)) != wxNOT_FOUND)
		{
			wxMessageBox("Failed to duplicate state -- Label already exists!",
						 "Duplicate", wxOK | wxICON_WARNING);
			inModal = false;
			return;
		}
		Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, "Duplicate state");
		StateListCtrl->DanPushBack(realString);
		animator.DuplicateState(name, realString);
	}
	inModal = false;
}

void DanFrame::OnStateRename(wxCommandEvent &event)
{
	// No safety check, hopefully HitTest() doesn't somehow fail me
	std::string name = StateListCtrl->GetItemText(hitIndex);

	inModal = true;
	wxTextEntryDialog *txt = new wxTextEntryDialog(this, _T("Rename state: ") + name, _T("Rename"), name);
	if(txt->ShowModal() == wxID_OK)
	{
		if(txt->GetValue().IsEmpty())
		{
			inModal = false;
			return;
		}
		std::string realString = RemoveWhiteSpace(txt->GetValue().ToStdString());
		if(StateListCtrl->FindItem(-1, wxString(realString)) != wxNOT_FOUND)
		{
			wxMessageBox("Failed to rename state -- Label already exists!",
						 "Rename", wxOK | wxICON_WARNING);
			inModal = false;
			return;
		}
		Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, "Rename state");
		StateListCtrl->SetItemText(hitIndex, realString);
		StateListCtrl->ChangeOwner(name, realString);
		animator.RenameState(name, realString);
	}
	inModal = false;
}

void DanFrame::OnStateContext(wxListEvent& event)
{
	event.Skip();
	wxPoint point = event.GetPoint();
	ShowStateContextMenu(point, event.GetIndex());
}

void DanFrame::OnSpriteContext(wxListEvent& event)
{
	event.Skip();
	wxPoint point = event.GetPoint();
	ShowSpriteContextMenu(point, event.GetIndex());
}

void DanFrame::OnSoundContext(wxListEvent& event)
{
	event.Skip();
	wxPoint point = event.GetPoint();
	ShowSoundContextMenu(point, event.GetIndex());
}

void DanFrame::OnExportTextures(wxCommandEvent& event)
{
	if(Locator::GetTextureManager()->compmap.empty())
		return;
	const wxChar *Filters = _T(
		"Text Files \t(*.txt)|*.txt"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Export TEXTURES Code"), wxEmptyString, wxEmptyString, Filters, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
	inModal = true;
	if(fd->ShowModal() == wxID_OK)
	{
		wxString code = "//\n// TEXTURES code generated by Danimator\n// Version: " DANVERSION "\n//\n// Modify as necessary.\n//\n\n";
		code += GetAllTexturesCode();
		wxFFile file(fd->GetFilename(), "w");
		file.Write(code);
	}
	inModal = false;
}

void DanFrame::OnApplySound(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr || SoundsListCtrl->GetFirstSelected() == -1)
		return;
	Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Change sound to '%s'", SoundsListCtrl->GetItemText(SoundsListCtrl->GetFirstSelected()).ToStdString()));
	if(SoundsListCtrl->GetItemText(SoundsListCtrl->GetFirstSelected()) == wxString("* No Sound"))
	{
		animator.GetCurrentState()->GetCurrentFrame().soundName = "";
		animator.GetCurrentState()->GetCurrentFrame().hasSound = false;
	}
	else
	{
		animator.GetCurrentState()->GetCurrentFrame().soundName = SoundsListCtrl->GetItemText(SoundsListCtrl->GetFirstSelected()).ToStdString();
		animator.GetCurrentState()->GetCurrentFrame().hasSound = true;
	}

	currentSoundCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().soundName);
	saved = false;
}

void DanFrame::OnSpriteEdit(wxCommandEvent &event)
{
	std::string name = SpritesListCtrl->GetItemText(hitIndex);
	textualPanel->m_activeTexture = Locator::GetTextureManager()->GetCompositeTexture(name);
	textualPanel->m_textureName = name;
	textualPanel->m_activeData = SpritesListCtrl->GetItemData(hitIndex);
	SetEditorMode(EDITMODE_TEXTURES);
}

void DanFrame::OnSpriteRename(wxCommandEvent &event)
{
	// No safety check, hopefully HitTest() doesn't somehow fail me
	std::string name = SpritesListCtrl->GetItemText(hitIndex);

	inModal = true;
	wxTextEntryDialog *txt = new wxTextEntryDialog(this, _T("Rename custom texture: ") + name, _T("Rename"), name);
	txt->SetMaxLength(8);
	if(txt->ShowModal() == wxID_OK)
	{
		if(txt->GetValue().IsEmpty())
		{
			inModal = false;
			return;
		}
		std::string realString = RemoveWhiteSpace(txt->GetValue().ToStdString());
		set_uppercase(realString);
		if(!ValidateSpriteFormat(realString))
			if(DanAlert::AlertIf(warn_badspritenames, "The sprite name you have entered for renaming does not conform to the required DECORATE naming format.\nDanimator can still use the sprite, but your exported animation code will not work without changes.\nClick Ok to continue or Cancel to abort."))
				return;
		if(SpritesListCtrl->FindItem(-1, wxString(realString)) != wxNOT_FOUND)
		{
			wxMessageBox("Failed to rename texture -- Name already exists!",
							"Rename", wxOK | wxICON_WARNING);
			inModal = false;
			return;
		}
		Locator::GetTextureManager()->Remap(name, realString);
		SpritesListCtrl->SetItemText(hitIndex, realString);
		int hashNum = ComputeStringHash(SpritesListCtrl->GetItemText(hitIndex).ToStdString()) + 500000;
		SpritesListCtrl->SetItemData(hitIndex, CreateSortData(SpritesListCtrl->GetItemText(hitIndex).ToStdString(), hashNum));

		Locator::GetTextureManager()->GetCompositeTexture(realString)->m_hash = hashNum;
		Locator::GetTextureManager()->GetCompositeTexture(realString)->m_texname = realString;

		SpritesListCtrl->SortItems(ListStringComparison, 0);

		animator.UpdateSpriteNames(name, realString);
		UpdateFrameInfo();
	}
	inModal = false;
}

void DanFrame::OnSpriteDuplicate(wxCommandEvent &event)
{
	// No safety check, hopefully HitTest() doesn't somehow fail me
	std::string name = SpritesListCtrl->GetItemText(hitIndex);

	inModal = true;
	wxTextEntryDialog *txt = new wxTextEntryDialog(this, _T("Duplicate custom texture: ") + name, _T("Duplicate"));
	if(txt->ShowModal() == wxID_OK)
	{
		if(txt->GetValue().IsEmpty())
		{
			inModal = false;
			return;
		}
		std::string realString = RemoveWhiteSpace(txt->GetValue().ToStdString());
		set_uppercase(realString);
		if(!ValidateSpriteFormat(realString))
			if(DanAlert::AlertIf(warn_badspritenames, "The sprite name you have entered for duplication does not conform to the required DECORATE naming format.\nDanimator can still use the sprite, but your exported animation code will not work without changes.\nClick Ok to continue or Cancel to abort."))
				return;
		if(SpritesListCtrl->FindItem(-1, wxString(realString)) != wxNOT_FOUND)
		{
			wxMessageBox("Failed to duplicate texture -- Name already exists!",
						 "Duplicate", wxOK | wxICON_WARNING);
			inModal = false;
			return;
		}
		Locator::GetTextureManager()->Duplicate(name, realString);

		int newloc = SpritesListCtrl->InsertItem(hitIndex, realString);
		int hashNum = ComputeStringHash(SpritesListCtrl->GetItemText(newloc).ToStdString()) + 500000;
		SpritesListCtrl->SetItemData(newloc, CreateSortData(SpritesListCtrl->GetItemText(newloc).ToStdString(), hashNum));
		SpritesListCtrl->SetItem(newloc, COL_SPRITES_SOURCE, _T("TEXTURES"));
		int width = Locator::GetTextureManager()->GetTexture(realString)->getSize().x;
		int height = Locator::GetTextureManager()->GetTexture(realString)->getSize().y;
		//SpritesListCtrl->SetItem(newloc, COL_SPRITES_SIZE, std::to_string(int(4 * width * height)));
		SpritesListCtrl->SetItem(newloc, COL_SPRITES_DIMS, std::to_string(width) + "x" + std::to_string(height));
		SpritesListCtrl->SortItems(ListStringComparison, 0);

		Locator::GetTextureManager()->GetCompositeTexture(realString)->m_hash = hashNum;
		Locator::GetTextureManager()->GetCompositeTexture(realString)->m_texname = realString;

		//animator.UpdateSpriteNames(name, realString);
	}
	inModal = false;
}

void DanFrame::OnSpriteViewCode(wxCommandEvent &event)
{
	if(SpritesListCtrl->GetFirstSelected() == -1 || !Locator::GetTextureManager()->IsComposite(SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString()))
		return;
	if(codeView)
		codeView->Close();
	codeView = new DanCode(Locator::GetTextureManager()->GetCompositeTexture(SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString())->GetTextureCode(), this, _T("View TEXTURES Code"), wxDefaultPosition, wxSize(300, 450));
}

void DanFrame::OnSoundRename(wxCommandEvent &event)
{
	// No safety check, hopefully HitTest() doesn't somehow fail me
	std::string name = SoundsListCtrl->GetItemText(hitIndex);

	inModal = true;
	wxTextEntryDialog *txt = new wxTextEntryDialog(this, _T("Rename sound: ") + name, _T("Rename"), name);
	if(txt->ShowModal() == wxID_OK)
	{
		if(txt->GetValue().IsEmpty())
		{
			inModal = false;
			return;
		}
		std::string realString = RemoveWhiteSpace(txt->GetValue().ToStdString());
		if(SoundsListCtrl->FindItem(-1, wxString(realString)) != wxNOT_FOUND)
		{
			wxMessageBox("Failed to rename sound -- Name already exists!",
						 "Rename", wxOK | wxICON_WARNING);
			inModal = false;
			return;
		}
		Locator::GetSoundManager()->Remap(name, realString);
		SoundsListCtrl->SetItemText(hitIndex, realString);

		animator.UpdateSoundNames(name, realString);
		UpdateFrameInfo();
	}
	inModal = false;
}

void DanFrame::OnInsertFrame(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;
	Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Inserted frame to '%s'", animator.GetCurrentState()->name));
	animator.GetCurrentState()->InsertEmptyFrame();
	UpdateTimeline();
	timelineSlider->SetValue(timelineSlider->GetValue() + 1);
	ResetFrame();
	UpdateSpins();
	UpdateFrameInfo();
	UpdateFlow();
	saved = false;
	EnableTimelineIfValid();
}

void DanFrame::OnBatchAction(wxCommandEvent& event)
{
	if(animator.GetCurrentState()->m_frames.size() < 2)
		return;
	BatchActionDialog *batchdlg = new BatchActionDialog(this);
	batchdlg->CenterOnParent();
	inModal = true;
	if(batchdlg->ShowModal() == wxID_OK)
	{
		if(batchdlg->frameStartEntry->GetValue() > batchdlg->frameEndEntry->GetValue())
		{
			DanAlert::Alert("Could not perform batch action\nFrame numbers entered were invalid");
			inModal = false;
			return;
		}

		Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Batch action on state '%s'", GetAnimator().GetCurrentState()->name));

		int startFrame = batchdlg->frameStartEntry->GetValue() - 1;
		int endFrame = batchdlg->frameEndEntry->GetValue();
		bool absolute = batchdlg->changeRadio->GetSelection();

		ClampTo(startFrame, 0, int(GetAnimator().GetCurrentState()->m_frames.size()) - 1);
		ClampTo(endFrame, 1, int(GetAnimator().GetCurrentState()->m_frames.size()));

		sf::Vector2f coords;
		int newtic = 0;
		for(int i = startFrame; i < endFrame; ++i)
		{
			switch(batchdlg->dataChoice->GetSelection())
			{
			case 0: // offsets
				if(batchdlg->xChangeLabel->GetValue())
					coords.x = batchdlg->xEntry->GetValue();
				else if(absolute)
					coords.x = animator.GetCurrentState()->m_frames[i].sprite.getPosition().x;
				else
					coords.x = 0.f;

				if(batchdlg->yChangeLabel->GetValue())
					coords.y = batchdlg->yEntry->GetValue();
				else if(absolute)
					coords.y = animator.GetCurrentState()->m_frames[i].sprite.getPosition().y;
				else
					coords.y = 0.f;

				if(absolute)
					animator.GetCurrentState()->m_frames[i].sprite.setPosition(coords);
				else
					animator.GetCurrentState()->m_frames[i].sprite.move(coords);
				break;
			case 1: // tics
				if(absolute)
				{
					newtic = batchdlg->ticsEntry->GetValue();
					if(newtic < 1)
						newtic = 1;
					animator.GetCurrentState()->m_frames[i].tics = newtic;
					animator.GetCurrentState()->m_frames[i].duration = TICS(animator.GetCurrentState()->m_frames[i].tics);
				}
				else
				{
					animator.GetCurrentState()->m_frames[i].tics += batchdlg->ticsEntry->GetValue();
					if(animator.GetCurrentState()->m_frames[i].tics < 1)
						animator.GetCurrentState()->m_frames[i].tics = 1;
					animator.GetCurrentState()->m_frames[i].duration = TICS(animator.GetCurrentState()->m_frames[i].tics);
				}
				break;
			}
		}

		UpdateTimeline();
		UpdateSpins();
		UpdateFrameInfo();
	}
	inModal = false;
}

void DanFrame::OnCopyFrame(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;

	if(animator.GetCurrentState()->m_frames.empty())
		return;

	ActionStatus("Frame copied");
	m_copiedFrame = animator.GetCurrentState()->GetCurrentFrame();
}

void DanFrame::OnPasteFrame(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;

	if(animator.GetCurrentState()->m_frames.empty())
		return;

	Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Pasted frame to state '%s', frame %d", GetAnimator().GetCurrentState()->name, timelineSlider->GetValue()));
	animator.GetCurrentState()->GetCurrentFrame() = m_copiedFrame;

	UpdateTimeline();
	UpdateSpins();
	UpdateFrameInfo();
}

void DanFrame::OnExportGIF(wxCommandEvent& event)
{
	if(animator.GetCurrentState() == nullptr)
		return;

	if(animator.GetCurrentState()->m_frames.empty())
		return;

	inModal = true;
	const wxChar *Filters = _T(
		"Animated GIF \t(*.gif)|*.gif"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Export GIF"), wxEmptyString, wxEmptyString, Filters, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
	if(fd->ShowModal() == wxID_OK)
	{
		WriteGIF(fd->GetFilename().ToStdString());
	}
	inModal = false;
}

void DanFrame::OnExportResources(wxCommandEvent& event)
{
	inModal = true;
	const wxChar *Filters = _T(
		"Danimator Resources \t(*.danres)|*.danres"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Export Resources"), wxEmptyString, wxEmptyString, Filters, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
	if(fd->ShowModal() == wxID_OK)
	{
		// I'll come back one day and give the user the option to choose what to export
		SaveProject(fd->GetFilename(), SAVEDATA_TEXTURES | SAVEDATA_COMPOSITES | SAVEDATA_SOUNDS, true);
	}
	inModal = false;
}

void DanFrame::OnImportResources(wxCommandEvent& event)
{
	inModal = true;
	const wxChar *Filters = _T(
		"Danimator Resources \t(*.danres)|*.danres"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Import Resources"), wxEmptyString, wxEmptyString, Filters, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
	if(fd->ShowModal() == wxID_OK)
	{
		LoadProject(fd->GetPath());
	}
	inModal = false;
}

void DanFrame::OnImportSndinfo(wxCommandEvent& event)
{
	inModal = true;
	const wxChar *Filters = _T(
		"SNDINFO Lump \t(*.*)|*.*"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Import SNDINFO"), wxEmptyString, wxEmptyString, Filters, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if(fd->ShowModal() == wxID_OK)
	{
		wxDirDialog *dirdlg = new wxDirDialog(this, _T("Sound files location"), wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
		if(dirdlg->ShowModal() == wxID_OK)
		{
			// Super duper dumb naive approach to parsing this
			// but whatever, "BETA 2" everyone!
			wxArrayString fileList;
			wxDir dir;
			dir.GetAllFiles(dirdlg->GetPath(), &fileList, wxEmptyString, wxDIR_DEFAULT);

			Parser parser;
			parser.OpenFile(fd->GetPath().ToStdString());
			parser.SetSpecialCharacters("$");
			while(parser.GetState() == PS_GOOD)
			{
				std::string token = parser.GetToken();
				while(parser.IsSpecialCharacter(token[0]) && parser.GetState() == PS_GOOD)
				{
					parser.SkipLineComment();
					token = parser.GetToken();
				}
				std::string sndAlias = token;

				//if(SoundsListCtrl->FindItem(-1, sndAlias) != wxNOT_FOUND)
				//	continue;


				token = parser.GetToken();
				if(!token.empty())
				{
					int fileIndex = -1;
					for(int i = 0; i < fileList.size(); ++i)
					{
						std::string fileAsLumpName = fileList[i].ToStdString();
						fileAsLumpName = StripStringDirectory(fileAsLumpName);
						fileAsLumpName = StripStringExtension(fileAsLumpName);

						if(CompNoCase(token, fileAsLumpName))
						{
							fileIndex = i;
							break;
						}
					}

					if(fileIndex > -1)
					{
						if(Locator::GetSoundManager()->Precache(fileList[fileIndex].ToStdString()))
						{
							Locator::GetSoundManager()->Remap(fileList[fileIndex].ToStdString(), sndAlias);
							int testloc = -1;
							if(testloc = SoundsListCtrl->FindItem(-1, sndAlias) != -1)
								SoundsListCtrl->DeleteItem(testloc);
							int newloc = SoundsListCtrl->InsertItem(SoundsListCtrl->GetItemCount(), sndAlias);
							SoundsListCtrl->SetItemData(newloc, CreateSortData(sndAlias, ComputeStringHash(SoundsListCtrl->GetItemText(newloc).ToStdString())));
							SoundsListCtrl->SetItem(newloc, COL_SOUNDS_SOURCE, fileList[fileIndex].ToStdString());
							int sndsz = Locator::GetSoundManager()->GetSound(sndAlias)->getSampleCount() * sizeof(sf::Int16);
							SoundsListCtrl->SetItem(newloc, COL_SOUNDS_SIZE, std::to_string(sndsz));

							saved = false;
						}
					}
				}
				token = "";
			}
			SoundsListCtrl->SortItems(ListStringComparison, 0);
		}
	}
	inModal = false;
}