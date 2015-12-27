#pragma once

#include "SFMLControl.h"
#include "SpritePreview.h"
#include "batchaction.h"

#define DANVERSION "BETA 2"

class SpriteDropTarget : public wxFileDropTarget
{
public:
	SpriteDropTarget() { }

	virtual bool OnDropFiles(wxCoord x, wxCoord y,
							 const wxArrayString& filenames) wxOVERRIDE;

private:
};

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
bool ValidateSpriteFormat(const std::string &sprName);

long CreateSortData(wxString _name, int hashNum);

enum
{
	SAVEDATA_NONE = 0,
	SAVEDATA_STATES = BIT(0),
	SAVEDATA_TEXTURES = BIT(1),
	SAVEDATA_COMPOSITES = BIT(2),
	SAVEDATA_SOUNDS = BIT(3),

	SAVEDATA_ALL = SAVEDATA_STATES | SAVEDATA_TEXTURES | SAVEDATA_COMPOSITES | SAVEDATA_SOUNDS,
};

class DanFrame : public wxFrame
{
public:
	DanFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	Animator &GetAnimator();

	void UpdateAll();
	void RebuildStateList();

	void ActionStatus(wxString msg);

	// Old save format compatibility
	void SaveProjectLegacy(const wxString &fileName);
	void LoadProjectLegacy(const wxString &fileName);


	void SaveProject(const wxString &fileName, int toSave = SAVEDATA_ALL, bool forceEmbed = false);
	void LoadProject(const wxString &fileName);
	void SetPlayingMode(bool mode, bool fromStart = false);
	void SetControls(bool controls);
	void SetStatelessControls(bool controls);
	void SetStartupMode(bool set);

	void SetEditorMode(int mode);

	void UpdateSpriteResource(wxUIntPtr sprName, int column, wxString newData);

	wxString GetDecorateCode(const std::string &stateName);
	wxString GetAllDecorateCode();

	wxString GetAllTexturesCode();
	wxString ProcessTextureDependency(const std::string &compTex);
	
	int OnHotkey(wxKeyEvent& event);

//protected:
	// Panels
	wxPanel *DanPanel;
	DanStatusBar *DanStatus;

	// Menu Bar
	wxMenu *menuFile;
	wxMenu *menuEdit;
	wxMenu *menuView;
	wxMenu *menuProject;
	wxMenu *menuImport;
	wxMenu *menuExport;
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
	wxButton *applySpriteButton;
	wxButton *loadSpriteButton;
	wxButton *createTexturesButton;
	wxButton *delSpriteButton;
	wxButton *exportTexturesButton;
	wxListView *SoundsListCtrl;
	wxButton *applySoundButton;
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
	wxButton *insertFrameButton;
	wxButton *batchButton;
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
	wxBoxSizer *framesButtonSet1Sizer;
	wxBoxSizer *framesButtonSet2Sizer;
	wxStaticBoxSizer *controlBox3;
	wxBoxSizer *ghostSizer;

	DanCode *codeView;

	TextualPanel *textualPanel;
	wxImagePanel *spritePreviewer;

	bool playingMode;
	bool saved;
	bool inStartup;
	bool inModal;
	int hitIndex;
	wxString projectName;

	void ShowStateContextMenu(const wxPoint& pos, int selection);
	void ShowSpriteContextMenu(const wxPoint& pos, int selection);
	void ShowSoundContextMenu(const wxPoint& pos, int selection);

private:
	void WriteGIF(const std::string &filename);

	Frame_t m_copiedFrame;

	std::set<std::string> processed;

	int editorMode;
	CompositeTexture m_savedTexture;

	wxTimer *statusTimer;
	void OnTimer(wxTimerEvent& event);

	void OnIdle(wxIdleEvent& event);

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
	bool errorFlag;

	void OnExportGIF(wxCommandEvent& event);

	void OnExportResources(wxCommandEvent& event);
	void OnImportResources(wxCommandEvent& event);
	void OnImportSndinfo(wxCommandEvent& event);

	void OnUndo(wxCommandEvent& event);
	void OnRedo(wxCommandEvent& event);

	//void OnContextMenu(wxContextMenuEvent& event);
	void OnStateContext(wxListEvent &event);
	void OnSpriteContext(wxListEvent &event);
	void OnSoundContext(wxListEvent &event);
	//void OnRightUp(wxMouseEvent& event);
	void OnStateRename(wxCommandEvent &event);
	void OnStateDuplicate(wxCommandEvent &event);

	void OnSpriteEdit(wxCommandEvent &event);
	void OnSpriteRename(wxCommandEvent &event);
	void OnSpriteDuplicate(wxCommandEvent &event);
	void OnSpriteViewCode(wxCommandEvent &event);

	void OnSoundRename(wxCommandEvent &event);

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
	void OnStateDeselect(wxListEvent& event);
	
	void OnAddSprite(wxCommandEvent& event);
	void OnDeleteSprite(wxCommandEvent& event);
	void OnAddSound(wxCommandEvent& event);
	void OnDeleteSound(wxCommandEvent& event);
	void OnSelectSprite(wxListEvent& event);
	void OnSelectSound(wxListEvent& event);
	void OnSpriteDeselect(wxListEvent& event);

	void OnSpriteClicked(wxListEvent& event);
	void OnSoundClicked(wxListEvent& event);

	void OnApplySprite(wxCommandEvent &event);
	void OnCreateTexture(wxCommandEvent &event);

	void OnApplySound(wxCommandEvent &event);

	void OnAddFrame(wxCommandEvent& event);
	void OnDeleteFrame(wxCommandEvent& event);
	void OnInsertFrame(wxCommandEvent& event);
	void OnBatchAction(wxCommandEvent& event);


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
	
	void OnExportTextures(wxCommandEvent& event);

	void OnCopyFrame(wxCommandEvent& event);
	void OnPasteFrame(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

enum
{
	EDITMODE_ANIMATION = 0,
	EDITMODE_TEXTURES,
};

enum
{
	ID_NEWPROJECT = 1,
	ID_SAVEPROJECT,
	ID_SAVEPROJECTAS,
	ID_OPENPROJECT,
	ID_CLOSEPROJECT,
	ID_UNDO,
	ID_REDO,
	ID_IMPORTSNDINFO,
	ID_IMPORTRESOURCES,
	ID_EXPORTRESOURCES,
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
	ID_EXPORTTEXTURES,

	ID_APPLYSPRITEBTN,
	ID_CREATETEXTURE,

	ID_APPLYSOUNDBTN,

	ID_STATUS_RESIZE,
	ID_STATUS_ZOOM,
	ID_STATUS_TIMER,

	ID_INSERTFRAME,
	ID_BATCHACTION,

	//ID_CONTEXT_STATE,
	ID_CONTEXT_STATE_RENAME,
	ID_CONTEXT_STATE_DUPLICATE,
	ID_CONTEXT_STATE_VIEWCODE,
	ID_CONTEXT_STATE_EXPORTGIF,

	ID_CONTEXT_SPRITE_EDIT,
	ID_CONTEXT_SPRITE_RENAME,
	ID_CONTEXT_SPRITE_DUPLICATE,
	ID_CONTEXT_SPRITE_VIEWCODE,

	ID_CONTEXT_SOUND_RENAME,


	MAINFRAME_IDS
};