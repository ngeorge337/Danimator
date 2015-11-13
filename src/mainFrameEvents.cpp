#include "libs.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "mainFrame.h"
#include "codewindow.h"
#include "preferences.h"
#include "actionmanager.h"

wxBEGIN_EVENT_TABLE(DanFrame, wxFrame)
EVT_CLOSE(DanFrame::OnClose)
EVT_MENU(wxID_EXIT, DanFrame::OnExit)
EVT_MENU(ID_SETTINGS, DanFrame::OnSettings)
EVT_MENU(ID_NEWPROJECT, DanFrame::OnNewProject)
EVT_MENU(ID_OPENPROJECT, DanFrame::OnOpenProject)
EVT_MENU(ID_SAVEPROJECT, DanFrame::OnSaveProject)
EVT_MENU(ID_SAVEPROJECTAS, DanFrame::OnSaveProjectAs)
EVT_MENU(wxID_ABOUT, DanFrame::OnAbout)
EVT_BUTTON(ID_NEWSTATE, DanFrame::OnNewState)
EVT_BUTTON(ID_DELSTATE, DanFrame::OnDeleteState)
EVT_BUTTON(ID_ADDSPRITE, DanFrame::OnAddSprite)
EVT_BUTTON(ID_DELSPRITE, DanFrame::OnDeleteSprite)
EVT_BUTTON(ID_ADDSOUND, DanFrame::OnAddSound)
EVT_BUTTON(ID_DELSOUND, DanFrame::OnDeleteSound)
EVT_BUTTON(ID_ADDFRAME, DanFrame::OnAddFrame)
EVT_BUTTON(ID_DELFRAME, DanFrame::OnDeleteFrame)
EVT_LIST_ITEM_SELECTED(ID_STATELIST, DanFrame::OnStateSelection)
EVT_BUTTON(ID_VIEWCODE, DanFrame::OnViewCode)
EVT_BUTTON(ID_EXPORTCODE, DanFrame::OnExportCode)
//EVT_LISTBOX_DCLICK(ID_STATELIST, DanFrame::OnStateSelection)
EVT_LIST_ITEM_ACTIVATED(ID_SELECTSPRITE, DanFrame::OnSelectSprite)
EVT_LIST_ITEM_ACTIVATED(ID_SELECTSOUND, DanFrame::OnSelectSound)
EVT_SLIDER(ID_TIMESLIDER, DanFrame::OnSliderChange)
EVT_SPINCTRL(ID_XSPIN, DanFrame::OnXSpinChange)
EVT_SPINCTRL(ID_YSPIN, DanFrame::OnYSpinChange)
EVT_SPINCTRL(ID_DURATION, DanFrame::OnDurationSpinChange)
EVT_BUTTON(ID_NEXTFRAME, DanFrame::OnNextFrame)
EVT_BUTTON(ID_PREVFRAME, DanFrame::OnPrevFrame)
EVT_BUTTON(ID_PLAY, DanFrame::OnPlay)
EVT_BUTTON(ID_PLAYFROMSTART, DanFrame::OnPlayFromStart)
EVT_CHOICE(ID_ENDING, DanFrame::OnFlowGoto)
//EVT_KEY_DOWN(DanFrame::OnHotkey)
EVT_TIMER(ID_STATUS_TIMER, DanFrame::OnTimer)
EVT_IDLE(DanFrame::OnIdle)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(DanStatusBar, wxStatusBar)
EVT_SIZE(DanStatusBar::OnSize)
EVT_SLIDER(ID_STATUS_ZOOM, DanStatusBar::OnZoomSliderChange)
wxEND_EVENT_TABLE()

int DanFrame::OnHotkey(wxKeyEvent& event)
{
	if(inModal)
		return -1;

	wxChar keyid = event.GetKeyCode();

	// Play/Stop -> Space
	if(keyid == WXK_SPACE)
	{
		OnPlay(wxCommandEvent());
		event.Skip();
		return true;
	}

	else if(!playingMode)
	{
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
	return -1;
}

void DanFrame::OnTimer(wxTimerEvent& event)
{
	DanStatus->SetStatusText("Ready");
}

void DanFrame::OnIdle(wxIdleEvent& event)
{
	wxString frm = "Frame: ";
	if(playingMode) frm += "Playing";
	else
		frm += timelineSlider->IsEnabled() ? std::to_string(timelineSlider->GetValue()) : "1";
	DanStatus->SetStatusText(frm, 1);	
}

void DanFrame::OnNewProject(wxCommandEvent& event)
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
	SpritesListCtrl->SetItemData(0, -9999);
	SoundsListCtrl->InsertItem(0, wxString("* No Sound"));
	SoundsListCtrl->SetItemData(0, -9999);

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
		SpritesListCtrl->SetItemData(0, -9999);
		SoundsListCtrl->InsertItem(0, wxString("* No Sound"));
		SoundsListCtrl->SetItemData(0, -9999);

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
}

void DanFrame::OnSaveProject(wxCommandEvent& event)
{
	if(inStartup) return;
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

void DanFrame::OnSaveProjectAs(wxCommandEvent& event)
{
	if(inStartup) return;
	const wxChar *Filters = _T(
		"Danimator Projects \t(*.danproj)|*.danproj"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Save Project As"), wxEmptyString, wxEmptyString, Filters, wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
	if(fd->ShowModal() == wxID_OK)
	{
		SaveProject(fd->GetFilename());
		projectName = fd->GetFilename();
		saved = true;
		this->SetTitle(wxString(StripStringExtension(projectName.ToStdString())) + " -- Danimator");
	}
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
	wxMessageBox("Danimator\nA friggin' ZDoom Animation Program\n\n© 2015 Nick George\n\nIcon art by Wartorn\n\nVersion " DANVERSION,
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

		if(animator.IsValidState(txt->GetValue().ToStdString()))
		{
			wxMessageBox("Failed to add new state -- Label already exists!",
						 "New State", wxOK | wxICON_WARNING);
			inModal = false;
			return;
		}

		bool selectIt = (animator.m_validStates.empty() || StateListCtrl->GetFirstSelected() == -1 ? true : false);

		std::string st = txt->GetValue().ToStdString();
		Locator::GetActionManager()->Insert(CMDTYPE_FRAMES, FormatString("Create state - %s", st));
		animator.CreateState(st)->AddEmptyFrame();
		StateListCtrl->DanPushBack(txt->GetValue());

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

void DanFrame::OnAddSprite(wxCommandEvent& event)
{
	const wxChar *Filters = _T(
		"All Supported Images|*.png;*.bmp;*.tga;*.jpg;*.jpeg;*.gif"
		);
	wxFileDialog *fd = new wxFileDialog(this, _T("Open Sprites"), wxEmptyString, wxEmptyString, Filters, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE | wxFD_CHANGE_DIR);
	inModal = true;
	if(fd->ShowModal() == wxID_OK)
	{
		wxArrayString fileFullPaths;
		wxArrayString fileNames;
		fd->GetPaths(fileFullPaths);
		fd->GetFilenames(fileNames);

		for(int i = 0; i < fileFullPaths.size(); i++)
		{
			if(SpritesListCtrl->FindItem(-1, fileNames[i]) != wxNOT_FOUND)
				continue;

			if(Locator::GetTextureManager()->Precache(fileFullPaths[i].ToStdString()))
			{
				std::string fn = fileNames[i].ToStdString();
				fn = StripStringExtension(fn);
				set_uppercase(fn);
				wxString finalstr = fn;
				Locator::GetTextureManager()->Remap(fileFullPaths[i].ToStdString(), finalstr.ToStdString());
				SpritesListCtrl->InsertItem(SpritesListCtrl->GetItemCount(), finalstr);
				SpritesListCtrl->SetItemData(SpritesListCtrl->GetItemCount() - 1, ComputeStringHash(SpritesListCtrl->GetItemText(SpritesListCtrl->GetItemCount() - 1).ToStdString()));
				SpritesListCtrl->SetItem(SpritesListCtrl->GetItemCount() - 1, COL_SPRITES_SOURCE, fileFullPaths[i]);
				int width = Locator::GetTextureManager()->GetTexture(finalstr.ToStdString())->getSize().x;
				int height = Locator::GetTextureManager()->GetTexture(finalstr.ToStdString())->getSize().y;
				SpritesListCtrl->SetItem(SpritesListCtrl->GetItemCount() - 1, COL_SPRITES_SIZE, std::to_string(int(4 * width * height)));
				SpritesListCtrl->SetItem(SpritesListCtrl->GetItemCount() - 1, COL_SPRITES_DIMS, std::to_string(width) + "x" + std::to_string(height));
				
				finalstr = "";

				saved = false;
			}
		}
		SpritesListCtrl->SortItems(ListStringComparison, 0);
	}
	inModal = false;
}

void DanFrame::OnDeleteSprite(wxCommandEvent& event)
{
	if(SpritesListCtrl->GetFirstSelected() != wxNOT_FOUND && SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).CmpNoCase(wxString("* No Sprite (TNT1A0)")) != 0 )
	{
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
				sndAlias = txt->GetValue();
			}
			else
				continue;

			if(SoundsListCtrl->FindItem(-1, sndAlias) != wxNOT_FOUND)
				continue;

			if(Locator::GetSoundManager()->Precache(fileFullPaths[i].ToStdString()))
			{
				Locator::GetSoundManager()->Remap(fileFullPaths[i].ToStdString(), sndAlias.ToStdString());
				SoundsListCtrl->InsertItem(SoundsListCtrl->GetItemCount(), sndAlias);
				SoundsListCtrl->SetItemData(SoundsListCtrl->GetItemCount() - 1, ComputeStringHash(SoundsListCtrl->GetItemText(SoundsListCtrl->GetItemCount() - 1).ToStdString()));
				SoundsListCtrl->SetItem(SoundsListCtrl->GetItemCount() - 1, COL_SOUNDS_SOURCE, fileFullPaths[i].ToStdString());
				int sndsz = Locator::GetSoundManager()->GetSound(sndAlias.ToStdString())->getSampleCount() * sizeof(sf::Int16);
				SoundsListCtrl->SetItem(SoundsListCtrl->GetItemCount() - 1, COL_SOUNDS_SIZE, std::to_string(sndsz));
				
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
	if(event.CanVeto() && !saved && !inStartup)
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
	codeView = new DanCode(this, _T("View Code"), wxDefaultPosition, wxSize(300, 200));
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
	}
	inModal = false;
}