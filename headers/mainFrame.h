#pragma once

#include "SFMLControl.h"


#define DANVERSION "BETA 2"

class DanStatusBar : public wxStatusBar
{
public:
	DanStatusBar(wxWindow *parent, long style);

	void OnSize(wxSizeEvent& event);
	void OnZoomSliderChange(wxCommandEvent& event);

	void SetZoomValue(int val);
	
	wxSlider *zoomSlider;
	wxStaticText *zoomText;
	wxStaticText *zoomValue;

	wxDECLARE_EVENT_TABLE();
};

int wxCALLBACK ListStringComparison(wxIntPtr item1, wxIntPtr item2, wxIntPtr WXUNUSED(sortData));

int ComputeStringHash(const std::string &str);

class DanFrame : public wxFrame
{
public:
	DanFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	Animator &GetAnimator();

	void UpdateAll();
	void RebuildStateList();

	void ActionStatus(wxString msg);

	void SaveProject(const wxString &fileName);
	void LoadProject(const wxString &fileName);
	void SetPlayingMode(bool mode, bool fromStart = false);
	void SetControls(bool controls);
	void SetStatelessControls(bool controls);
	void SetStartupMode(bool set);

	wxString GetDecorateCode(const std::string &stateName);
	wxString GetAllDecorateCode();
	
	int OnHotkey(wxKeyEvent& event);

//protected:
	// Panels
	wxPanel *DanPanel;
	DanStatusBar *DanStatus;

	// Menu Bar
	wxMenu *menuFile;
	wxMenu *menuView;
	wxMenu *menuHelp;
	wxMenuBar *menuBar;

	// The States list
	//wxListBox *StateListCtrl;
	DanStateList *StateListCtrl;
	wxButton *newStateButton;
	wxButton *delStateButton;
	wxButton *viewCodeButton;
	wxButton *exportCodeButton;

	// The resources notebook and lists
	wxPanel *spritePanel;
	wxPanel *soundPanel;
	wxNotebook *ResourcesNotebook;
	wxListView *SpritesListCtrl;
	wxButton *loadSpriteButton;
	wxButton *delSpriteButton;
	wxListView *SoundsListCtrl;
	wxButton *loadSoundButton;
	wxButton *delSoundButton;

	// OpenGL Canvas
	wxSFMLCanvas *glCanvas;
	//wxGLCanvas *glCanvas;
	//wxGLContext *glContext;

	// Offset controls
	wxStaticText *xText;
	wxStaticText *yText;
	wxSpinCtrl *xSpin;
	wxSpinCtrl *ySpin;
	wxStaticText *currentSpriteText;
	wxTextCtrl *currentSpriteCtrl;
	wxStaticText *currentSoundText;
	wxTextCtrl *currentSoundCtrl;

	// Timeline controls
	wxSlider *timelineSlider;
	wxButton *prevFrameButton;
	wxButton *nextFrameButton;
	wxButton *playButton;
	wxButton *playFromStartButton;
	wxButton *addFrameButton;
	wxButton *delFrameButton;
	wxStaticText *durationText;
	wxSpinCtrl *durationSpin;
	wxCheckBox *loopCheckBox;
	wxStaticText *endText;
	wxChoice *endChoice;

	// Options
	wxCheckBox *crosshairCheckBox;
	wxCheckBox *allowSoundCheckBox;
	wxCheckBox *allowGhostCheckBox;
	wxCheckBox *stencilCheckBox;
	wxStaticText *ghostFramesText;
	wxSpinCtrl *ghostFramesSpin;

	// Sizers
	// Main sizers
	wxFlexGridSizer *DanSizer;
	// States
	wxFlexGridSizer *stateSizer;
	wxBoxSizer *stateListSizer;
	wxBoxSizer *stateButtonSizer;
	// Sprites
	wxFlexGridSizer *spritePageSizer;
	wxBoxSizer *spriteListSizer;
	wxBoxSizer *spriteButtonSizer;
	// Sounds
	wxFlexGridSizer *soundPageSizer;
	wxBoxSizer *soundListSizer;
	wxBoxSizer *soundButtonSizer;
	//Canvas Controls
	wxFlexGridSizer *renderSizer;
	wxBoxSizer *canvasSizer;
	wxFlexGridSizer *controlSizer;
	wxStaticBoxSizer *controlBox1;
	wxBoxSizer *offsetsSizer;
	wxBoxSizer *currentSpriteSizer;
	wxBoxSizer *currentSoundSizer;
	wxStaticBoxSizer *controlBox2;
	wxFlexGridSizer *timelineSizer;
	wxFlexGridSizer *timeControlSizer;
	wxFlexGridSizer *timeOptionsSizer;
	wxBoxSizer *durationSizer;
	wxBoxSizer *timelineButtonSizer;
	wxBoxSizer *framesButtonSizer;
	wxStaticBoxSizer *controlBox3;
	wxBoxSizer *ghostSizer;

	DanCode *codeView;

	bool playingMode;
	bool saved;
	bool inStartup;
	wxString projectName;

private:

	wxTimer *statusTimer;
	void OnTimer(wxTimerEvent& event);

	void OnIdle(wxIdleEvent& event);

	bool inModal;
	bool isStatusSet;

	// Construction
	void BuildMenuBar();
	void BuildStatusBar();
	void BuildStatesList();
	void BuildResourceLists();
	void BuildCanvasControls();
	void CreateTimelineButtons();

	// Animation stuff
	Animator animator;
	int playFrame;

	void OnClose(wxCloseEvent& event);

	void OnNewProject(wxCommandEvent& event);
	void OnSaveProject(wxCommandEvent& event);
	void OnSaveProjectAs(wxCommandEvent& event);
	void OnOpenProject(wxCommandEvent& event);
	void OnSettings(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

	void OnNewState(wxCommandEvent& event);
	void OnDeleteState(wxCommandEvent& event);
	void OnStateSelection(wxListEvent& event);
	
	void OnAddSprite(wxCommandEvent& event);
	void OnDeleteSprite(wxCommandEvent& event);
	void OnAddSound(wxCommandEvent& event);
	void OnDeleteSound(wxCommandEvent& event);
	void OnSelectSprite(wxListEvent& event);
	void OnSelectSound(wxListEvent& event);

	void OnAddFrame(wxCommandEvent& event);
	void OnDeleteFrame(wxCommandEvent& event);

	void EnableTimelineIfValid();
	void ResetFrame();
	void UpdateFlow();
	void UpdateFrameInfo();
	void UpdateSpins();
	void UpdateTimeline();

	void OnSliderChange(wxCommandEvent& event);

	void OnXSpinChange(wxSpinEvent& event);
	void OnYSpinChange(wxSpinEvent& event);

	void OnDurationSpinChange(wxSpinEvent& event);

	void OnPlay(wxCommandEvent& event);
	void OnPlayFromStart(wxCommandEvent& event);

	void OnNextFrame(wxCommandEvent& event);
	void OnPrevFrame(wxCommandEvent& event);
	
	void OnFlowGoto(wxCommandEvent& event);

	void OnViewCode(wxCommandEvent& event);
	void OnExportCode(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

enum
{
	ID_NEWPROJECT = 1,
	ID_SAVEPROJECT,
	ID_SAVEPROJECTAS,
	ID_OPENPROJECT,
	ID_CLOSEPROJECT,
	ID_SETTINGS,
	ID_ENDING,
	ID_NEWSTATE,
	ID_DELSTATE,
	ID_ADDSPRITE,
	ID_DELSPRITE,
	ID_ADDSOUND,
	ID_DELSOUND,
	ID_SELECTSPRITE,
	ID_SELECTSOUND,
	ID_ADDFRAME,
	ID_DELFRAME,
	ID_PREVFRAME,
	ID_NEXTFRAME,
	ID_TIMESLIDER,
	ID_PLAY,
	ID_PLAYFROMSTART,
	ID_STATELIST,
	ID_DURATION,
	ID_XSPIN,
	ID_YSPIN,
	ID_VIEWCODE,
	ID_EXPORTCODE,

	ID_STATUS_RESIZE,
	ID_STATUS_ZOOM,
	ID_STATUS_TIMER,
};