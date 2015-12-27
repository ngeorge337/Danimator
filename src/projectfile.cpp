#include "libs.h"
#include "alert.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "SFMLControl.h"
#include "textualView.h"
#include "textualPanel.h"
#include "mainFrame.h"
#include "codewindow.h"
#include "preferences.h"
#include "actionmanager.h"

extern DanFrame *theFrame;

#define DAN_FILEVERSION 2

enum eDataTypes
{
	DTYPE_EMPTY = 0,
	DTYPE_TEXTUREDIR,
	DTYPE_TEXTURE,
	DTYPE_COMPOSITE,
	DTYPE_SOUND,
	DTYPE_SOUNDDIR,
	DTYPE_STATE,

	MAX_TYPES
};

void DanFrame::SaveProject(const wxString &fileName, int toSave, bool forceEmbed)
{
	File arc;
	arc.Create(fileName.ToStdString(), FM_WRITE_BINARY);

	if(!arc.IsOpen())
		return;

	// Write the project version first and foremost
	// This is also the only way we can identify legacy projects
	// as a "1" or "0" for the embed flags of the original save
	// will also indicate that it's a beta 1 project
	// above that and it's beta 2 and beyond
	BYTE ver = DAN_FILEVERSION;
	arc.WriteBytes(&ver, sizeof(BYTE));


	// Store resources first
	// Textures, then composites, then sounds
	// Unlike the old save system, we don't need to
	// write out the amount of each resource; we can
	// just store a byte representing what type
	// of data it is
	TextureManager *texman = Locator::GetTextureManager();
	if(!texman->texmap.empty() && toSave & SAVEDATA_TEXTURES)
	{
		for(auto it = texman->texmap.begin(); it != texman->texmap.end(); ++it)
		{
			if(it->second.ptr->getSize().x <= 0 || it->second.ptr->getSize().y <= 0)
				continue;

			// Don't need to save the default empty texture
			if(it->first == "TNT1A0" || it->first == "*No Sprite(TNT1A0)" || it->first == "* No Sprite(TNT1A0)")
				continue;

			// Write byte indicating a Texture
			if(b_embedSprites || forceEmbed)
				arc << BYTE(DTYPE_TEXTURE);
			else
				arc << BYTE(DTYPE_TEXTUREDIR);

			// Since we mangle the names a bit, we need to save the map key
			arc << it->first;

			// if embedding, just write the texture, otherwise we need the directory path too
			if(!b_embedSprites && !forceEmbed)
			{
				arc << it->second.file_dir;
				arc << it->second.file_name;
			}
			else
				arc << *it->second.ptr;

			// wow that was insanely easy...
		}
	}
	if(!texman->compmap.empty() && toSave & SAVEDATA_COMPOSITES)
	{
		for(auto itx = texman->compmap.begin(); itx != texman->compmap.end(); ++itx)
		{
			if(itx->first == "TNT1A0")
				continue;

			arc << BYTE(DTYPE_COMPOSITE);

			// Since it's not a library type like sf::Texture is, we can make use of the serialization class
			arc << itx->first;
			itx->second->Serialize(arc);
		}
	}

	SoundManager *sndman = Locator::GetSoundManager();
	if(!sndman->sndmap.empty() && toSave & SAVEDATA_SOUNDS)
	{
		for(auto itz = sndman->sndmap.begin(); itz != sndman->sndmap.end(); ++itz)
		{
			// Write byte indicating a Sound
			if(b_embedSounds || forceEmbed)
				arc << BYTE(DTYPE_SOUND);
			else
				arc << BYTE(DTYPE_SOUNDDIR);

			// Since we mangle the names a bit, we need to save the map key
			arc << itz->first;

			// if embedding, just write the sound data, otherwise we need the directory path too
			if(!b_embedSounds && !forceEmbed)
			{
				arc << itz->second.file_dir;
				arc << itz->second.file_name;
			}
			else
				arc << *itz->second.ptr;
		}
	}

	// Now write the states
	std::map<std::string, FState_t> &outStates = animator.m_validStates;
	if(!outStates.empty() && toSave & SAVEDATA_STATES)
	{
		for(auto itg = outStates.begin(); itg != outStates.end(); ++itg)
		{
			arc << BYTE(DTYPE_STATE);
			arc << itg->second;
		}
	}

	arc.Close();
}

void DanFrame::LoadProject(const wxString &fileName)
{
	File arc;
	arc.Create(fileName.ToStdString(), FM_READ_BINARY);

	BYTE fv;
	arc >> fv;

	// Load old projects if version <= 1 (embedding flag indicates this from the old project files)
	if(fv <= 1)
	{
		arc.Close();
		LoadProjectLegacy(fileName);
		return;
	}

	arc.SetVersion(fv);

	// Declare some types
	sf::Texture texture;
	sf::SoundBuffer sound;
	std::string key;
	std::string fname;
	std::string fpath;
	FState_t state;
	CompositeTexture composite;
	int width;
	int height;
	int sndsz;
	long index = 0;

	while(!arc.IsEOF())
	{
		BYTE dataType = 0;
		arc >> dataType;

		if(arc.IsEOF())
			break;

		switch(dataType)
		{
		case DTYPE_TEXTURE:
			arc >> key;
			arc >> texture;
			if(Locator::GetTextureManager()->texmap.find(key) == Locator::GetTextureManager()->texmap.end())
			{
				Locator::GetTextureManager()->Acquire(texture, "embedded/textureData.file");
				Locator::GetTextureManager()->Remap(std::string("textureData.file"), std::string(key));
				index = SpritesListCtrl->InsertItem(SpritesListCtrl->GetItemCount(), std::string(key));
				SpritesListCtrl->SetItemData(index, CreateSortData(key, ComputeStringHash(key)));
				SpritesListCtrl->SetItem(index, COL_SPRITES_SOURCE, "Embedded");
				width = Locator::GetTextureManager()->GetTexture(key)->getSize().x;
				height = Locator::GetTextureManager()->GetTexture(key)->getSize().y;
				SpritesListCtrl->SetItem(index, COL_SPRITES_SIZE, std::to_string(int(4 * width * height)));
				SpritesListCtrl->SetItem(index, COL_SPRITES_DIMS, std::to_string(width) + "x" + std::to_string(height));
			}
			break;
		case DTYPE_TEXTUREDIR:
			arc >> key >> fpath >> fname;
			if(Locator::GetTextureManager()->texmap.find(key) == Locator::GetTextureManager()->texmap.end())
			{
				if(Locator::GetTextureManager()->Precache(fpath + fname))
				{
					Locator::GetTextureManager()->Remap(fpath + fname, std::string(key));
					index = SpritesListCtrl->InsertItem(SpritesListCtrl->GetItemCount(), std::string(key));
					SpritesListCtrl->SetItemData(index, CreateSortData(key, ComputeStringHash(key)));
					SpritesListCtrl->SetItem(index, COL_SPRITES_SOURCE, fpath + fname);
					width = Locator::GetTextureManager()->GetTexture(key)->getSize().x;
					height = Locator::GetTextureManager()->GetTexture(key)->getSize().y;
					SpritesListCtrl->SetItem(index, COL_SPRITES_SIZE, std::to_string(int(4 * width * height)));
					SpritesListCtrl->SetItem(index, COL_SPRITES_DIMS, std::to_string(width) + "x" + std::to_string(height));
				}
			}
			break;
		case DTYPE_COMPOSITE:
			arc >> key;
			composite.Deserialize(arc);
			if(Locator::GetTextureManager()->compmap.find(key) == Locator::GetTextureManager()->compmap.end())
			{
				Locator::GetTextureManager()->compmap.insert(std::pair<std::string, std::shared_ptr<CompositeTexture>>(key, std::shared_ptr<CompositeTexture>(new CompositeTexture(composite))));
				index = SpritesListCtrl->InsertItem(SpritesListCtrl->GetItemCount(), std::string(key));
				SpritesListCtrl->SetItemData(index, CreateSortData(key, ComputeStringHash(key) + 500000));
				SpritesListCtrl->SetItem(index, COL_SPRITES_SOURCE, "TEXTURES");
				width = Locator::GetTextureManager()->GetCompositeTexture(key)->m_dims.x;
				height = Locator::GetTextureManager()->GetCompositeTexture(key)->m_dims.y;
				//SpritesListCtrl->SetItem(index, COL_SPRITES_SIZE, std::to_string(int(4 * width * height)));
				SpritesListCtrl->SetItem(index, COL_SPRITES_DIMS, std::to_string(width) + "x" + std::to_string(height));
			}
			break;
		case DTYPE_SOUND:
			arc >> key;
			arc >> sound;
			if(Locator::GetSoundManager()->sndmap.find(key) == Locator::GetSoundManager()->sndmap.end())
			{
				Locator::GetSoundManager()->Acquire(sound, "embedded/soundData.file");
				Locator::GetSoundManager()->Remap(std::string("soundData.file"), std::string(key));
				index = SoundsListCtrl->InsertItem(SoundsListCtrl->GetItemCount(), std::string(key));
				SoundsListCtrl->SetItemData(index, CreateSortData(key, ComputeStringHash(key)));
				SoundsListCtrl->SetItem(index, COL_SOUNDS_SOURCE, "Embedded");
				sndsz = Locator::GetSoundManager()->GetSound(key)->getSampleCount() * sizeof(sf::Int16);
				SoundsListCtrl->SetItem(index, COL_SOUNDS_SIZE, std::to_string(sndsz));
			}
			break;
		case DTYPE_SOUNDDIR:
			arc >> key >> fpath >> fname;
			if(Locator::GetSoundManager()->sndmap.find(key) == Locator::GetSoundManager()->sndmap.end())
			{
				if(Locator::GetSoundManager()->Precache(fpath + fname))
				{
					Locator::GetSoundManager()->Remap(fpath + fname, std::string(key));
					index = SoundsListCtrl->InsertItem(SoundsListCtrl->GetItemCount(), std::string(key));
					SoundsListCtrl->SetItemData(index, CreateSortData(key, ComputeStringHash(key)));
					SoundsListCtrl->SetItem(index, COL_SOUNDS_SOURCE, fpath + fname);
					sndsz = Locator::GetSoundManager()->GetSound(key)->getSampleCount() * sizeof(sf::Int16);
					SoundsListCtrl->SetItem(index, COL_SOUNDS_SIZE, std::to_string(sndsz));
				}
			}
			break;
		case DTYPE_STATE:
			arc >> state;
			animator.m_validStates.insert(std::pair<std::string, FState_t>(state.name, state));
			StateListCtrl->DanPushBack(wxString(state.name)); // No safety check. #YOLO
			break;
		case DTYPE_EMPTY:
		default:
			break;
		}

		key = "";
		fname = "";
		fpath = "";
		texture = sf::Texture();
		sound = sf::SoundBuffer();
		composite = CompositeTexture();
		state = FState_t();
		width = 0;
		height = 0;
		sndsz = 0;
	}

	// Final steps: compile composite textures, then ensure sprite pointers are valid
	for(auto ct = Locator::GetTextureManager()->compmap.begin(); ct != Locator::GetTextureManager()->compmap.end(); ++ct)
	{
		ct->second->CreateComposite(true);
	}

	animator.EnsureSpritePointers();


	SpritesListCtrl->SortItems(ListStringComparison, 0);
	SoundsListCtrl->SortItems(ListStringComparison, 0);

	arc.Close();
}