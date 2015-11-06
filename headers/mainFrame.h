#pragma once

#include "SFMLControl.h"


#define DANVERSION "BETA 2"

class DanFrame : public wxFrame
{
public:
	DanFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	Animator &GetAnimator();

	void SaveProject(const wxString &fileName);
	void LoadProject(const wxString &fileName);
	void SetPlayingMode(bool mode, bool fromStart = false);
	void SetControls(bool controls);
	void SetStatelessControls(bool controls);
	void SetStartupMode(bool set);

	wxString GetDecorateCode(const std::string &stateName);
	wxString GetAllDecorateCode();

//protected:
	// Panels
	wxPanel *DanPanel;
	wxStatusBar *DanStatus;

	// Menu Bar
	wxMenu *menuFile;
	wxMenu *menuView;
	wxMenu *menuHelp;
	wxMenuBar *menuBar;

	// The States list
	wxListBox *StateListCtrl;
	wxButton *newStateButton;
	wxButton *delStateButton;
	wxButton *viewCodeButton;
	wxButton *exportCodeButton;

	// The resources notebook and lists
	wxPanel *spritePanel;
	wxPanel *soundPanel;
	wxNotebook *ResourcesNotebook;
	wxListBox *SpritesListCtrl;
	wxButton *loadSpriteButton;
	wxButton *delSpriteButton;
	wxListBox *SoundsListCtrl;
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
	void OnStateSelection(wxCommandEvent& event);

	void UpdateTimeline();

	void OnAddSprite(wxCommandEvent& event);
	void OnDeleteSprite(wxCommandEvent& event);
	void OnAddSound(wxCommandEvent& event);
	void OnDeleteSound(wxCommandEvent& event);
	void OnSelectSprite(wxCommandEvent& event);
	void OnSelectSound(wxCommandEvent& event);

	void OnAddFrame(wxCommandEvent& event);
	void OnDeleteFrame(wxCommandEvent& event);

	void EnableTimelineIfValid();
	void ResetFrame();
	void UpdateFlow();
	void UpdateFrameInfo();
	void UpdateSpins();

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
};