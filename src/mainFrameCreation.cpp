#include "libs.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "textualPanel.h"
#include "mainFrame.h"
#include "codewindow.h"
#include "preferences.h"

extern DanFrame *theFrame;

void DanFrame::BuildMenuBar()
{
	// Create our menu bar items, and add their elements
	// FILE menu
	menuFile = new wxMenu;
	wxMenuItem *mNewProject = new wxMenuItem(menuFile, ID_NEWPROJECT, _T("New Project\tCtrl-N"), _T("Start a new project"));
	mNewProject->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(16, 16)));

	wxMenuItem *mOpenProject = new wxMenuItem(menuFile, ID_OPENPROJECT, _T("Open Project\tCtrl-O"), _T("Open an existing project"));
	mOpenProject->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize(16, 16)));

	wxMenuItem *mSave = new wxMenuItem(menuFile, ID_SAVEPROJECT, _T("Save Project\tCtrl-S"), _T("Save the current project"));
	mSave->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_OTHER, wxSize(16, 16)));

	wxMenuItem *mSaveAs = new wxMenuItem(menuFile, ID_SAVEPROJECTAS, _T("Save Project As..."), _T("Save the current project to a new file"));
	mSaveAs->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_OTHER, wxSize(16, 16)));

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

	// Edit menu
	menuEdit = new wxMenu;

	wxMenuItem *mUndo = new wxMenuItem(menuFile, ID_UNDO, _T("Undo\tCtrl-Z"), _T("Undo last editing action"));
	mUndo->SetBitmap(wxArtProvider::GetBitmap(wxART_UNDO, wxART_OTHER, wxSize(16, 16)));

	wxMenuItem *mRedo = new wxMenuItem(menuFile, ID_REDO, _T("Redo\tCtrl-Y"), _T("Redo last editing action"));
	mRedo->SetBitmap(wxArtProvider::GetBitmap(wxART_REDO, wxART_OTHER, wxSize(16, 16)));

	menuEdit->Append(mUndo);
	menuEdit->Append(mRedo);

	// Project menu (import | export)
	menuImport = new wxMenu;
	menuExport = new wxMenu;

	wxMenuItem *mImportSndinfo = new wxMenuItem(menuFile, ID_IMPORTSNDINFO, _T("SNDINFO..."), _T("Import SNDINFO and load sound resources"));
	wxMenuItem *mImportResources = new wxMenuItem(menuFile, ID_IMPORTRESOURCES, _T("Resources..."), _T("Import project resources from a .danres file"));
	wxMenuItem *mExportResources = new wxMenuItem(menuFile, ID_EXPORTRESOURCES, _T("Resources..."), _T("Export a .danres Resource compilation"));

	menuImport->Append(mImportResources);
	menuImport->Append(mImportSndinfo);

	menuExport->Append(mExportResources);

	wxMenu *menuProject = new wxMenu;

	menuProject->AppendSubMenu(menuImport, _T("Import"));
	menuProject->AppendSubMenu(menuExport, _T("Export"));

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
	menuBar->SetWindowStyle(wxNO_BORDER);
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuEdit, "&Edit");
	menuBar->Append(menuView, "&View");
	menuBar->Append(menuProject, "&Project");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
}

void DanFrame::BuildStatesList()
{
	// States list
	stateSizer = new wxFlexGridSizer(2, 1, 0, 0);
	stateListSizer = new wxBoxSizer(wxVERTICAL);
	stateButtonSizer = new wxBoxSizer(wxHORIZONTAL);

	StateListCtrl = new DanStateList(DanPanel, ID_STATELIST, wxPoint(0, 0), wxSize(200, 120), wxLC_SINGLE_SEL | wxLC_REPORT | wxLC_HRULES);
	StateListCtrl->Connect(ID_STATELIST, wxEVT_LIST_ITEM_RIGHT_CLICK, wxListEventHandler(DanFrame::OnStateContext));
	//StateListCtrl->DeleteAllItems();
	StateListCtrl->AppendColumn(_T("State"), wxLIST_FORMAT_LEFT, -1);
	StateListCtrl->AppendColumn(_T("Frames"), wxLIST_FORMAT_LEFT, -1);
	StateListCtrl->AppendColumn(_T("Tics"), wxLIST_FORMAT_LEFT, -1);
	stateListSizer->Add(StateListCtrl, 1, wxALIGN_LEFT);

	newStateButton = new wxButton(DanPanel, ID_NEWSTATE, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	newStateButton->SetBitmap(wxBITMAP_PNG(State_Add));
	newStateButton->SetToolTip(_T("New State"));
	delStateButton = new wxButton(DanPanel, ID_DELSTATE, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	delStateButton->SetBitmap(wxBITMAP_PNG(State_Del));
	delStateButton->SetToolTip(_T("Delete State"));
	viewCodeButton = new wxButton(DanPanel, ID_VIEWCODE, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	viewCodeButton->SetBitmap(wxBITMAP_PNG(ViewCode));
	viewCodeButton->SetToolTip(_T("View DECORATE Code"));
	exportCodeButton = new wxButton(DanPanel, ID_EXPORTCODE, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	exportCodeButton->SetBitmap(wxBITMAP_PNG(ExportCode));
	exportCodeButton->SetToolTip(_T("Export to DECORATE"));
	stateButtonSizer->Add(newStateButton);
	stateButtonSizer->Add(delStateButton);
	stateButtonSizer->Add(viewCodeButton);
	stateButtonSizer->Add(exportCodeButton);

	stateSizer->SetFlexibleDirection(wxBOTH);
	stateSizer->Add(stateListSizer, 1, wxEXPAND | wxLEFT | wxTOP | wxBOTTOM, 4);
	stateSizer->Add(stateButtonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 4);
	stateSizer->AddGrowableRow(0, 1);
}

void DanFrame::BuildResourceLists()
{
	// Resources Notebook
	ResourcesNotebook = new wxNotebook(DanPanel, wxID_ANY, wxDefaultPosition, wxSize(200, 240), wxNB_BOTTOM);


	// SPRITES ===============================================
	spritePageSizer = new wxFlexGridSizer(2, 1, 0, 0);
	spriteListSizer = new wxBoxSizer(wxVERTICAL);
	spriteButtonSizer = new wxBoxSizer(wxHORIZONTAL);


	spritePanel = new wxPanel(ResourcesNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	
	spritePreviewer = new wxImagePanel(spritePanel, wxDefaultPosition, wxSize(200, 100));

	SpritesListCtrl = new wxListView(spritePanel, ID_SELECTSPRITE, wxPoint(0, 0), wxSize(200, 200), wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING | wxLC_REPORT | wxLC_VRULES);
	SpritesListCtrl->SetDropTarget(new SpriteDropTarget);
	SpritesListCtrl->AppendColumn(_T("Sprite"), wxLIST_FORMAT_LEFT, -1);
	SpritesListCtrl->AppendColumn(_T("Source"), wxLIST_FORMAT_LEFT, -1);
	SpritesListCtrl->AppendColumn(_T("Size (bytes)"), wxLIST_FORMAT_LEFT, -1);
	SpritesListCtrl->AppendColumn(_T("Dims"), wxLIST_FORMAT_LEFT, -1);
	SpritesListCtrl->InsertItem(0, wxString("* No Sprite (TNT1A0)"));
	SpritesListCtrl->SetItemData(0, CreateSortData("\0", -9999));
	SpritesListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
	spriteListSizer->Add(spritePreviewer, 0, wxALIGN_LEFT);
	spriteListSizer->AddSpacer(2);
	spriteListSizer->Add(SpritesListCtrl, 1, wxALIGN_LEFT);

	applySpriteButton = new wxButton(spritePanel, ID_APPLYSPRITEBTN, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	applySpriteButton->SetBitmap(wxBITMAP_PNG(APPLYSPRITE));
	applySpriteButton->SetToolTip(_T("Apply selected sprite to Frame"));
	loadSpriteButton = new wxButton(spritePanel, ID_ADDSPRITE, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	loadSpriteButton->SetBitmap(wxBITMAP_PNG(Sprite_Add));
	loadSpriteButton->SetToolTip(_T("Load Sprites..."));
	createTexturesButton = new wxButton(spritePanel, ID_CREATETEXTURE, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	createTexturesButton->SetBitmap(wxBITMAP_PNG(TEX_ADD));
	createTexturesButton->SetToolTip(_T("Create a new TEXTURES image"));
	delSpriteButton = new wxButton(spritePanel, ID_DELSPRITE, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	delSpriteButton->SetBitmap(wxBITMAP_PNG(Sprite_Del));
	delSpriteButton->SetToolTip(_T("Delete Sprite"));
	exportTexturesButton = new wxButton(spritePanel, ID_EXPORTTEXTURES, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	exportTexturesButton->SetBitmap(wxBITMAP_PNG(EXPORTTEX));
	exportTexturesButton->SetToolTip(_T("Export all TEXTURES code"));
	spriteButtonSizer->Add(applySpriteButton);
	spriteButtonSizer->Add(loadSpriteButton);
	spriteButtonSizer->Add(createTexturesButton);
	spriteButtonSizer->Add(delSpriteButton);
	spriteButtonSizer->Add(exportTexturesButton);

	//spritePageSizer->SetFlexibleDirection(wxVERTICAL);
	spritePageSizer->Add(spriteListSizer, 1, wxEXPAND | wxALL, 4);
	spritePageSizer->Add(spriteButtonSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxRIGHT, 4);
	spritePageSizer->AddGrowableRow(0, 1);

	spritePanel->SetSizerAndFit(spritePageSizer);
	spritePreviewer->Show(dan_showSpritePreview);
	spritePanel->Layout();
	// =======================================================

	// SOUNDS ===============================================
	soundPageSizer = new wxFlexGridSizer(2, 1, 0, 0);
	soundListSizer = new wxBoxSizer(wxVERTICAL);
	soundButtonSizer = new wxBoxSizer(wxHORIZONTAL);


	soundPanel = new wxPanel(ResourcesNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

	SoundsListCtrl = new wxListView(soundPanel, ID_SELECTSOUND, wxPoint(0, 0), wxSize(200, 200), wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING | wxLC_REPORT | wxLC_VRULES);
	SoundsListCtrl->AppendColumn(_T("Sound"), wxLIST_FORMAT_LEFT, -1);
	SoundsListCtrl->AppendColumn(_T("Source"), wxLIST_FORMAT_LEFT, -1);
	SoundsListCtrl->AppendColumn(_T("Size (bytes)"), wxLIST_FORMAT_LEFT, -1);
	SoundsListCtrl->InsertItem(0, wxString("* No Sound"));
	SoundsListCtrl->SetItemData(0, CreateSortData("\0", -9999));
	SoundsListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
	soundListSizer->Add(SoundsListCtrl, 1, wxALIGN_LEFT);

	applySoundButton = new wxButton(soundPanel, ID_APPLYSOUNDBTN, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	applySoundButton->SetBitmap(wxBITMAP_PNG(APPLYSOUND));
	applySoundButton->SetToolTip(_T("Apply selected sound to Frame"));
	loadSoundButton = new wxButton(soundPanel, ID_ADDSOUND, _T("."), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	loadSoundButton->SetBitmap(wxBITMAP_PNG(Sound_Add));
	loadSoundButton->SetToolTip(_T("Load Sounds..."));
	delSoundButton = new wxButton(soundPanel, ID_DELSOUND, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	delSoundButton->SetBitmap(wxBITMAP_PNG(Sound_Del));
	delSoundButton->SetToolTip(_T("Delete Sound"));
	soundButtonSizer->Add(applySoundButton);
	soundButtonSizer->Add(loadSoundButton);
	soundButtonSizer->Add(delSoundButton);

	//soundPageSizer->SetFlexibleDirection(wxVERTICAL);
	soundPageSizer->Add(soundListSizer, 1, wxEXPAND | wxALL, 4);
	soundPageSizer->Add(soundButtonSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxRIGHT, 4);
	soundPageSizer->AddGrowableRow(0, 1);

	soundPanel->SetSizer(soundPageSizer);
	// =======================================================

	ResourcesNotebook->SetPageSize(wxSize(200, 300));
	ResourcesNotebook->AddPage(spritePanel, _T("Sprites"), true);
	ResourcesNotebook->AddPage(soundPanel, _T("Sounds"));
	SpritesListCtrl->Connect(ID_SELECTSPRITE, wxEVT_LIST_ITEM_RIGHT_CLICK, wxListEventHandler(DanFrame::OnSpriteContext));
	SoundsListCtrl->Connect(ID_SELECTSOUND, wxEVT_LIST_ITEM_RIGHT_CLICK, wxListEventHandler(DanFrame::OnSoundContext));
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
	xSpin = new wxSpinCtrl(controlBox1->GetStaticBox(), ID_XSPIN, _T("0"), wxDefaultPosition, wxSize(80, 24), wxTE_PROCESS_ENTER | wxSP_ARROW_KEYS | wxALIGN_RIGHT, -65535, 65535, 0);
	xText = new wxStaticText(controlBox1->GetStaticBox(), wxID_ANY, _T("X:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	ySpin = new wxSpinCtrl(controlBox1->GetStaticBox(), ID_YSPIN, _T("0"), wxDefaultPosition, wxSize(80, 24), wxTE_PROCESS_ENTER | wxSP_ARROW_KEYS | wxALIGN_RIGHT, -65535, 65535, 0);
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
	framesButtonSizer = new wxBoxSizer(wxVERTICAL);
	framesButtonSet1Sizer = new wxBoxSizer(wxHORIZONTAL);
	framesButtonSet2Sizer = new wxBoxSizer(wxHORIZONTAL);
	durationSizer = new wxBoxSizer(wxHORIZONTAL);
	timelineSlider = new wxSlider(controlBox2->GetStaticBox(), ID_TIMESLIDER, 1, 1, 2, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_TICKS | wxSL_MIN_MAX_LABELS);
	durationText = new wxStaticText(controlBox2->GetStaticBox(), wxID_ANY, _T("Tics:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	durationSpin = new wxSpinCtrl(controlBox2->GetStaticBox(), ID_DURATION, _T("1"), wxDefaultPosition, wxSize(80, 24), wxTE_PROCESS_ENTER | wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 65535, 1);
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
	timelineButtonSizer->Add(loopCheckBox, 0, wxTOP, 8);
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
	crosshairCheckBox->SetToolTip(_T("Draws a basic crosshair in the center of the viewport\nto help with visual alignment"));
	allowSoundCheckBox = new wxCheckBox(controlBox3->GetStaticBox(), wxID_ANY, _T("Play Sounds"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	allowSoundCheckBox->SetToolTip(_T("Toggle sound simulation during animation playback"));
	allowSoundCheckBox->SetValue(true);
	allowGhostCheckBox = new wxCheckBox(controlBox3->GetStaticBox(), wxID_ANY, _T("Ghosting"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	allowGhostCheckBox->SetToolTip(_T("Display 'ghost frames' number of previous animation frames\nblended into the viewport to help with motion and alignment"));
	stencilCheckBox = new wxCheckBox(controlBox3->GetStaticBox(), wxID_ANY, _T("Stencil"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	stencilCheckBox->SetToolTip(_T("Use the viewport to clip sprites, providing a more accurate in-game appearance"));
	ghostFramesText = new wxStaticText(controlBox3->GetStaticBox(), wxID_ANY, _T("Ghost Frames"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	ghostFramesSpin = new wxSpinCtrl(controlBox3->GetStaticBox(), wxID_ANY, _T("1"), wxDefaultPosition, wxSize(60, 24), wxTE_PROCESS_ENTER | wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 10, 1);
	ghostSizer = new wxBoxSizer(wxHORIZONTAL);
	ghostSizer->Add(ghostFramesSpin, 1, wxLEFT | wxRIGHT, 4);
	ghostSizer->AddSpacer(2);
	ghostSizer->Add(ghostFramesText, 1, wxTOP | wxRIGHT, 4);
	controlBox3->Add(crosshairCheckBox, 0, wxLEFT | wxRIGHT | wxTOP, 4);
	controlBox3->AddSpacer(2);
	controlBox3->Add(allowSoundCheckBox, 0, wxLEFT | wxRIGHT, 4);
	controlBox3->AddSpacer(2);
	controlBox3->Add(allowGhostCheckBox, 0, wxLEFT | wxRIGHT, 4);
	controlBox3->AddSpacer(2);
	controlBox3->Add(stencilCheckBox, 0, wxLEFT | wxRIGHT, 4);
	controlBox3->AddSpacer(2);
	controlBox3->Add(ghostSizer);

	controlSizer->Add(controlBox1, 0, wxEXPAND);
	controlSizer->Add(controlBox2, 1, wxEXPAND);
	controlSizer->Add(controlBox3, 0, wxEXPAND);
	controlSizer->AddGrowableCol(1, 1);

	renderSizer->Add(glCanvas, 1, wxEXPAND | wxFIXED_MINSIZE | wxALL, 4);
	renderSizer->Add(controlSizer, 1, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT, 4);
	renderSizer->AddGrowableCol(0, 1);
	renderSizer->AddGrowableRow(0, 1);
}

void DanFrame::CreateTimelineButtons()
{
	playFromStartButton = new wxButton(controlBox2->GetStaticBox(), ID_PLAYFROMSTART, _T("Play Animation From Start"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	playFromStartButton->SetBitmap(wxBITMAP_PNG(PlayAnimStart));
	playFromStartButton->SetToolTip(_T("Play Animation From Start"));
	timelineButtonSizer->Add(playFromStartButton, 0);

	prevFrameButton = new wxButton(controlBox2->GetStaticBox(), ID_PREVFRAME, _T("Previous Frame"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	prevFrameButton->SetBitmap(wxBITMAP_PNG(FrameBack));
	prevFrameButton->SetToolTip(_T("Previous Frame"));
	timelineButtonSizer->Add(prevFrameButton, 0);

	playButton = new wxButton(controlBox2->GetStaticBox(), ID_PLAY, _T("Play Animation"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	playButton->SetBitmap(wxBITMAP_PNG(PlayAnim));
	playButton->SetToolTip(_T("Play Animation"));
	timelineButtonSizer->Add(playButton, 0);

	nextFrameButton = new wxButton(controlBox2->GetStaticBox(), ID_NEXTFRAME, _T("Next Frame"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	nextFrameButton->SetBitmap(wxBITMAP_PNG(FrameForward));
	nextFrameButton->SetToolTip(_T("Next Frame"));
	timelineButtonSizer->Add(nextFrameButton, 0);

	addFrameButton = new wxButton(controlBox2->GetStaticBox(), ID_ADDFRAME, _T("Add Frame (uses previous frame's offsets)"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	addFrameButton->SetBitmap(wxBITMAP_PNG(FRAMEADD));
	addFrameButton->SetToolTip(_T("Add Frame"));
	framesButtonSet1Sizer->Add(addFrameButton, 0);

	delFrameButton = new wxButton(controlBox2->GetStaticBox(), ID_DELFRAME, _T("Delete Current Frame"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	delFrameButton->SetBitmap(wxBITMAP_PNG(FRAMEDEL));
	delFrameButton->SetToolTip(_T("Delete Frame"));
	framesButtonSet1Sizer->Add(delFrameButton, 0);

	insertFrameButton = new wxButton(controlBox2->GetStaticBox(), ID_INSERTFRAME, _T("Insert Frame after current"), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	insertFrameButton->SetBitmap(wxBITMAP_PNG(FRAMEINSERT));
	insertFrameButton->SetToolTip(_T("Insert Frame after current"));
	framesButtonSet2Sizer->Add(insertFrameButton, 0);

	batchButton = new wxButton(controlBox2->GetStaticBox(), ID_BATCHACTION, _T("Batch Action..."), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	batchButton->SetBitmap(wxBITMAP_PNG(SETOFFSET));
	batchButton->SetToolTip(_T("Batch Action..."));
	framesButtonSet2Sizer->Add(batchButton, 0);

	framesButtonSizer->Add(framesButtonSet1Sizer);
	framesButtonSizer->Add(framesButtonSet2Sizer);
}

void DanFrame::BuildStatusBar()
{
	// Create the status bar
	DanStatus = new DanStatusBar(this, wxSTB_DEFAULT_STYLE);
	this->SetStatusBar(DanStatus);

	//SetStatusText("Welcome...to Danimator!");
	//SetStatusText("");
}