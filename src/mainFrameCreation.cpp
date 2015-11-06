#include "libs.h"
#include "animator.h"
#include "mainFrame.h"
#include "codewindow.h"
#include "preferences.h"

void DanFrame::BuildMenuBar()
{
	// Create our menu bar items, and add their elements
	// FILE menu
	menuFile = new wxMenu;
	wxMenuItem *mNewProject = new wxMenuItem(menuFile, ID_NEWPROJECT, _T("New Project"), _T("Start a new project"));
	mNewProject->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));

	wxMenuItem *mOpenProject = new wxMenuItem(menuFile, ID_OPENPROJECT, _T("Open Project"), _T("Open an existing project"));
	mOpenProject->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN));

	wxMenuItem *mSave = new wxMenuItem(menuFile, ID_SAVEPROJECT, _T("Save Project"), _T("Save the current project"));
	mSave->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE));

	wxMenuItem *mSaveAs = new wxMenuItem(menuFile, ID_SAVEPROJECTAS, _T("Save Project As..."), _T("Save the current project to a new file"));
	mSaveAs->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS));

	menuFile->Append(mNewProject);
	menuFile->Append(mOpenProject);
	menuFile->Append(mSave);
	menuFile->Append(mSaveAs);

	//menuFile->Append(ID_NEWPROJECT, "New Project", "Start a new project");
	//menuFile->Append(ID_OPENPROJECT, "Open Project", "Open an existing project");
	//menuFile->Append(ID_SAVEPROJECT, "Save Project", "Save the current project");
	//menuFile->Append(ID_SAVEPROJECTAS, "Save Project As...", "Save the current project to a new file");
	//menuFile->AppendSeparator();
	//menuFile->Append(ID_CLOSEPROJECT, "Close Project", "Close the current project");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	// View menu
	menuView = new wxMenu;

	wxMenuItem *mSettings = new wxMenuItem(menuView, ID_SETTINGS, _T("Settings..."), _T("Change Settings"));
	mSettings->SetBitmap(wxArtProvider::GetBitmap(wxART_LIST_VIEW));

	menuView->Append(mSettings);
	//menuView->Append(ID_SETTINGS, "Settings...", "Change Settings");

	// HELP menu
	menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	// Now add the menubar and put the menus on it
	menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuView, "&View");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
}

void DanFrame::BuildStatesList()
{
	// States list
	stateSizer = new wxFlexGridSizer(2, 1, 0, 0);
	stateListSizer = new wxBoxSizer(wxVERTICAL);
	stateButtonSizer = new wxBoxSizer(wxHORIZONTAL);

	StateListCtrl = new wxListBox(DanPanel, ID_STATELIST, wxPoint(0, 0), wxSize(128, 224), NULL, NULL, wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_SORT);
	stateListSizer->Add(StateListCtrl, 1, wxALIGN_LEFT);

	newStateButton = new wxButton(DanPanel, ID_NEWSTATE, _T("New State"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	newStateButton->SetBitmap(wxBITMAP_PNG(State_Add));
	newStateButton->SetToolTip(_T("New State"));
	delStateButton = new wxButton(DanPanel, ID_DELSTATE, _T("Delete State"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	delStateButton->SetBitmap(wxBITMAP_PNG(State_Del));
	delStateButton->SetToolTip(_T("Delete State"));
	viewCodeButton = new wxButton(DanPanel, ID_VIEWCODE, _T("View DECORATE Code"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	viewCodeButton->SetBitmap(wxBITMAP_PNG(ViewCode));
	viewCodeButton->SetToolTip(_T("View DECORATE Code"));
	exportCodeButton = new wxButton(DanPanel, ID_EXPORTCODE, _T("Export to DECORATE"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	exportCodeButton->SetBitmap(wxBITMAP_PNG(ExportCode));
	exportCodeButton->SetToolTip(_T("Export to DECORATE"));
	stateButtonSizer->Add(newStateButton);
	stateButtonSizer->Add(delStateButton);
	stateButtonSizer->Add(viewCodeButton);
	stateButtonSizer->Add(exportCodeButton);

	stateSizer->SetFlexibleDirection(wxVERTICAL);
	stateSizer->Add(stateListSizer, 1, wxEXPAND | wxLEFT | wxTOP | wxBOTTOM, 4);
	stateSizer->Add(stateButtonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 4);
	stateSizer->AddGrowableRow(0, 1);
}

void DanFrame::BuildResourceLists()
{
	// Resources Notebook
	ResourcesNotebook = new wxNotebook(DanPanel, wxID_ANY, wxDefaultPosition, wxSize(200, 200), wxNB_BOTTOM);

	// SPRITES ===============================================
	spritePageSizer = new wxFlexGridSizer(2, 1, 0, 0);
	spriteListSizer = new wxBoxSizer(wxVERTICAL);
	spriteButtonSizer = new wxBoxSizer(wxHORIZONTAL);


	spritePanel = new wxPanel(ResourcesNotebook);

	SpritesListCtrl = new wxListBox(spritePanel, ID_SELECTSPRITE, wxPoint(0, 0), wxSize(200, 200), NULL, NULL, wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_SORT);
	SpritesListCtrl->Append(wxString("* No Sprite (TNT1A0)"));
	spriteListSizer->Add(SpritesListCtrl, 1, wxALIGN_LEFT);

	loadSpriteButton = new wxButton(spritePanel, ID_ADDSPRITE, _T("Load Sprites..."), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	loadSpriteButton->SetBitmap(wxBITMAP_PNG(Sprite_Add));
	loadSpriteButton->SetToolTip(_T("Load Sprites..."));
	delSpriteButton = new wxButton(spritePanel, ID_DELSPRITE, _T("Delete Sprite"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	delSpriteButton->SetBitmap(wxBITMAP_PNG(Sprite_Del));
	delSpriteButton->SetToolTip(_T("Delete Sprite"));
	spriteButtonSizer->Add(loadSpriteButton);
	spriteButtonSizer->Add(delSpriteButton);

	//spritePageSizer->SetFlexibleDirection(wxVERTICAL);
	spritePageSizer->Add(spriteListSizer, 1, wxEXPAND | wxALL, 4);
	spritePageSizer->Add(spriteButtonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxRIGHT, 4);
	spritePageSizer->AddGrowableRow(0, 1);

	spritePanel->SetSizer(spritePageSizer);
	// =======================================================

	// SOUNDS ===============================================
	soundPageSizer = new wxFlexGridSizer(2, 1, 0, 0);
	soundListSizer = new wxBoxSizer(wxVERTICAL);
	soundButtonSizer = new wxBoxSizer(wxHORIZONTAL);


	soundPanel = new wxPanel(ResourcesNotebook);

	SoundsListCtrl = new wxListBox(soundPanel, ID_SELECTSOUND, wxPoint(0, 0), wxSize(200, 200), NULL, NULL, wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_SORT);
	SoundsListCtrl->Append(wxString("* No Sound"));
	soundListSizer->Add(SoundsListCtrl, 1, wxALIGN_LEFT);

	loadSoundButton = new wxButton(soundPanel, ID_ADDSOUND, _T("Load Sounds..."), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	loadSoundButton->SetBitmap(wxBITMAP_PNG(Sound_Add));
	loadSoundButton->SetToolTip(_T("Load Sounds..."));
	delSoundButton = new wxButton(soundPanel, ID_DELSOUND, _T("Delete Sound"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	delSoundButton->SetBitmap(wxBITMAP_PNG(Sound_Del));
	delSoundButton->SetToolTip(_T("Delete Sound"));
	soundButtonSizer->Add(loadSoundButton);
	soundButtonSizer->Add(delSoundButton);

	//soundPageSizer->SetFlexibleDirection(wxVERTICAL);
	soundPageSizer->Add(soundListSizer, 1, wxEXPAND | wxALL, 4);
	soundPageSizer->Add(soundButtonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxRIGHT, 4);
	soundPageSizer->AddGrowableRow(0, 1);

	soundPanel->SetSizer(soundPageSizer);
	// =======================================================

	ResourcesNotebook->SetPageSize(wxSize(200, 200));
	ResourcesNotebook->AddPage(spritePanel, _T("Sprites"), true);
	ResourcesNotebook->AddPage(soundPanel, _T("Sounds"));
}

void DanFrame::BuildCanvasControls()
{
	renderSizer = new wxFlexGridSizer(2, 1, 8, 0);
	controlSizer = new wxFlexGridSizer(1, 3, 0, 4);

	// GL Canvas
	glCanvas = new wxSFMLCanvas(DanPanel, wxID_ANY, wxDefaultPosition, wxSize(320, 240), wxBORDER_SIMPLE);
	//glCanvas = new wxGLCanvas(DanPanel, wxID_ANY, NULL, wxDefaultPosition, wxSize(320, 200), wxBORDER_SIMPLE);
	//glCanvas->SetMinSize(wxSize(320, 200));
	//glContext = new wxGLContext(glCanvas);
	//glCanvas->SetCurrent(*glContext);

	// Controls ========================================
	// Offsets
	controlBox1 = new wxStaticBoxSizer(wxVERTICAL, DanPanel, _T("Offsets"));
	offsetsSizer = new wxBoxSizer(wxHORIZONTAL);
	currentSpriteSizer = new wxBoxSizer(wxHORIZONTAL);
	currentSoundSizer = new wxBoxSizer(wxHORIZONTAL);
	xSpin = new wxSpinCtrl(controlBox1->GetStaticBox(), ID_XSPIN, _T("0"), wxDefaultPosition, wxSize(80, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, -1024, 1024, 0);
	xText = new wxStaticText(controlBox1->GetStaticBox(), wxID_ANY, _T("X:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	ySpin = new wxSpinCtrl(controlBox1->GetStaticBox(), ID_YSPIN, _T("0"), wxDefaultPosition, wxSize(80, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, -1024, 1024, 0);
	yText = new wxStaticText(controlBox1->GetStaticBox(), wxID_ANY, _T("Y:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	currentSpriteText = new wxStaticText(controlBox1->GetStaticBox(), wxID_ANY, _T("Sprite:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	currentSoundText = new wxStaticText(controlBox1->GetStaticBox(), wxID_ANY, _T("Sound:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	currentSpriteCtrl = new wxTextCtrl(controlBox1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	currentSoundCtrl = new wxTextCtrl(controlBox1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);

	offsetsSizer->Add(xText, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	offsetsSizer->AddSpacer(4);
	offsetsSizer->Add(xSpin, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	offsetsSizer->AddSpacer(8);
	offsetsSizer->Add(yText, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	offsetsSizer->AddSpacer(4);
	offsetsSizer->Add(ySpin, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	offsetsSizer->AddSpacer(8);
	currentSpriteSizer->Add(currentSpriteText, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
	currentSpriteSizer->Add(currentSpriteCtrl, 1, wxALIGN_CENTER_VERTICAL);
	currentSoundSizer->Add(currentSoundText, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
	currentSoundSizer->Add(currentSoundCtrl, 1, wxALIGN_CENTER_VERTICAL);
	controlBox1->Add(offsetsSizer);
	controlBox1->AddSpacer(8);
	controlBox1->Add(currentSpriteSizer, 1, wxEXPAND | wxRIGHT, 8);
	//controlBox1->AddSpacer(4);
	controlBox1->Add(currentSoundSizer, 1, wxEXPAND | wxRIGHT, 8);
	// Timeline!!!
	controlBox2 = new wxStaticBoxSizer(wxHORIZONTAL, DanPanel, _T("Timeline"));
	timelineSizer = new wxFlexGridSizer(1, 2, 4, 0);
	timeControlSizer = new wxFlexGridSizer(3, 1, 4, 0);
	timeOptionsSizer = new wxFlexGridSizer(2, 1, 4, 0);
	timelineButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	framesButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	durationSizer = new wxBoxSizer(wxHORIZONTAL);
	timelineSlider = new wxSlider(controlBox2->GetStaticBox(), ID_TIMESLIDER, 1, 1, 2, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_TICKS | wxSL_MIN_MAX_LABELS);
	durationText = new wxStaticText(controlBox2->GetStaticBox(), wxID_ANY, _T("Tics:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	durationSpin = new wxSpinCtrl(controlBox2->GetStaticBox(), ID_DURATION, _T("1"), wxDefaultPosition, wxSize(80, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 65535, 1);
	loopCheckBox = new wxCheckBox(controlBox2->GetStaticBox(), wxID_ANY, _T("Loop"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	wxArrayString endings;
	endings.Add("None");
	endings.Add("Loop");
	endings.Add("Wait");
	endings.Add("Stop");
	endings.Add("Goto...");
	endText = new wxStaticText(controlBox2->GetStaticBox(), wxID_ANY, _T("Flow:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	endChoice = new wxChoice(controlBox2->GetStaticBox(), ID_ENDING, wxDefaultPosition, wxSize(100, 24), endings, 0);
	endChoice->SetSelection(0);
	CreateTimelineButtons();
	timelineButtonSizer->AddSpacer(4);
	timelineButtonSizer->Add(loopCheckBox, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 8);
	durationSizer->Add(durationText, 0, wxTOP, 4);
	durationSizer->AddSpacer(4);
	durationSizer->Add(durationSpin);
	durationSizer->AddSpacer(8);
	durationSizer->Add(endText, 0, wxTOP, 4);
	durationSizer->AddSpacer(4);
	durationSizer->Add(endChoice);
	timeControlSizer->Add(timelineSlider, 1, wxEXPAND | wxBOTTOM, 4);
	timeControlSizer->Add(durationSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 4);
	timeControlSizer->Add(timelineButtonSizer, 1, wxALIGN_CENTER_HORIZONTAL);
	timeControlSizer->AddGrowableCol(0, 1);
	timeOptionsSizer->Add(framesButtonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 24);
	timelineSizer->Add(timeControlSizer, 1, wxEXPAND | wxRIGHT, 8);
	timelineSizer->Add(timeOptionsSizer, 1, wxALIGN_RIGHT | wxLEFT, 8);
	timelineSizer->AddGrowableCol(0, 1);
	controlBox2->Add(timelineSizer, 1, wxEXPAND);
	// Options (ghosting, etc.)
	controlBox3 = new wxStaticBoxSizer(wxVERTICAL, DanPanel, _T("Options"));
	crosshairCheckBox = new wxCheckBox(controlBox3->GetStaticBox(), wxID_ANY, _T("Crosshair"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	allowSoundCheckBox = new wxCheckBox(controlBox3->GetStaticBox(), wxID_ANY, _T("Play Sounds"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	allowSoundCheckBox->SetValue(true);
	allowGhostCheckBox = new wxCheckBox(controlBox3->GetStaticBox(), wxID_ANY, _T("Ghosting"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	ghostFramesText = new wxStaticText(controlBox3->GetStaticBox(), wxID_ANY, _T("Ghost Frames"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	ghostFramesSpin = new wxSpinCtrl(controlBox3->GetStaticBox(), wxID_ANY, _T("1"), wxDefaultPosition, wxSize(60, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 10, 1);
	ghostSizer = new wxBoxSizer(wxHORIZONTAL);
	ghostSizer->Add(ghostFramesSpin, 0, wxLEFT | wxRIGHT, 4);
	ghostSizer->AddSpacer(2);
	ghostSizer->Add(ghostFramesText, 0, wxTOP | wxRIGHT, 4);
	controlBox3->Add(crosshairCheckBox, 0, wxLEFT | wxRIGHT | wxTOP, 4);
	controlBox3->AddSpacer(2);
	controlBox3->Add(allowSoundCheckBox, 0, wxLEFT | wxRIGHT, 4);
	controlBox3->AddSpacer(2);
	controlBox3->Add(allowGhostCheckBox, 0, wxLEFT | wxRIGHT, 4);
	controlBox3->AddSpacer(2);
	controlBox3->Add(ghostSizer);

	controlSizer->Add(controlBox1, 0, wxEXPAND);
	controlSizer->Add(controlBox2, 1, wxEXPAND);
	controlSizer->Add(controlBox3, 0, wxEXPAND);
	controlSizer->AddGrowableCol(1, 1);

	renderSizer->Add(glCanvas, 1, wxEXPAND | wxFIXED_MINSIZE | wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 4);
	renderSizer->Add(controlSizer, 1, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT, 4);
	renderSizer->AddGrowableCol(0, 1);
	renderSizer->AddGrowableRow(0, 1);
}

void DanFrame::CreateTimelineButtons()
{
	playFromStartButton = new wxButton(controlBox2->GetStaticBox(), ID_PLAYFROMSTART, _T("Play Animation From Start"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	playFromStartButton->SetBitmap(wxBITMAP_PNG(PlayAnimStart));
	playFromStartButton->SetToolTip(_T("Play Animation From Start"));
	timelineButtonSizer->Add(playFromStartButton, 0, wxALIGN_CENTER_HORIZONTAL);

	prevFrameButton = new wxButton(controlBox2->GetStaticBox(), ID_PREVFRAME, _T("Previous Frame"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	prevFrameButton->SetBitmap(wxBITMAP_PNG(FrameBack));
	prevFrameButton->SetToolTip(_T("Previous Frame"));
	timelineButtonSizer->Add(prevFrameButton, 0, wxALIGN_CENTER_HORIZONTAL);

	playButton = new wxButton(controlBox2->GetStaticBox(), ID_PLAY, _T("Play Animation"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	playButton->SetBitmap(wxBITMAP_PNG(PlayAnim));
	playButton->SetToolTip(_T("Play Animation"));
	timelineButtonSizer->Add(playButton, 0, wxALIGN_CENTER_HORIZONTAL);

	nextFrameButton = new wxButton(controlBox2->GetStaticBox(), ID_NEXTFRAME, _T("Next Frame"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	nextFrameButton->SetBitmap(wxBITMAP_PNG(FrameForward));
	nextFrameButton->SetToolTip(_T("Next Frame"));
	timelineButtonSizer->Add(nextFrameButton, 0, wxALIGN_CENTER_HORIZONTAL);

	addFrameButton = new wxButton(controlBox2->GetStaticBox(), ID_ADDFRAME, _T("Add Frame (uses previous frame's offsets)"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	addFrameButton->SetBitmap(wxArtProvider::GetBitmap(wxART_PLUS));
	addFrameButton->SetToolTip(_T("Add Frame"));
	framesButtonSizer->Add(addFrameButton, 0, wxALIGN_CENTER_HORIZONTAL);

	delFrameButton = new wxButton(controlBox2->GetStaticBox(), ID_DELFRAME, _T("Delete Current Frame"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	delFrameButton->SetBitmap(wxArtProvider::GetBitmap(wxART_MINUS));
	delFrameButton->SetToolTip(_T("Delete Frame"));
	framesButtonSizer->Add(delFrameButton, 0, wxALIGN_CENTER_HORIZONTAL);
}

void DanFrame::BuildStatusBar()
{
	// Create the status bar
	DanStatus = CreateStatusBar(3);
	int fieldsizes[3] = { -2, -1, 120 };
	DanStatus->SetStatusWidths(3, fieldsizes);
	//SetStatusText("Welcome...to Danimator!");
	SetStatusText("");
}