#pragma once

#include "SpritePreview.h"

class TextualPanel : public wxPanel
{
	friend class TextualView;	// Hi I'm a lazy shit and like to introduce coupling into my code. Don't mind me!
	friend class DanFrame;
public:
	TextualPanel(wxWindow *parent);

	int hitIndex;
	int contextSelected;

	std::shared_ptr<CompositeTexture> GetActiveTexture();
	void RefreshLayerList(int selection = -1);
	void UpdateLayerInfo();
	void UpdateBlendInfo();
	void UpdateTextureInfo();
	void SetLayerPanel(bool enable);
	void SetBlendingPanel(bool enable);

	void ResetControls();

	void OnLayerContext(wxContextMenuEvent& event);
	void ShowLayerContextMenu(const wxPoint& pos, int selection);

	void OnRenameLayer(wxCommandEvent &event);
	void OnDuplicateLayer(wxCommandEvent &event);

	// Fuckkin make it public for now gaaaaah
	// Sprite list
	wxListView *SpritesListCtrl;
	wxButton *ApplyChangesButton;
	//wxButton *DiscardChangesButton;
	wxButton *ApplySpriteButton;
	wxButton *ViewTexturesCodeButton;
	//wxButton *ReturnToAnimationButton;

	// Layer list
	wxCheckListBox *LayerList;
	wxButton *AddLayerButton;
	wxButton *DeleteLayerButton;
	wxButton *MoveLayerUpButton;
	wxButton *MoveLayerDownButton;

	// glCanvas (overridden from SFMLControl)
	TextualView *textualView;

	// Layer controls
	wxStaticText *xText;
	wxStaticText *yText;
	wxSpinCtrl *xSpin;
	wxSpinCtrl *ySpin;
	wxCheckBox *FlipXBox;
	wxCheckBox *FlipYBox;
	wxStaticText *rotationText;
	wxChoice *RotationChoice;	// rotates CW, keeps origin (so I should e.g. rotate around the center of the sprite)
	wxStaticText *currentPatchText;
	wxTextCtrl *currentPatchCtrl;
	
	// Blend controls
	wxStaticText *blendColorText;
	wxColourPickerCtrl *blendColorCtrl;
	//wxStaticText *blendAlphaText;
	//wxSpinCtrl *blendAlphaSpin;
	wxStaticText *translucencyText;
	wxChoice *TranslucencyChoice;
	wxStaticText *opacityText;
	wxSpinCtrl *opacitySpin;

	// Texture Controls
	wxStaticText *widthText;
	wxSpinCtrl *widthSpin;
	wxStaticText *heightText;
	wxSpinCtrl *heightSpin;
	wxStaticText *xScaleText;
	wxSpinCtrl *xScaleSpin;
	wxStaticText *yScaleText;
	wxSpinCtrl *yScaleSpin;
	wxStaticText *namespaceText;
	wxChoice *namespaceChoice;
	wxCheckBox *stencilBox;


	wxImagePanel *spritePreviewer;

	// Sizers
	// Main sizers
	wxFlexGridSizer *TextualSizer;
	// Sprites
	wxBoxSizer *spriteSizer;
	wxBoxSizer *spriteButtonSizer;
	// Layers
	wxBoxSizer *layerSizer;
	wxBoxSizer *layerButtonSizer;
	//Canvas Controls
	wxFlexGridSizer *renderSizer;
	// Main Canvas
	wxBoxSizer *canvasSizer;
	// StaticBox control sizer
	wxFlexGridSizer *controlSizer;
	// Layer sizers
	wxBoxSizer *offsetsSizer;
	wxBoxSizer *flipSizer;
	wxBoxSizer *currentSpriteSizer;
	// Blend sizers
	wxBoxSizer *blendSizer;
	// Texture sizers
	wxBoxSizer *dimsSizer;
	wxBoxSizer *scalesSizer;
	//wxBoxSizer *optionalSizer;
	wxStaticBoxSizer *layerControlsGroup;
	wxStaticBoxSizer *blendControlsGroup;
	wxStaticBoxSizer *textureControlsGroup;

	wxDECLARE_EVENT_TABLE();

private:
	int GetActualSelection();

	void OnSelectPatch(wxListEvent &event);
	void OnPatchSelect(wxListEvent &event);
	void OnPatchDeselect(wxListEvent &event);
	void OnApplyChanges(wxCommandEvent &event);
	//void OnDiscardChanges(wxCommandEvent &event);
	void OnApplyPatch(wxCommandEvent &event);
	void OnViewTexturesCode(wxCommandEvent &event);
	void OnSelectLayer(wxCommandEvent &event);
	void OnToggleLayer(wxCommandEvent &event);
	void OnAddLayer(wxCommandEvent &event);
	void OnDeleteLayer(wxCommandEvent &event);
	void OnLayerUp(wxCommandEvent &event);
	void OnLayerDown(wxCommandEvent &event);
	void OnXOriginChange(wxSpinEvent &event);
	void OnYOriginChange(wxSpinEvent &event);
	void OnXFlip(wxCommandEvent &event);
	void OnYFlip(wxCommandEvent &event);
	void OnRotationChange(wxCommandEvent &event);
	void OnColorChange(wxColourPickerEvent &event);
	void OnStyleChange(wxCommandEvent &event);
	void OnOpacityChange(wxSpinEvent &event);
	void OnWidthChange(wxSpinEvent &event);
	void OnHeightChange(wxSpinEvent &event);
	void OnXScaleChange(wxSpinEvent &event);
	void OnYScaleChange(wxSpinEvent &event);
	void OnNamespaceChange(wxCommandEvent &event);

	void BuildSpriteList();
	void BuildLayerList();
	void BuildCanvas();

	void ConstructSpriteListItems();

	wxUIntPtr m_activeData;
	std::shared_ptr<CompositeTexture> m_activeTexture;
	wxString m_textureName;
	//int m_activeLayer;
};

enum
{
	ID_SELECTPATCH = 170,
	ID_PATCHBUTTON,
	ID_SAVECHANGES,
	ID_DISCARDCHANGES,
	ID_VIEWTEXTUREXCODE,

	ID_SELECTLAYER,
	ID_ADDLAYER,
	ID_DELETELAYER,
	ID_LAYERUP,
	ID_LAYERDOWN,

	ID_XORIGIN,
	ID_YORIGIN,
	ID_FLIPX,
	ID_FLIPY,
	ID_ROTATION,
	
	ID_COLORCHOOSE,
	ID_COLORALPHA,
	ID_TRANSLUCENTSTYLE,
	ID_OPACITY,

	ID_WIDTH,
	ID_HEIGHT,
	ID_XSCALE,
	ID_YSCALE,
	ID_NAMESPACE,



	ID_CONTEXT_LAYER_RENAME,
	ID_CONTEXT_LAYER_DUPLICATE,

	TEXTUAL_IDS
};

enum RenderStyles
{
	STYLE_NORMAL = 0,
	STYLE_COPYALPHA = 0,
	STYLE_COPY,
	STYLE_TRANSLUCENT,
	STYLE_ADD,
	STYLE_COPYNEWALPHA,
	STYLE_MODULATE,
	STYLE_OVERLAY,
	STYLE_SUBTRACT,
	STYLE_REVERSESUBTRACT,
};