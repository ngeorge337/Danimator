#include "libs.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "CheckedListCtrl.h"
#include "SFMLControl.h"
#include "textualView.h"
#include "actionmanager.h"
#include "resrcman.h"
#include "texman.h"
#include "soundman.h"
#include "locator.h"
#include "textualPanel.h"
#include "mainFrame.h"

extern DanFrame *theFrame;

TextualPanel::TextualPanel(wxWindow *parent) : wxPanel(parent)
{
	TextualSizer = new wxFlexGridSizer(0, 2, 0, 2);
	wxBoxSizer *tempSizer = new wxBoxSizer(wxVERTICAL);

	BuildSpriteList();
	BuildLayerList();
	BuildCanvas();


	// Finalize Sizers
	tempSizer->Add(spriteSizer, 2, wxEXPAND);
	tempSizer->Add(layerSizer, 1, wxEXPAND);
	TextualSizer->Add(tempSizer, 1, wxEXPAND);
	TextualSizer->Add(renderSizer, 1, wxEXPAND);
	TextualSizer->AddGrowableCol(1, 1);
	TextualSizer->AddGrowableRow(0, 1);
	this->SetSizerAndFit(TextualSizer);
	//this->GetParent()->GetSizer()->Show(this, true, true);
	//this->GetParent()->GetSizer()->Layout();
}

void TextualPanel::BuildSpriteList()
{
	spriteSizer = new wxFlexGridSizer(2, 1, 0, 0);
	spriteButtonSizer = new wxBoxSizer(wxHORIZONTAL);

	SpritesListCtrl = new wxListView(this, ID_SELECTPATCH, wxPoint(0, 0), wxSize(200, 200), wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING | wxLC_REPORT | wxLC_VRULES);
	SpritesListCtrl->AppendColumn(_T("Sprite"), wxLIST_FORMAT_LEFT, -1);
	SpritesListCtrl->AppendColumn(_T("Source"), wxLIST_FORMAT_LEFT, -1);
	SpritesListCtrl->AppendColumn(_T("Size (bytes)"), wxLIST_FORMAT_LEFT, -1);
	SpritesListCtrl->AppendColumn(_T("Dims"), wxLIST_FORMAT_LEFT, -1);
	SpritesListCtrl->InsertItem(0, wxString("* No Sprite (TNT1A0)"));
	SpritesListCtrl->SetItemData(0, -9999);
	SpritesListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE);

	ApplyChangesButton = new wxButton(this, ID_SAVECHANGES, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	ApplyChangesButton->SetBitmap(wxBITMAP_PNG(APPLYTEX));
	ApplyChangesButton->SetToolTip(_T("Apply Changes and return\nto Animation mode"));
	DiscardChangesButton = new wxButton(this, ID_DISCARDCHANGES, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	DiscardChangesButton->SetBitmap(wxBITMAP_PNG(DISCARDTEX));
	DiscardChangesButton->SetToolTip(_T("Discard all Changes and return\nto Animation mode"));
	ApplySpriteButton = new wxButton(this, ID_PATCHBUTTON, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	ApplySpriteButton->SetBitmap(wxBITMAP_PNG(APPLYSPRITE));
	ApplySpriteButton->SetToolTip(_T("Apply selected sprite to current Layer"));
	ViewTexturesCodeButton = new wxButton(this, ID_VIEWTEXTUREXCODE, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	ViewTexturesCodeButton->SetBitmap(wxBITMAP_PNG(VIEWTEX));
	ViewTexturesCodeButton->SetToolTip(_T("View TEXTURES code"));

	spriteButtonSizer->Add(ApplyChangesButton);
	spriteButtonSizer->Add(DiscardChangesButton);
	spriteButtonSizer->Add(ApplySpriteButton);
	spriteButtonSizer->Add(ViewTexturesCodeButton);

	spriteSizer->Add(SpritesListCtrl, 1, wxEXPAND | wxALL, 4);
	spriteSizer->Add(spriteButtonSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxRIGHT, 4);
	spriteSizer->AddGrowableRow(0, 1);

	ConstructSpriteListItems();
}

void TextualPanel::BuildLayerList()
{
	layerSizer = new wxFlexGridSizer(2, 1, 0, 0);
	layerButtonSizer = new wxBoxSizer(wxHORIZONTAL);

	LayerList = new wxCheckListBox(this, ID_SELECTLAYER, wxPoint(0, 0), wxSize(200, 200), NULL, NULL, wxLB_SINGLE | wxLB_NEEDED_SB);

	AddLayerButton = new wxButton(this, ID_ADDLAYER, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	AddLayerButton->SetBitmap(wxBITMAP_PNG(LAYERADD));
	AddLayerButton->SetToolTip(_T("Add a new layer"));
	DeleteLayerButton = new wxButton(this, ID_DELETELAYER, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	DeleteLayerButton->SetBitmap(wxBITMAP_PNG(LAYERDEL));
	DeleteLayerButton->SetToolTip(_T("Delete selected layer"));
	MoveLayerUpButton = new wxButton(this, ID_LAYERUP, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	MoveLayerUpButton->SetBitmap(wxBITMAP_PNG(LAYERUP));
	MoveLayerUpButton->SetToolTip(_T("Move selected layer up"));
	MoveLayerDownButton = new wxButton(this, ID_LAYERDOWN, _T(""), wxPoint(0, 0), wxSize(32, 32), wxBU_NOTEXT);
	MoveLayerDownButton->SetBitmap(wxBITMAP_PNG(LAYERDOWN));
	MoveLayerDownButton->SetToolTip(_T("Move selected layer down"));

	layerButtonSizer->Add(AddLayerButton);
	layerButtonSizer->Add(DeleteLayerButton);
	layerButtonSizer->Add(MoveLayerUpButton);
	layerButtonSizer->Add(MoveLayerDownButton);

	layerSizer->Add(LayerList, 1, wxEXPAND | wxALL, 4);
	layerSizer->Add(layerButtonSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxRIGHT, 4);
	layerSizer->AddGrowableRow(0, 1);
}

void TextualPanel::BuildCanvas()
{
	renderSizer = new wxFlexGridSizer(2, 1, 8, 0);
	controlSizer = new wxFlexGridSizer(1, 3, 0, 4);

	// GL Canvas
	textualView = new TextualView(this, wxID_ANY, wxDefaultPosition, wxSize(320, 240), wxBORDER_SIMPLE);

	// Controls ========================================
	// Offsets and Layer info
	layerControlsGroup = new wxStaticBoxSizer(wxVERTICAL, this, _T("Layer"));
	offsetsSizer = new wxBoxSizer(wxHORIZONTAL);
	flipSizer = new wxBoxSizer(wxHORIZONTAL);
	currentSpriteSizer = new wxBoxSizer(wxHORIZONTAL);
	xSpin = new wxSpinCtrl(layerControlsGroup->GetStaticBox(), ID_XORIGIN, _T("0"), wxDefaultPosition, wxSize(80, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, -65535, 65535, 0);
	xText = new wxStaticText(layerControlsGroup->GetStaticBox(), wxID_ANY, _T("X:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	ySpin = new wxSpinCtrl(layerControlsGroup->GetStaticBox(), ID_YORIGIN, _T("0"), wxDefaultPosition, wxSize(80, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, -65535, 65535, 0);
	yText = new wxStaticText(layerControlsGroup->GetStaticBox(), wxID_ANY, _T("Y:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	FlipXBox = new wxCheckBox(layerControlsGroup->GetStaticBox(), ID_FLIPX, _T("Flip X"));
	FlipYBox = new wxCheckBox(layerControlsGroup->GetStaticBox(), ID_FLIPY, _T("Flip Y"));
	wxArrayString rotations;
	rotations.Add("0");
	rotations.Add("90");
	rotations.Add("180");
	rotations.Add("270");
	rotationText = new wxStaticText(layerControlsGroup->GetStaticBox(), wxID_ANY, _T("Rotation:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	RotationChoice = new wxChoice(layerControlsGroup->GetStaticBox(), ID_ROTATION, wxDefaultPosition, wxSize(140, 24), rotations, 0);
	RotationChoice->SetSelection(0);

	currentPatchText = new wxStaticText(layerControlsGroup->GetStaticBox(), wxID_ANY, _T("Sprite:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	currentPatchCtrl = new wxTextCtrl(layerControlsGroup->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);

	wxBoxSizer *xCtrlSizer = new wxBoxSizer(wxVERTICAL);
	offsetsSizer->Add(xText, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 4);
	offsetsSizer->AddSpacer(4);
	xCtrlSizer->Add(xSpin);
	xCtrlSizer->AddSpacer(2);
	xCtrlSizer->Add(FlipXBox);
	offsetsSizer->Add(xCtrlSizer, 0, wxALIGN_CENTER_HORIZONTAL);

	offsetsSizer->AddSpacer(4);

	wxBoxSizer *yCtrlSizer = new wxBoxSizer(wxVERTICAL);
	offsetsSizer->Add(yText, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 4);
	offsetsSizer->AddSpacer(4);
	yCtrlSizer->Add(ySpin);
	yCtrlSizer->AddSpacer(2);
	yCtrlSizer->Add(FlipYBox);
	offsetsSizer->Add(yCtrlSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	offsetsSizer->AddSpacer(8);

	wxBoxSizer *rotSizer = new wxBoxSizer(wxHORIZONTAL);
	rotSizer->Add(rotationText, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	rotSizer->AddSpacer(4);
	rotSizer->Add(RotationChoice, 0, wxEXPAND);

	currentSpriteSizer->Add(currentPatchText, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
	currentSpriteSizer->Add(currentPatchCtrl, 1, wxALIGN_CENTER_VERTICAL);
	layerControlsGroup->Add(offsetsSizer);
	layerControlsGroup->AddSpacer(4);
	layerControlsGroup->Add(flipSizer);
	layerControlsGroup->AddSpacer(4);
	layerControlsGroup->Add(rotSizer, 0, wxALIGN_LEFT);
	layerControlsGroup->AddSpacer(4);
	layerControlsGroup->Add(currentSpriteSizer, 1, wxEXPAND | wxRIGHT, 8);

	// Blending options
	blendControlsGroup = new wxStaticBoxSizer(wxHORIZONTAL, this, _T("Blending"));
	wxBoxSizer *blendTextCtrlSizer = new wxBoxSizer(wxVERTICAL);
	blendSizer = new wxBoxSizer(wxVERTICAL);

	blendColorText = new wxStaticText(blendControlsGroup->GetStaticBox(), wxID_ANY, _T("Color:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	blendColorCtrl = new wxColourPickerCtrl(blendControlsGroup->GetStaticBox(), ID_COLORCHOOSE, *wxWHITE, wxDefaultPosition, wxSize(100, 24));
	blendTextCtrlSizer->Add(blendColorText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 6);
	blendSizer->Add(blendColorCtrl, 0, wxALL, 2);

	blendAlphaText = new wxStaticText(blendControlsGroup->GetStaticBox(), wxID_ANY, _T("Alpha:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	blendAlphaSpin = new wxSpinCtrl(blendControlsGroup->GetStaticBox(), ID_COLORALPHA, _T("255"), wxDefaultPosition, wxSize(100, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 0, 255, 255);
	blendTextCtrlSizer->Add(blendAlphaText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 6);
	blendSizer->Add(blendAlphaSpin, 0, wxALL, 2);

	translucencyText = new wxStaticText(blendControlsGroup->GetStaticBox(), wxID_ANY, _T("Style:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	wxArrayString transStyles;
	transStyles.Add("Copy");
	transStyles.Add("CopyAlpha");
	transStyles.Add("CopyNewAlpha");
	transStyles.Add("Add");
	transStyles.Add("Modulate");
	transStyles.Add("Overlay");
	transStyles.Add("Subtract");
	transStyles.Add("Translucent");
	transStyles.Add("Reverse Subtract");
	TranslucencyChoice = new wxChoice(blendControlsGroup->GetStaticBox(), ID_TRANSLUCENTSTYLE, wxDefaultPosition, wxSize(100, 24), transStyles, 0);
	TranslucencyChoice->SetSelection(0);
	blendTextCtrlSizer->Add(translucencyText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 6);
	blendSizer->Add(TranslucencyChoice, 0, wxALL, 2);

	opacityText = new wxStaticText(blendControlsGroup->GetStaticBox(), wxID_ANY, _T("Opacity:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	opacitySpin = new wxSpinCtrl(blendControlsGroup->GetStaticBox(), ID_OPACITY, _T("255"), wxDefaultPosition, wxSize(100, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 0, 255, 255);
	blendTextCtrlSizer->Add(opacityText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 6);
	blendSizer->Add(opacitySpin, 0, wxALL, 2);

	blendControlsGroup->Add(blendTextCtrlSizer, 0, wxEXPAND | wxALL, 4);
	blendControlsGroup->Add(blendSizer, 1, wxTOP | wxRIGHT | wxBOTTOM, 4);

	// Texture settings
	textureControlsGroup = new wxStaticBoxSizer(wxVERTICAL, this, _T("Texture"));
	dimsSizer = new wxBoxSizer(wxHORIZONTAL);
	scalesSizer = new wxBoxSizer(wxHORIZONTAL);

	widthText = new wxStaticText(textureControlsGroup->GetStaticBox(), wxID_ANY, _T("Width:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	widthSpin = new wxSpinCtrl(textureControlsGroup->GetStaticBox(), ID_WIDTH, _T("64"), wxDefaultPosition, wxSize(50, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 9000, 64);
	heightText = new wxStaticText(textureControlsGroup->GetStaticBox(), wxID_ANY, _T("Height:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	heightSpin = new wxSpinCtrl(textureControlsGroup->GetStaticBox(), ID_HEIGHT, _T("64"), wxDefaultPosition, wxSize(50, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 9000, 64);
	dimsSizer->Add(widthText, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	dimsSizer->AddSpacer(4);
	dimsSizer->Add(widthSpin, 2, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	dimsSizer->AddSpacer(8);
	dimsSizer->Add(heightText, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	dimsSizer->AddSpacer(4);
	dimsSizer->Add(heightSpin, 2, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	//dimsSizer->AddSpacer(2);

	xScaleText = new wxStaticText(textureControlsGroup->GetStaticBox(), wxID_ANY, _T("X Scale %:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	xScaleSpin = new wxSpinCtrl(textureControlsGroup->GetStaticBox(), ID_XSCALE, _T("100"), wxDefaultPosition, wxSize(50, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 400, 100);
	yScaleText = new wxStaticText(textureControlsGroup->GetStaticBox(), wxID_ANY, _T("Y Scale %:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	yScaleSpin = new wxSpinCtrl(textureControlsGroup->GetStaticBox(), ID_YSCALE, _T("100"), wxDefaultPosition, wxSize(50, 24), wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 400, 100);
	scalesSizer->Add(xScaleText, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	scalesSizer->AddSpacer(4);
	scalesSizer->Add(xScaleSpin, 2, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	scalesSizer->AddSpacer(8);
	scalesSizer->Add(yScaleText, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	scalesSizer->AddSpacer(4);
	scalesSizer->Add(yScaleSpin, 2, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	//scalesSizer->AddSpacer(2);
	
	wxBoxSizer *namespaceSizer = new wxBoxSizer(wxHORIZONTAL);
	wxArrayString namespaces;
	namespaces.Add("Sprite");
	namespaces.Add("Texture");
	namespaces.Add("Graphic");
	namespaces.Add("WallTexture");
	namespaces.Add("Flat");
	namespaceText = new wxStaticText(textureControlsGroup->GetStaticBox(), wxID_ANY, _T("Namespace:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	namespaceChoice = new wxChoice(textureControlsGroup->GetStaticBox(), ID_NAMESPACE, wxDefaultPosition, wxSize(100, 24), namespaces, 0);
	namespaceChoice->SetSelection(0);
	namespaceSizer->Add(namespaceText, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	namespaceSizer->AddSpacer(4);
	namespaceSizer->Add(namespaceChoice, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);

	stencilBox = new wxCheckBox(textureControlsGroup->GetStaticBox(), ID_FLIPX, _T("Stencil"));

	textureControlsGroup->Add(dimsSizer, 0, wxEXPAND | wxALL, 4);
	textureControlsGroup->Add(scalesSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 4);
	textureControlsGroup->Add(namespaceSizer, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 4);
	textureControlsGroup->Add(stencilBox, 0, wxEXPAND | wxALL, 4);

	// Finalize
	controlSizer->Add(layerControlsGroup, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL);
	controlSizer->Add(blendControlsGroup, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL);
	controlSizer->Add(textureControlsGroup, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL);
	//controlSizer->AddGrowableCol(1, 1);

	renderSizer->Add(textualView, 1, wxEXPAND | wxFIXED_MINSIZE | wxALL, 4);
	renderSizer->Add(controlSizer, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxRIGHT | wxLEFT, 4);
	renderSizer->AddGrowableCol(0, 1);
	renderSizer->AddGrowableRow(0, 1);
}