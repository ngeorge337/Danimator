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

extern DanFrame *theFrame;

TextualPanel::TextualPanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER)
{
	this->SetWindowStyle(wxNO_BORDER);
	TextualSizer = new wxFlexGridSizer(0, 2, 0, 2);
	wxBoxSizer *tempSizer = new wxBoxSizer(wxVERTICAL);

	BuildSpriteList();
	BuildLayerList();
	BuildCanvas();


	// Finalize Sizers
	tempSizer->Add(spriteSizer, 2, wxEXPAND);
	tempSizer->Add(layerSizer, 1, wxEXPAND);
	tempSizer->Layout();
	TextualSizer->Add(tempSizer, 1, wxEXPAND);
	TextualSizer->Add(renderSizer, 1, wxEXPAND);
	TextualSizer->AddGrowableCol(1, 1);
	TextualSizer->AddGrowableRow(0, 1);
	this->SetSizerAndFit(TextualSizer);
	SetLayerPanel(LayerList->GetSelection() != -1);
	SetBlendingPanel(LayerList->GetSelection() != -1);
}

void TextualPanel::ConstructSpriteListItems()
{
	long index = 0;
	SpritesListCtrl->DeleteAllItems();
	SpritesListCtrl->InsertItem(0, wxString("* No Sprite (TNT1A0)"));
	SpritesListCtrl->SetItemData(0, CreateSortData("\0", -9999));
	TextureManager *tm = Services::GetTextureManager();
	for(auto it = tm->texmap.begin(); it != tm->texmap.end(); ++it)
	{
		if(it->first == "TNT1A0")
			continue;

		index = SpritesListCtrl->InsertItem(SpritesListCtrl->GetItemCount(), it->first);
		SpritesListCtrl->SetItemData(index, CreateSortData(it->first, ComputeStringHash(SpritesListCtrl->GetItemText(index).ToStdString())));
		int width = it->second.ptr->getSize().x;
		int height = it->second.ptr->getSize().y;
		SpritesListCtrl->SetItem(index, 1, std::to_string(width) + "x" + std::to_string(height));
	}
	for(auto itx = tm->compmap.begin(); itx != tm->compmap.end(); ++itx)
	{
		// The last check prevents circular dependencies (composite texture that requires another composite texture, but that one requires the current)
		if(itx->first == "TNT1A0" || itx->first == m_textureName.ToStdString() || itx->second->HasDependency(m_textureName.ToStdString()))
			continue;

		index = SpritesListCtrl->InsertItem(SpritesListCtrl->GetItemCount(), itx->first);
		SpritesListCtrl->SetItemData(index, CreateSortData(itx->first, ComputeStringHash(SpritesListCtrl->GetItemText(index).ToStdString()) + 500000));
		int width = itx->second->GetDimensions().x;
		int height = itx->second->GetDimensions().y;
		SpritesListCtrl->SetItem(index, 1, std::to_string(width) + "x" + std::to_string(height));
	}
	SpritesListCtrl->SortItems(ListStringComparison, 0);
}

void TextualPanel::SetBlendingPanel(bool enable)
{
	blendColorCtrl->Enable(enable);
	//blendAlphaSpin->Enable(enable);
	TranslucencyChoice->Enable(enable);
	opacitySpin->Enable(enable);
}

void TextualPanel::SetLayerPanel(bool enable)
{
	xSpin->Enable(enable);
	ySpin->Enable(enable);
	FlipXBox->Enable(enable);
	FlipYBox->Enable(enable);
	RotationChoice->Enable(enable);
	currentPatchCtrl->Enable(enable);
}

void TextualPanel::UpdateTextureInfo()
{
	if(GetActiveTexture() == nullptr)
		return;

	int width = GetActiveTexture()->GetDimensions().x;
	int height = GetActiveTexture()->GetDimensions().y;
	widthSpin->SetValue(width);
	heightSpin->SetValue(height);
	xScaleSpin->SetValue(GetActiveTexture()->GetScaleFactor().x * 100);
	yScaleSpin->SetValue(GetActiveTexture()->GetScaleFactor().y * 100);
	namespaceChoice->SetStringSelection(GetActiveTexture()->GetNamespace());
}

void TextualPanel::UpdateBlendInfo()
{
	if(GetActiveTexture() == nullptr)
		return;
	int activeLayer = GetActualSelection();
	if(activeLayer == -1)
		return;

	sf::Uint8 redVal = GetActiveTexture()->GetLayer(activeLayer).spr.getColor().r;
	sf::Uint8 blueVal = GetActiveTexture()->GetLayer(activeLayer).spr.getColor().g;
	sf::Uint8 greenVal = GetActiveTexture()->GetLayer(activeLayer).spr.getColor().b;
	wxColour c;
	c.Set(redVal, blueVal, greenVal);
	blendColorCtrl->SetColour(c);
	TranslucencyChoice->SetSelection(GetActiveTexture()->GetLayer(activeLayer).renderStyle);
	opacitySpin->SetValue(GetActiveTexture()->GetLayer(activeLayer).opacityValue);
}

void TextualPanel::UpdateLayerInfo()
{
	if(GetActiveTexture() == nullptr)
		return;
	int activeLayer = GetActualSelection();
	if(activeLayer == -1)
		return;
	xSpin->SetValue(GetActiveTexture()->GetLayer(activeLayer).spr.getPosition().x);
	ySpin->SetValue(GetActiveTexture()->GetLayer(activeLayer).spr.getPosition().y);
	FlipXBox->SetValue(GetActiveTexture()->GetLayer(activeLayer).flipX);
	FlipYBox->SetValue(GetActiveTexture()->GetLayer(activeLayer).flipY);
	RotationChoice->SetSelection(GetActiveTexture()->GetLayer(activeLayer).rotValue);
	currentPatchCtrl->SetValue(GetActiveTexture()->GetLayer(activeLayer).spriteName);
}

void TextualPanel::RefreshLayerList(int selection /*= -1*/)
{
	if(m_activeTexture == nullptr)
		return;
	std::vector<CompositeLayer_t> &layers = m_activeTexture->GetAllLayers();
	LayerList->Freeze();
	LayerList->Clear();
	wxArrayString layerItems;
	if(!layers.empty())
	{
		std::vector<std::string> strings;
		// insert in reverse
		// because items at the end are the "top most" layers
		for(int i = layers.size() - 1; i >= 0; i--)
		//for(int i = 0; i < layers.size(); i++)
		{
			strings.push_back(layers[i].layerName);
			//layerItems.Add(layers[i].layerName);
		}
		for(int y = 0; y < strings.size(); y++)
			layerItems.Add(strings[y]);
		LayerList->InsertItems(layerItems, 0);
		for(int z = 0, x = layers.size() - 1; z < layers.size(); z++, x--)
		{
			LayerList->Check(z, layers[x].isVisible);
		}
		LayerList->SetSelection(selection, true);
	}
	LayerList->Thaw();
	//m_activeLayer = selection;

	SetLayerPanel(LayerList->GetSelection() != -1);
	SetBlendingPanel(LayerList->GetSelection() != -1);

	UpdateLayerInfo();
	UpdateBlendInfo();
}

std::shared_ptr<CompositeTexture> TextualPanel::GetActiveTexture()
{
	return m_activeTexture;
}

void TextualPanel::ResetControls()
{
	SetLayerPanel(false);
	SetBlendingPanel(false);

	LayerList->Clear();
	UpdateLayerInfo();
	UpdateBlendInfo();
	UpdateTextureInfo();
	RefreshLayerList();
	spritePreviewer->ClearImage();
	spritePreviewer->Refresh();
	ConstructSpriteListItems();
}

int TextualPanel::GetActualSelection()
{
	if(m_activeTexture == nullptr || LayerList->GetSelection() == -1)
		return -1;

	int r = m_activeTexture->GetAllLayers().size() - 1;
	r -= LayerList->GetSelection();

	return r;
}

void TextualPanel::ShowLayerContextMenu(const wxPoint& pos, int selection)
{
	theFrame->textualPanel->hitIndex = theFrame->textualPanel->LayerList->HitTest(pos);
	theFrame->textualPanel->contextSelected = selection;
	if(theFrame->textualPanel->hitIndex != wxNOT_FOUND)
	{
		wxMenu context;
		context.Append(ID_CONTEXT_LAYER_RENAME, _T("Rename..."));
		context.Append(ID_CONTEXT_LAYER_DUPLICATE, _T("Duplicate..."));

		PopupMenu(&context, pos);
	}
}



