#include "libs.h"
#include "alert.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "SFMLControl.h"
#include "textualView.h"
#include "textualPanel.h"
#include "mainFrame.h"
#include "codewindow.h"
#include "preferences.h"
#include "actionmanager.h"

extern DanFrame *theFrame;

DanFrame::DanFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size), playingMode(false), playFrame(0), saved(true), projectName(""), inStartup(true), inModal(false), errorFlag(false)
{
	wxBoxSizer *frameSizer = new wxBoxSizer(wxVERTICAL);

	statusTimer = new wxTimer(this, ID_STATUS_TIMER);
	//statusTimer->Start(5000);

	BuildMenuBar();
	BuildStatusBar();
	DanStatus->SetStatusText("Ready");

	// Add the panel so we can "mount" controls to it
	DanPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	DanSizer = new wxFlexGridSizer(0, 2, 0, 2);
	wxBoxSizer *tempSizer = new wxBoxSizer(wxVERTICAL);
	textualPanel = new TextualPanel(this);

	frameSizer->Add(DanPanel, 1, wxEXPAND);
	frameSizer->Add(textualPanel, 1, wxEXPAND);
	
	BuildStatesList();
	BuildResourceLists();
	BuildCanvasControls();

	// Finalize Sizers
	tempSizer->Add(ResourcesNotebook, 2, wxEXPAND);
	tempSizer->Add(stateSizer, 1, wxEXPAND);
	DanSizer->Add(tempSizer, 1, wxEXPAND);
	//DanSizer->Add(stateSizer, 1, wxEXPAND);
	//DanSizer->Add(ResourcesNotebook, 1, wxEXPAND);
	DanSizer->Add(renderSizer, 1, wxEXPAND);
	DanSizer->AddGrowableCol(1, 1);
	DanSizer->AddGrowableRow(0, 1);
	DanPanel->SetSizerAndFit(DanSizer);

	textualPanel->Hide();
	this->SetSizerAndFit(frameSizer);
	this->Layout();
		
	//DanPanel->Hide();
	//textualPanel->Show();

	SetStartupMode(true);

	CenterOnScreen();
}

void DanFrame::SetControls(bool controls)
{
	//StateListCtrl->Enable(controls);
	newStateButton->Enable(controls);
	delStateButton->Enable(controls);
	SpritesListCtrl->Enable(controls);
	applySpriteButton->Enable(controls);
	loadSpriteButton->Enable(controls);
	createTexturesButton->Enable(controls);
	delSpriteButton->Enable(controls);
	SoundsListCtrl->Enable(controls);
	loadSoundButton->Enable(controls);
	delSoundButton->Enable(controls);
	exportCodeButton->Enable(controls);
	xSpin->Enable(controls);
	ySpin->Enable(controls);
	currentSpriteCtrl->Enable(controls);
	currentSoundCtrl->Enable(controls);
	prevFrameButton->Enable(controls);
	nextFrameButton->Enable(controls);
	playFromStartButton->Enable(controls);
	addFrameButton->Enable(controls);
	delFrameButton->Enable(controls);
	durationSpin->Enable(controls);
	endChoice->Enable(controls);
	insertFrameButton->Enable(controls);
	batchButton->Enable(controls);
	menuFile->Enable(ID_NEWPROJECT, controls);
	menuFile->Enable(ID_OPENPROJECT, controls);
}

void DanFrame::SetStatelessControls(bool controls)
{
	//StateListCtrl->Enable(controls);
	//newStateButton->Enable(controls);
	delStateButton->Enable(controls);
	//SpritesListCtrl->Enable(controls);
	//loadSpriteButton->Enable(controls);
	//delSpriteButton->Enable(controls);
	//SoundsListCtrl->Enable(controls);
	//loadSoundButton->Enable(controls);
	//delSoundButton->Enable(controls);
	viewCodeButton->Enable(controls);
	exportCodeButton->Enable(controls);
	xSpin->Enable(controls);
	ySpin->Enable(controls);
	currentSpriteCtrl->Enable(controls);
	currentSoundCtrl->Enable(controls);
	prevFrameButton->Enable(controls);
	nextFrameButton->Enable(controls);
	playFromStartButton->Enable(controls);
	playButton->Enable(controls);
	addFrameButton->Enable(controls);
	delFrameButton->Enable(controls);
	durationSpin->Enable(controls);
	endChoice->Enable(controls);
	timelineSlider->Enable(controls);
	insertFrameButton->Enable(controls);
	batchButton->Enable(controls);
}

void DanFrame::SetStartupMode(bool set)
{
	bool controlMode = !set; // Going into startup mode = controls off (false)
	//StateListCtrl->Enable(set);
	//newStateButton->Enable(set);
	delStateButton->Enable(controlMode);
	//SpritesListCtrl->Enable(controlMode);
	//loadSpriteButton->Enable(controlMode);
	//delSpriteButton->Enable(controlMode);
	//SoundsListCtrl->Enable(controlMode);
	//loadSoundButton->Enable(controlMode);
	//delSoundButton->Enable(controlMode);
	xSpin->Enable(controlMode);
	ySpin->Enable(controlMode);
	if(controlMode)
		EnableTimelineIfValid();
	else
		timelineSlider->Disable();
	currentSpriteCtrl->Enable(controlMode);
	currentSoundCtrl->Enable(controlMode);
	prevFrameButton->Enable(controlMode);
	nextFrameButton->Enable(controlMode);
	playButton->Enable(controlMode);
	playFromStartButton->Enable(controlMode);
	loopCheckBox->Enable(controlMode);
	addFrameButton->Enable(controlMode);
	delFrameButton->Enable(controlMode);
	durationSpin->Enable(controlMode);
	endChoice->Enable(controlMode);
	viewCodeButton->Enable(controlMode);
	exportCodeButton->Enable(controlMode);
	insertFrameButton->Enable(controlMode);
	batchButton->Enable(controlMode);

	menuFile->Enable(ID_SAVEPROJECT, controlMode);
	menuFile->Enable(ID_SAVEPROJECTAS, controlMode);

	inStartup = set;
}

void DanFrame::SetPlayingMode(bool mode, bool fromStart /*= false*/)
{
	playingMode = mode;
	if(mode)
	{
		playFrame = animator.GetCurrentState()->frameOffset;
		SetControls(false);
		playButton->SetBitmap(wxBITMAP_PNG(StopAnim));
		playButton->SetToolTip(_T("Stop Animation"));
		if(fromStart) animator.GetCurrentState()->Reset(0);
		else animator.GetCurrentState()->Reset(animator.GetCurrentState()->frameOffset);
	}
	else if(!mode)
	{
		Audio::StopAllSounds();
		SetControls(true);
		playButton->SetBitmap(wxBITMAP_PNG(PlayAnim));
		playButton->SetToolTip(_T("Play Animation"));
		animator.GetCurrentState()->frameOffset = playFrame;
		timelineSlider->SetValue(animator.GetCurrentState()->frameOffset + 1);
		xSpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().sprite.getPosition().x);
		ySpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().sprite.getPosition().y);
		currentSpriteCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().spriteName);
		currentSoundCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().soundName);
		durationSpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().tics);
		endChoice->SetSelection(animator.GetCurrentState()->ending);
		if(endChoice->GetSelection() == END_GOTO)
			endChoice->SetString(END_GOTO, "Goto " + animator.GetCurrentState()->gotoLabel);
		else
			endChoice->SetString(END_GOTO, "Goto...");
	}
}

Animator & DanFrame::GetAnimator()
{
	return animator;
}

wxString DanFrame::GetAllDecorateCode()
{
	wxString ret = "";
	for(auto it = animator.m_validStates.begin(); it != animator.m_validStates.end(); ++it)
	{
		ret += GetDecorateCode(it->first);
		ret += "\n";
	}
	return ret;
}

wxString DanFrame::GetAllTexturesCode()
{
	wxString ret = "";	// composite textures already processed (for making sure dependencies were completed)
	processed.clear();
	for(auto it = Locator::GetTextureManager()->compmap.begin(); it != Locator::GetTextureManager()->compmap.end(); ++it)
	{
		if(it->second->HasAnyDependencies())	// Process immediately if the composite doesn't have any dependencies
		{
			ret += ProcessTextureDependency(it->first);
			//ret += "\n";
		}
		// should never have a circular dependency but this check still makes sense in some cases
		if(processed.find(it->first) == processed.end())
		{
			ret += it->second->GetTextureCode();
			ret += "\n";
		}
	}
	return ret;
}

wxString DanFrame::ProcessTextureDependency(const std::string &compTex)
{
	wxString ret = "";
	auto deps = Locator::GetTextureManager()->GetCompositeTexture(compTex)->GetDependencies();
	for(auto itx = deps.begin(); itx != deps.end(); ++itx)
	{
		// skip a dependency that was processed earlier
		if(processed.find(*itx) != processed.end())
			continue;
		if(Locator::GetTextureManager()->IsComposite(*itx))
		{
			if(Locator::GetTextureManager()->GetCompositeTexture(*itx)->HasAnyDependencies())
			{
				ret += ProcessTextureDependency(*itx);
			}
			if(processed.find(*itx) == processed.end())
			{
				processed.insert(*itx);
				ret += Locator::GetTextureManager()->GetCompositeTexture(*itx)->GetTextureCode();
				ret += "\n";
			}
		}
	}
	return ret;
}

wxString DanFrame::GetDecorateCode(const std::string &stateName)
{
	Parser parser;
	wxString output = "";
	std::string lastFrame = "";
	FState_t *state;
	//int index = 0;
	if(!animator.IsValidState(stateName))
		return wxString("");
	state = animator.GetState(stateName);
	if(state->m_frames.empty())
		return wxString("");

	// Add the statename
	output += stateName + ":\n";
	for(int i = 0; i < state->m_frames.size(); i++)
	{
		// Add the frames one at a time
		// State Frame Duration Offset(x, y) A_PlaySound(SoundName, CHAN_AUTO)
		// Flow [label]
		// SHTG A 4 Offset(92, 140) A_PlaySound("Gun/Pump", CHAN_AUTO)
		// Goto Ready
		output += "\t";
		parser.OpenString(animator.GetState(stateName)->GetFrame(i).spriteName);
		for(int x = 0; x < 4; ++x)
		{
			if(parser.GetState() != PS_GOOD)
			{
				errorFlag = true;
				break;
			}
			output += parser.GetChar();
		}
		output += " ";
		if(parser.GetState() == PS_GOOD)
		{
			output += parser.GetChar();
			output += " ";
		}
		else errorFlag = true;
		output += std::to_string(animator.GetState(stateName)->GetFrame(i).tics);
		if(animator.GetState(stateName)->GetFrame(i).sprite.getPosition().x != 0 || animator.GetState(stateName)->GetFrame(i).sprite.getPosition().y != 0)
			output += " Offset(" + std::to_string(int(animator.GetState(stateName)->GetFrame(i).sprite.getPosition().x)) + ", " + std::to_string(int(animator.GetState(stateName)->GetFrame(i).sprite.getPosition().y)) + ")";
		if(animator.GetState(stateName)->GetFrame(i).hasSound)
			output += " A_PlaySound(\"" + animator.GetState(stateName)->GetFrame(i).soundName + "\", CHAN_AUTO)";
		output += "\n";
	}
	if(animator.GetState(stateName)->ending != END_NONE)
	{
		output += "\t";
		switch(animator.GetState(stateName)->ending)
		{
		case END_LOOP: output += "Loop"; break;
		case END_WAIT: output += "Wait"; break;
		case END_STOP: output += "Stop"; break;
		case END_GOTO: output += "Goto " + animator.GetState(stateName)->gotoLabel; break;
		default:
			break;
		}
	}

	return output;
}

void DanFrame::EnableTimelineIfValid()
{
	if(animator.GetCurrentState() == nullptr)
	{
		timelineSlider->Disable();
		return;
	}

	if(animator.GetCurrentState()->m_frames.size() >= 2)
		timelineSlider->Enable();
	else
		timelineSlider->Disable();
}

void DanFrame::UpdateSpins()
{
	if(animator.GetCurrentState() == nullptr)
		return;
	xSpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().sprite.getPosition().x);
	ySpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().sprite.getPosition().y);
}


void DanFrame::UpdateFrameInfo()
{
	if(animator.GetCurrentState() == nullptr)
		return;
	currentSpriteCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().spriteName);
	currentSoundCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().soundName);
	durationSpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().tics);
}

void DanFrame::UpdateFlow()
{
	if(animator.GetCurrentState() == nullptr)
		return;
	endChoice->SetSelection(animator.GetCurrentState()->ending);

	if(endChoice->GetSelection() == END_GOTO)
		endChoice->SetString(END_GOTO, "Goto " + animator.GetCurrentState()->gotoLabel);
	else
		endChoice->SetString(END_GOTO, "Goto...");
}


void DanFrame::UpdateTimeline()
{
	StateListCtrl->OnUpdateInfo();
	if(animator.GetCurrentState() != nullptr)
	{
		/*
		if(StateListCtrl->GetFirstSelected() != -1)
		{
			int totalTime = 0;
			StateListCtrl->DanSetItem(StateListCtrl->GetFirstSelected(), COL_STATE_FRAMES, std::to_string(animator.GetCurrentState()->m_frames.size()));
			for(int i = 0; i < animator.GetCurrentState()->m_frames.size(); i++)
				totalTime += animator.GetCurrentState()->m_frames[i].tics;
			StateListCtrl->DanSetItem(StateListCtrl->GetFirstSelected(), COL_STATE_DURATION, std::to_string(totalTime));
		}
		*/
		if(animator.GetCurrentState()->m_frames.size() >= 2)
			timelineSlider->SetMax(animator.GetCurrentState()->m_frames.size());
		else
			timelineSlider->SetMax(2);
	}
	EnableTimelineIfValid();
}

void DanFrame::ResetFrame()
{
	if(animator.GetCurrentState() != nullptr)
		animator.GetCurrentState()->Reset(timelineSlider->GetValue() - 1);
}

void DanFrame::UpdateAll()
{
	if(animator.GetCurrentState() == nullptr)
		SetStatelessControls(false);
	UpdateTimeline();
	UpdateSpins();
	UpdateFrameInfo();
	UpdateFlow();
	if(animator.GetCurrentState() == nullptr)
		timelineSlider->SetValue(1);
	else
		timelineSlider->SetValue(GetAnimator().GetCurrentState()->frameOffset + 1);
}

void DanFrame::ActionStatus(wxString msg)
{
	DanStatus->SetStatusText(msg);
	isStatusSet = true;
	statusTimer->StartOnce(5000);
}

void DanFrame::RebuildStateList()
{
	StateListCtrl->DeleteAllItems();
	for(auto it = GetAnimator().m_validStates.begin(); it != GetAnimator().m_validStates.end(); ++it)
	{
		int totalTime = 0;
		int indx = StateListCtrl->InsertItem(StateListCtrl->GetItemCount(), it->second.name);
		StateListCtrl->DanSetItem(indx, COL_STATE_FRAMES, wxString(std::to_string(it->second.m_frames.size())));
		for(int i = 0; i < it->second.m_frames.size(); i++)
			totalTime += it->second.m_frames[i].tics;
		StateListCtrl->DanSetItem(indx, COL_STATE_DURATION, wxString(std::to_string(totalTime)));
	}
}

void DanFrame::SetEditorMode(int mode)
{
	if(editorMode == mode)
		return;
	if(playingMode)
		SetPlayingMode(false);
	switch(mode)
	{
	case EDITMODE_ANIMATION:
		textualPanel->m_activeTexture = nullptr;
		textualPanel->m_activeData = NULL;
		textualPanel->Hide();
		DanPanel->Show();
		DanPanel->Layout();
		spritePreviewer->needUpdate = true;
		spritePreviewer->Refresh();
		//glCanvas->SetFocus();
		break;
	case EDITMODE_TEXTURES:
		textualPanel->ResetControls();
		DanPanel->Hide();
		textualPanel->Show();
		textualPanel->Layout();
		//textualPanel->textualView->SetFocus();
		break;
	default:
		break;
	}
	editorMode = mode;
	this->Layout();
	//this->Fit();
}

void DanFrame::UpdateSpriteResource(wxUIntPtr sprName, int column, wxString newData)
{
	if(SpritesListCtrl->GetItemCount() == 0)
		return;

	//int index = SpritesListCtrl->FindItem(-1, sprName, true);
	//int index = SpritesListCtrl->FindItem(-1, textualPanel->GetActiveTexture()->m_hash);
	int index = SpritesListCtrl->FindItem(-1, sprName);
	if(index == -1)
		return;

	SpritesListCtrl->SetItem(index, column, newData);
}

void DanFrame::ShowStateContextMenu(const wxPoint& pos, int selection)
{
	if(selection == -1)
	{
		theFrame->hitIndex = 0;
		int flags = 0;
		theFrame->hitIndex = theFrame->StateListCtrl->HitTest(pos, flags);
		if(theFrame->hitIndex != -1 && flags & wxLIST_HITTEST_ONITEM)
		{
			selection = theFrame->hitIndex;
		}
	}
	else
		theFrame->hitIndex = selection;

	wxMenu context;
	context.Append(ID_CONTEXT_STATE_RENAME, _T("Rename..."));
	context.Append(ID_CONTEXT_STATE_DUPLICATE, _T("Duplicate..."));
	context.Append(ID_CONTEXT_STATE_VIEWCODE, _T("View Code"));
	context.Append(ID_CONTEXT_STATE_EXPORTGIF, _T("Export to GIF..."));

	PopupMenu(&context, pos);
}

void DanFrame::ShowSpriteContextMenu(const wxPoint& pos, int selection)
{
	if(selection == -1)
	{
		theFrame->hitIndex = 0;
		int flags = 0;
		theFrame->hitIndex = theFrame->SpritesListCtrl->HitTest(pos, flags);
		if(theFrame->hitIndex != -1 && flags & wxLIST_HITTEST_ONITEM)
		{
			selection = theFrame->hitIndex;
		}
	}
	else
		theFrame->hitIndex = selection;

	if(Locator::GetTextureManager()->IsComposite(theFrame->SpritesListCtrl->GetItemText(theFrame->hitIndex).ToStdString()))
	{
		wxMenu context;
		context.Append(ID_CONTEXT_SPRITE_EDIT, _T("Edit..."));
		context.Append(ID_CONTEXT_SPRITE_RENAME, _T("Rename..."));
		context.Append(ID_CONTEXT_SPRITE_DUPLICATE, _T("Duplicate..."));
		context.Append(ID_CONTEXT_SPRITE_VIEWCODE, _T("View Code"));

		PopupMenu(&context, pos);
	}
}

void DanFrame::ShowSoundContextMenu(const wxPoint& pos, int selection)
{
	if(selection == -1)
	{
		theFrame->hitIndex = 0;
		int flags = 0;
		theFrame->hitIndex = theFrame->SoundsListCtrl->HitTest(pos, flags);
		if(theFrame->hitIndex != -1 && flags & wxLIST_HITTEST_ONITEM)
		{
			selection = theFrame->hitIndex;
		}
	}
	else
		theFrame->hitIndex = selection;

	wxMenu context;
	context.Append(ID_CONTEXT_SOUND_RENAME, _T("Rename..."));

	PopupMenu(&context, pos);
}

void DanFrame::WriteGIF(const std::string &filename)
{
	FState_t *state = animator.GetCurrentState();
	Magick::Image image;
	sf::Image copyImage;
	sf::RenderTexture renderTexture;
	//const sf::Uint8 *pixelptr = nullptr;
	renderTexture.create(320, 200, true);

	std::vector<Magick::Image> gifOutput;

	for(int i = 0; i < state->m_frames.size(); ++i)
	{
		renderTexture.setActive();
		renderTexture.clear(sf::Color::Transparent);
		
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0);

		renderTexture.draw(state->m_frames[i].sprite);
		renderTexture.display();

		copyImage = renderTexture.getTexture().copyToImage();

		image = Magick::Image(int(renderTexture.getSize().x), int(renderTexture.getSize().y), std::string("RGBA"), Magick::CharPixel, copyImage.getPixelsPtr());
		//image.flip();
		image.gifDisposeMethod(3);
		image.matte(true);
		image.animationDelay(int(Round((state->m_frames[i].tics * 20.f) / 7.f)));

		gifOutput.push_back(image);

		glDisable(GL_ALPHA_TEST);
	}

	Magick::writeImages(gifOutput.begin(), gifOutput.end(), filename);
	ActionStatus("GIF Export Successful!");
}

void DanFrame::OnUndo(wxCommandEvent& event)
{
	if(editorMode == EDITMODE_ANIMATION)
	{
		if(!playingMode)
		{
			Locator::GetActionManager()->Undo();
		}
	}
	else if(editorMode == EDITMODE_TEXTURES)
	{
		textualPanel->GetActiveTexture()->GetActionHistory().Undo();
	}
}

void DanFrame::OnRedo(wxCommandEvent& event)
{
	if(editorMode == EDITMODE_ANIMATION)
	{
		if(!playingMode)
		{
			Locator::GetActionManager()->Redo();
		}
	}
	else if(editorMode == EDITMODE_TEXTURES)
	{
		textualPanel->GetActiveTexture()->GetActionHistory().Redo();
	}
}

DanStatusBar::DanStatusBar(wxWindow *parent, long style) : wxStatusBar(parent, wxID_ANY, style, "DanStatusBar")
{
	SetFieldsCount(3);
	int fieldsizes[3] = { -2, 135, 180 };
	SetStatusWidths(3, fieldsizes);

	zoomText = new wxStaticText(this, wxID_ANY, "Zoom: ", wxPoint(GetSize().GetWidth() - 198, 0), wxDefaultSize, 0);
	zoomValue = new wxStaticText(this, wxID_ANY, "   %",  wxPoint(GetSize().GetWidth() - 160, 0), wxDefaultSize, wxALIGN_RIGHT);
	zoomSlider = new wxSlider(this, ID_STATUS_ZOOM, 25, 2, 39, wxPoint(GetSize().GetWidth() - 130, 0), wxSize(120, 22), wxSL_HORIZONTAL);
}

void DanStatusBar::OnSize(wxSizeEvent& event)
{
	wxRect rect;
	if(!GetFieldRect(2, rect))
	{
		event.Skip();
		return;
	}

	zoomText->SetPosition(		wxPoint(GetSize().GetWidth() - 198, 4));
	zoomValue->SetPosition(		wxPoint(GetSize().GetWidth() - 160, 4));
	zoomSlider->SetPosition(	wxPoint(GetSize().GetWidth() - 130, 3));

	//event.Skip();
}

void DanStatusBar::SetZoomValue(int val)
{
	zoomValue->SetLabel(wxString(std::to_string((val)) + "%"));
	zoomSlider->SetValue(val / 5);
}

void DanStatusBar::OnZoomSliderChange(wxCommandEvent& event)
{
	wxSFMLCanvas::m_zoomlevel = 5 * zoomSlider->GetValue();
	wxSFMLCanvas::m_zoom = 2.f - (float(wxSFMLCanvas::m_zoomlevel) * 0.01f);
	zoomValue->SetLabel(wxString(std::to_string(wxSFMLCanvas::m_zoomlevel) + "%"));
}

int wxCALLBACK ListStringComparison(wxIntPtr item1, wxIntPtr item2, wxIntPtr WXUNUSED(sortData))
{
	if(item1 == NULL || item2 == NULL) return 0;
	SpriteSort_t *it1 = (SpriteSort_t *)item1;
	SpriteSort_t *it2 = (SpriteSort_t *)item2;

	if(it1->sprHash > 500000 && it2->sprHash < 500000)
		return 1;
	if(it1->sprHash < 500000 && it2->sprHash > 500000)
		return -1;

	return wxStrcmp(it1->sprString, it2->sprString);

	//return 0;
}

int ComputeStringHash(const std::string &str)
{
	int r = 0;
	for(int i = 0; i < str.length(); i++)
		r += int(str[i]);
	r += 150 * (str.length() - 1);
	return r;
	//std::hash<std::string> hashfn;
	//return int(hashfn(str));
}

bool ValidateSpriteFormat(const std::string &sprName)
{
	Parser parser;
	bool eflag = false;
	parser.OpenString(sprName);
	parser.SetSpecialCharacters("[];:'\"\\/{}()!@#$%^&*=_-+,.?<>`~");
	char ch = ' ';
	int z = 0;
	for(int x = 0; x < 6; ++x)
	{
		if(parser.GetState() != PS_GOOD)
		{
			eflag = true;
			break;
		}
		if(x < 5)
		{
			ch = parser.GetChar();
			if(parser.IsSpecialCharacter(ch))
				eflag = true;
		}
		else
		{
			z = parser.GetInteger();
			if(z < 0 || z > 9)
				eflag = true;
		}
	}
	return !eflag;
}

long CreateSortData(wxString _name, int hashNum)
{
	SpriteSort_t *sprsort = new SpriteSort_t();
	sprsort->sprHash = hashNum;
	sprsort->sprString = _name;
	return (long)sprsort;
}

bool SpriteDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
	bool eflag = false;
	bool goodFormat = false;
	wxArrayString formats;
	formats.Add(".png");
	formats.Add(".tga");
	formats.Add(".jpg");
	formats.Add(".jpeg");
	formats.Add(".gif");
	formats.Add(".bmp");

	for(int i = 0; i < filenames.size(); i++)
	{
		std::string ext = GetStringExtension(filenames[i].ToStdString());
		for(int x = 0; x < formats.size(); ++x)
		{
			if(ext == formats[x])
			{
				goodFormat = true;
				break;
			}
		}
		if(!goodFormat)
			continue;
		std::string fn = filenames[i].ToStdString();
		fn = StripStringExtension(fn);
		fn = StripStringDirectory(fn);
		fn = RemoveWhiteSpace(fn);
		set_uppercase(fn);

		if(theFrame->SpritesListCtrl->FindItem(-1, fn) != wxNOT_FOUND)
			continue;

		if(Locator::GetTextureManager()->Precache(filenames[i].ToStdString()))
		{
			if(!ValidateSpriteFormat(fn))
				eflag = true;

			wxString finalstr = fn;
			Locator::GetTextureManager()->Remap(filenames[i].ToStdString(), finalstr.ToStdString());
			int testloc = -1;
			testloc = theFrame->SpritesListCtrl->FindItem(-1, finalstr);
			if(testloc != -1)
				theFrame->SpritesListCtrl->DeleteItem(testloc);
			int newloc = theFrame->SpritesListCtrl->InsertItem(theFrame->SpritesListCtrl->GetItemCount(), finalstr);
			theFrame->SpritesListCtrl->SetItemData(newloc, CreateSortData(finalstr, ComputeStringHash(theFrame->SpritesListCtrl->GetItemText(newloc).ToStdString())));
			theFrame->SpritesListCtrl->SetItem(newloc, COL_SPRITES_SOURCE, filenames[i]);
			int width = Locator::GetTextureManager()->GetTexture(finalstr.ToStdString())->getSize().x;
			int height = Locator::GetTextureManager()->GetTexture(finalstr.ToStdString())->getSize().y;
			theFrame->SpritesListCtrl->SetItem(newloc, COL_SPRITES_SIZE, std::to_string(int(4 * width * height)));
			theFrame->SpritesListCtrl->SetItem(newloc, COL_SPRITES_DIMS, std::to_string(width) + "x" + std::to_string(height));

			finalstr = "";

			theFrame->saved = false;
		}
		else
			DanAlert::Alert(FormatString("The file '%s' could not be loaded, either because it is corrupt or\nthe file is an invalid data format.", StripStringDirectory(filenames[i].ToStdString())));
	}
	theFrame->SpritesListCtrl->SortItems(ListStringComparison, 0);
	if(eflag)
		DanAlert::AlertIfNoAbort(warn_badspritenames, "One or more sprites being imported contained incorrectly formatted sprite names.\nBe sure to fix this or the resulting code export will also be incorrect.\nDanimator will, however, still function with the improper names.");

	return true;
}
