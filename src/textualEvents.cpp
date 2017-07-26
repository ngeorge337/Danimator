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
#include "services.h"
#include "codewindow.h"
#include "textualPanel.h"
#include "mainFrame.h"

wxBEGIN_EVENT_TABLE(TextualPanel, wxPanel)
EVT_BUTTON(ID_PATCHBUTTON, TextualPanel::OnApplyPatch)
EVT_BUTTON(ID_SAVECHANGES, TextualPanel::OnApplyChanges)
EVT_BUTTON(ID_VIEWTEXTUREXCODE, TextualPanel::OnViewTexturesCode)
EVT_BUTTON(ID_ADDLAYER, TextualPanel::OnAddLayer)
EVT_BUTTON(ID_DELETELAYER, TextualPanel::OnDeleteLayer)
EVT_BUTTON(ID_LAYERUP, TextualPanel::OnLayerUp)
EVT_BUTTON(ID_LAYERDOWN, TextualPanel::OnLayerDown)
EVT_LIST_ITEM_ACTIVATED(ID_SELECTPATCH, TextualPanel::OnSelectPatch)
EVT_LIST_ITEM_SELECTED(ID_SELECTPATCH, TextualPanel::OnPatchSelect)
EVT_LIST_ITEM_DESELECTED(ID_SELECTPATCH, TextualPanel::OnPatchDeselect)
EVT_SPINCTRL(ID_XORIGIN, TextualPanel::OnXOriginChange)
EVT_SPINCTRL(ID_YORIGIN, TextualPanel::OnYOriginChange)
EVT_CHECKBOX(ID_FLIPX, TextualPanel::OnXFlip)
EVT_CHECKBOX(ID_FLIPY, TextualPanel::OnYFlip)
EVT_CHOICE(ID_ROTATION, TextualPanel::OnRotationChange)
EVT_COLOURPICKER_CHANGED(ID_COLORCHOOSE, TextualPanel::OnColorChange)
EVT_CHOICE(ID_TRANSLUCENTSTYLE, TextualPanel::OnStyleChange)
EVT_SPINCTRL(ID_OPACITY, TextualPanel::OnOpacityChange)
EVT_SPINCTRL(ID_WIDTH, TextualPanel::OnWidthChange)
EVT_SPINCTRL(ID_HEIGHT, TextualPanel::OnHeightChange)
EVT_SPINCTRL(ID_XSCALE, TextualPanel::OnXScaleChange)
EVT_SPINCTRL(ID_YSCALE, TextualPanel::OnYScaleChange)
EVT_CHOICE(ID_NAMESPACE, TextualPanel::OnNamespaceChange)
EVT_CHECKLISTBOX(ID_SELECTLAYER, TextualPanel::OnToggleLayer)
EVT_LISTBOX(ID_SELECTLAYER, TextualPanel::OnSelectLayer)
EVT_MENU(ID_CONTEXT_LAYER_RENAME, TextualPanel::OnRenameLayer)
EVT_MENU(ID_CONTEXT_LAYER_DUPLICATE, TextualPanel::OnDuplicateLayer)
wxEND_EVENT_TABLE()

extern DanFrame *theFrame;


void TextualPanel::OnPatchSelect(wxListEvent &event)
{
	spritePreviewer->SetImageFromTexture(SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString());
	spritePreviewer->Refresh();
}

void TextualPanel::OnPatchDeselect(wxListEvent &event)
{
	spritePreviewer->ClearImage();
	spritePreviewer->Refresh();
}

void TextualPanel::OnLayerContext(wxContextMenuEvent& event)
{
	wxPoint pos = ScreenToClient(wxGetMousePosition());
	ShowLayerContextMenu(pos, event.GetSelection());
}

void TextualPanel::OnNamespaceChange(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr)
		return;

	m_activeTexture->SetNamespace(namespaceChoice->GetStringSelection().ToStdString());
	UpdateTextureInfo();
	theFrame->saved = false;
}

void TextualPanel::OnYScaleChange(wxSpinEvent &event)
{
	if(m_activeTexture == nullptr)
		return;

	m_activeTexture->SetScaleFactor(xScaleSpin->GetValue(), yScaleSpin->GetValue());
	UpdateTextureInfo();
	theFrame->saved = false;
}

void TextualPanel::OnXScaleChange(wxSpinEvent &event)
{
	if(m_activeTexture == nullptr)
		return;

	m_activeTexture->SetScaleFactor(xScaleSpin->GetValue(), yScaleSpin->GetValue());
	UpdateTextureInfo();
	theFrame->saved = false;
}

void TextualPanel::OnHeightChange(wxSpinEvent &event)
{
	if(m_activeTexture == nullptr)
		return;

	m_activeTexture->SetDimensions(widthSpin->GetValue(), heightSpin->GetValue());
	UpdateTextureInfo();
	theFrame->saved = false;
}

void TextualPanel::OnWidthChange(wxSpinEvent &event)
{
	if(m_activeTexture == nullptr)
		return;

	m_activeTexture->SetDimensions(widthSpin->GetValue(), heightSpin->GetValue());
	UpdateTextureInfo();
	theFrame->saved = false;
}

void TextualPanel::OnOpacityChange(wxSpinEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;

	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Changed layer Opacity"));
	sf::Color color = m_activeTexture->GetLayer(GetActualSelection()).spr.getColor();
	color.a = opacitySpin->GetValue();

	m_activeTexture->GetLayer(GetActualSelection()).spr.setColor(color);
	m_activeTexture->GetLayer(GetActualSelection()).opacityValue = opacitySpin->GetValue();
	UpdateBlendInfo();
	theFrame->saved = false;
}

void TextualPanel::OnStyleChange(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;

	m_activeTexture->GetLayer(GetActualSelection()).renderStyle = TranslucencyChoice->GetSelection();
	UpdateBlendInfo();

	theFrame->saved = false;
}

void TextualPanel::OnColorChange(wxColourPickerEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;

	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Changed layer Color"));
	wxColour CtrlColor = blendColorCtrl->GetColour();
	sf::Color color;
	color.r = CtrlColor.Red();
	color.g = CtrlColor.Green();
	color.b = CtrlColor.Blue();
	color.a = 255;

	m_activeTexture->GetLayer(GetActualSelection()).spr.setColor(color);
	UpdateBlendInfo();
	theFrame->saved = false;
}

void TextualPanel::OnRotationChange(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;

	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Changed layer Rotation"));
	int val = RotationChoice->GetSelection();
	if(val == wxNOT_FOUND)
		val = 0;
	m_activeTexture->GetLayer(GetActualSelection()).rotValue = val;
	UpdateLayerInfo();
	theFrame->saved = false;
}

void TextualPanel::OnYFlip(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;

	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Flipped layer Y Axis"));
	m_activeTexture->GetLayer(GetActualSelection()).flipY = FlipYBox->GetValue();
	UpdateLayerInfo();
	theFrame->saved = false;
}

void TextualPanel::OnXFlip(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;

	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Flipped layer X Axis"));
	m_activeTexture->GetLayer(GetActualSelection()).flipX = FlipXBox->GetValue();
	UpdateLayerInfo();
	theFrame->saved = false;
}

void TextualPanel::OnYOriginChange(wxSpinEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;

	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Changed layer Y Origin"));
	float x = m_activeTexture->GetLayer(GetActualSelection()).spr.getPosition().x;
	float y = ySpin->GetValue();
	m_activeTexture->GetLayer(GetActualSelection()).spr.setPosition(x, y);
	UpdateLayerInfo();
	theFrame->saved = false;
}

void TextualPanel::OnXOriginChange(wxSpinEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;

	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Changed layer X Origin"));
	float x = xSpin->GetValue();
	float y = m_activeTexture->GetLayer(GetActualSelection()).spr.getPosition().y;
	m_activeTexture->GetLayer(GetActualSelection()).spr.setPosition(x, y);
	UpdateLayerInfo();
	theFrame->saved = false;
}

void TextualPanel::OnLayerDown(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;

	std::string lName = LayerList->GetStringSelection().ToStdString();
	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Moved layer '%s' down", lName));
	int selection = LayerList->GetSelection();
	int change = GetActualSelection();
	change -= m_activeTexture->MoveLayerTowardsBegin(GetActualSelection());
	selection += change;
		
	RefreshLayerList(selection);
	theFrame->saved = false;
}

void TextualPanel::OnLayerUp(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;

	std::string lName = LayerList->GetStringSelection().ToStdString();
	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Moved layer '%s' up", lName));
	int selection = LayerList->GetSelection();
	int change = GetActualSelection();
	change -= m_activeTexture->MoveLayerTowardsEnd(GetActualSelection());
	selection += change;

	RefreshLayerList(selection);
	theFrame->saved = false;
}

void TextualPanel::OnDeleteLayer(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;

	std::string lName = LayerList->GetStringSelection().ToStdString();
	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Delete layer '%s'", lName));
	m_activeTexture->DeleteLayer(GetActualSelection());
	RefreshLayerList(-1);
	theFrame->saved = false;
}

void TextualPanel::OnAddLayer(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr)
		return;

	std::string lName = "Layer" + std::to_string(m_activeTexture->GetAllLayers().size() + 1);
	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Added layer '%s'", lName));
	m_activeTexture->AddLayer(lName);
	RefreshLayerList(0);
	theFrame->saved = false;
}

void TextualPanel::OnToggleLayer(wxCommandEvent &event)
{
	// This should probably never happen here
	if(m_activeTexture == nullptr)
		return;
	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Toggle visibility for layer '%s'", LayerList->GetStringSelection().ToStdString()));
	int index = m_activeTexture->GetAllLayers().size() - 1; 
	index -= event.GetInt();
	m_activeTexture->GetLayer(index).isVisible = LayerList->IsChecked(event.GetInt());
	theFrame->saved = false;
}

void TextualPanel::OnViewTexturesCode(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr)
		return;
	if(theFrame->codeView)
		theFrame->codeView->Close();
	theFrame->codeView = new DanCode(m_activeTexture->GetTextureCode(), this, _T("View TEXTURES Code"), wxDefaultPosition, wxSize(300, 450));
}

void TextualPanel::OnApplyPatch(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1 || SpritesListCtrl->GetFirstSelected() == -1)
		return;

	std::string texName = SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString();
	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Changed patch of layer '%s' to %s", LayerList->GetStringSelection().ToStdString(), texName));
	if(!SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).CmpNoCase(wxString("* No Sprite (TNT1A0)")))
		texName = "TNT1A0";

	m_activeTexture->GetLayer(GetActualSelection()).spriteName = texName;
	m_activeTexture->GetLayer(GetActualSelection()).spr.setTexture(*Services::GetTextureManager()->GetTexture(texName), true);

	UpdateLayerInfo();
	UpdateBlendInfo();

	theFrame->saved = false;
}

void TextualPanel::OnApplyChanges(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr)
		return;

	m_activeTexture->CreateComposite();
	theFrame->GetAnimator().EnsureSpritePointers();

	// we should probably update the information in the Sprite list of the animation mode
	sf::Vector2i dims = m_activeTexture->GetDimensions();
	theFrame->UpdateSpriteResource(m_activeData, COL_SPRITES_SOURCE, wxString("TEXTURES"));
	theFrame->UpdateSpriteResource(m_activeData, COL_SPRITES_DIMS, std::to_string(dims.x) + "x" + std::to_string(dims.y));
	theFrame->UpdateSpriteResource(m_activeData, COL_SPRITES_SIZE, "");

	theFrame->saved = false;
	theFrame->SetEditorMode(EDITMODE_ANIMATION);
}

void TextualPanel::OnSelectPatch(wxListEvent &event)
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return;
	std::string texName = SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).ToStdString();
	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Changed patch of layer '%s' to %s", LayerList->GetStringSelection().ToStdString(), texName));
	if(!SpritesListCtrl->GetItemText(SpritesListCtrl->GetFirstSelected()).CmpNoCase(wxString("* No Sprite (TNT1A0)")))
		texName = "TNT1A0";

	m_activeTexture->GetLayer(GetActualSelection()).spriteName = texName;
	m_activeTexture->GetLayer(GetActualSelection()).spr.setTexture(*Services::GetTextureManager()->GetTexture(texName), true);

	UpdateLayerInfo();
	UpdateBlendInfo();

	theFrame->saved = false;
}

void TextualPanel::OnSelectLayer(wxCommandEvent &event)
{
	if(m_activeTexture == nullptr)
		return;
	
	SetLayerPanel(LayerList->GetSelection() != -1);
	SetBlendingPanel(LayerList->GetSelection() != -1);

	UpdateLayerInfo();
	UpdateBlendInfo();
}

void TextualPanel::OnRenameLayer(wxCommandEvent &event)
{
	theFrame->inModal = true;
	wxString name = LayerList->GetString(hitIndex);
	wxTextEntryDialog *txt = new wxTextEntryDialog(this, _T("Rename layer: ") + name, _T("Rename"), name);
	if(txt->ShowModal() == wxID_OK)
	{
		if(txt->GetValue().IsEmpty())
		{
			theFrame->inModal = false;
			return;
		}
		m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, "Change layer name");
		LayerList->SetString(hitIndex, txt->GetValue());
		int r = m_activeTexture->GetAllLayers().size() - 1;
		r -= hitIndex;
		m_activeTexture->GetLayer(r).layerName = txt->GetValue().ToStdString();
		theFrame->saved = false;
	}
	theFrame->inModal = false;
}

void TextualPanel::OnDuplicateLayer(wxCommandEvent &event)
{
	m_activeTexture->GetActionHistory().Insert(CMDTYPE_TEXTURES, FormatString("Duplicate layer %s", LayerList->GetString(hitIndex).ToStdString()));
	int r = m_activeTexture->GetAllLayers().size() - 1;
	r -= hitIndex;
	m_activeTexture->AddLayer(LayerList->GetString(hitIndex).ToStdString() + " Copy");
	m_activeTexture->GetAllLayers().back() = CompositeLayer_t(m_activeTexture->GetLayer(r));
	m_activeTexture->GetAllLayers().back().isVisible = true;
	m_activeTexture->GetAllLayers().back().layerName = LayerList->GetString(hitIndex).ToStdString() + " Copy";
	RefreshLayerList(0);
	theFrame->saved = false;
}