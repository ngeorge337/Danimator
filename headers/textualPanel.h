#pragma once

class TextualPanel : public wxPanel
{
	friend class TextualView;	// Hi I'm a lazy shit and like to introduce coupling into my code. Don't mind me!
public:
	TextualPanel(wxWindow *parent);


	// Fuckkin make it public for now gaaaaah
	// Sprite list
	wxListView *SpritesListCtrl;
	wxButton *ApplyChangesButton;
	wxButton *DiscardChangesButton;
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
	wxStaticText *blendAlphaText;
	wxSpinCtrl *blendAlphaSpin;
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


	// Sizers
	// Main sizers
	wxFlexGridSizer *TextualSizer;
	// Sprites
	wxFlexGridSizer *spriteSizer;
	wxBoxSizer *spriteButtonSizer;
	// Layers
	wxFlexGridSizer *layerSizer;
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

private:
	void BuildSpriteList();
	void BuildLayerList();
	void BuildCanvas();

	void ConstructSpriteListItems() {}

	std::shared_ptr<CompositeTexture> m_activeTexture;
};

enum
{
	ID_SELECTPATCH = 32,
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
};