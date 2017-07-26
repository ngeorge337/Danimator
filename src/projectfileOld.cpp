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

void DanFrame::SaveProjectLegacy(const wxString &fileName)
{
	wxFFile outFile(fileName, "wb");
	//char *buffer;
	bool embSpr = b_embedSprites;
	bool embSnd = b_embedSounds;
	bool texEmb = false;
	bool sndEmb = false;
	//int empties = 0;
	int len = 0;
	std::size_t sz = 0;
	float flen = 0.f;
	bool flag = false;
	int imgW = 0;
	int imgH = 0;
	TextureManager *tm = Services::GetTextureManager();
	SoundManager *sm = Services::GetSoundManager();

	// Embedding flags
	if(embSpr && tm->texmap.size() > 0)
		texEmb = true;
	outFile.Write(&texEmb, sizeof(BYTE));
	if(embSnd && sm->sndmap.size() > 0)
		sndEmb = true;
	outFile.Write(&sndEmb, sizeof(BYTE));
	// Texture Count
	len = tm->texmap.size();
	if(tm->texmap.find("TNT1A0") != tm->texmap.end())
		len--;
	outFile.Write(&len, sizeof(int));

	// Write out each texture ------------
	for(auto it = tm->texmap.begin(); it != tm->texmap.end(); ++it)
	{
		if(it->first == "TNT1A0")
			continue;

		// Key length
		len = it->first.size();
		outFile.Write(&len, sizeof(int));
		// File Name Length
		len = it->second.file_name.length();
		outFile.Write(&len, sizeof(int));
		// Dir length
		len = it->second.file_dir.length();
		outFile.Write(&len, sizeof(int));
		// Embedding? Write data length too
		if(embSpr)
		{
			len = it->second.ptr->getSize().x * it->second.ptr->getSize().y * 4;
			outFile.Write(&len, sizeof(int));
		}
		// Write key
		len = it->first.length();
		outFile.Write(it->first.c_str(), sizeof(char) * len);
		// Write File name
		len = it->second.file_name.length();
		outFile.Write(it->second.file_name.c_str(), sizeof(char) * len);
		// Write Dir
		len = it->second.file_dir.length();
		outFile.Write(it->second.file_dir.c_str(), sizeof(char) * len);
		// Write image data if embedding
		if(embSpr)
		{
			sf::Image tempimg = sf::Image();
			tempimg = it->second.ptr->copyToImage();
			imgW = it->second.ptr->getSize().x;
			imgH = it->second.ptr->getSize().y;
			const sf::Uint8 *imgData = tempimg.getPixelsPtr();
			len = it->second.ptr->getSize().x * it->second.ptr->getSize().y * 4;
			outFile.Write(&imgW, sizeof(int));
			outFile.Write(&imgH, sizeof(int));
			outFile.Write(imgData, sizeof(sf::Uint8) * len);
		}
	}

	// Sound Count
	len = sm->sndmap.size();
	outFile.Write(&len, sizeof(int));
	// Write out each sound ------------
	for(auto itx = sm->sndmap.begin(); itx != sm->sndmap.end(); ++itx)
	{
		// Key length
		len = itx->first.size();
		outFile.Write(&len, sizeof(int));
		// File Name Length
		len = itx->second.file_name.length();
		outFile.Write(&len, sizeof(int));
		// Dir length
		len = itx->second.file_dir.length();
		outFile.Write(&len, sizeof(int));
		// Write key
		len = itx->first.length();
		outFile.Write(itx->first.c_str(), sizeof(char) * len);
		// Write File name
		len = itx->second.file_name.length();
		outFile.Write(itx->second.file_name.c_str(), sizeof(char) * len);
		// Write Dir
		len = itx->second.file_dir.length();
		outFile.Write(itx->second.file_dir.c_str(), sizeof(char) * len);
		// Write sound data if embedding
		if(embSnd)
		{
			sz = sizeof(sf::Int16) * itx->second.ptr->getSampleCount();
			outFile.Write(&sz, sizeof(std::size_t));
			len = itx->second.ptr->getChannelCount();
			outFile.Write(&len, sizeof(int));
			len = itx->second.ptr->getSampleRate();
			outFile.Write(&len, sizeof(int));
			len = itx->second.ptr->getSampleCount();
			outFile.Write(&len, sizeof(int));
			const sf::Int16 *samples = itx->second.ptr->getSamples();
			outFile.Write(samples, sz);
		}
	}

	len = 0;
	flen = 0.f;
	// Write frame information
	// Get total frame count and write that in
	for(auto itg = animator.m_validStates.begin(); itg != animator.m_validStates.end(); ++itg)
	{
		len += itg->second.m_frames.size();
	}
	outFile.Write(&len, sizeof(int));
	for(auto itz = animator.m_validStates.begin(); itz != animator.m_validStates.end(); ++itz)
	{
		for(int x = 0; x < itz->second.m_frames.size(); x++)
		{
			// Write the length of state label for these frames
			len = itz->first.length();
			outFile.Write(&len, sizeof(int));
			// Write Flow control type
			len = itz->second.ending;
			outFile.Write(&len, sizeof(BYTE));
			// Write length of goto label (if applicable)
			if(itz->second.ending == END_GOTO)
			{
				len = itz->second.gotoLabel.length();
				outFile.Write(&len, sizeof(int));
			}
			// Write duration
			flen = itz->second.m_frames[x].duration;
			outFile.Write(&flen, sizeof(float));
			// Write tics
			len = itz->second.m_frames[x].tics;
			outFile.Write(&len, sizeof(int));
			// Write X and Y offsets
			flen = itz->second.m_frames[x].sprite.getPosition().x;
			outFile.Write(&flen, sizeof(float));
			flen = itz->second.m_frames[x].sprite.getPosition().y;
			outFile.Write(&flen, sizeof(float));
			// Write length of sprite name
			len = itz->second.m_frames[x].spriteName.length();
			outFile.Write(&len, sizeof(int));
			// Check sound flagging
			flag = itz->second.m_frames[x].hasSound;
			outFile.Write(&flag, sizeof(BYTE));
			if(flag)
			{
				len = itz->second.m_frames[x].soundName.length();
				outFile.Write(&len, sizeof(int));
			}
			// Write goto label
			if(itz->second.ending == END_GOTO)
			{
				len = itz->second.gotoLabel.length();
				outFile.Write(itz->second.gotoLabel.c_str(), len);
			}
			// Write the state label
			len = itz->first.length();
			outFile.Write(itz->first.c_str(), len);
			// Write the sprite name
			len = itz->second.m_frames[x].spriteName.length();
			outFile.Write(itz->second.m_frames[x].spriteName.c_str(), len);
			// Write the sound name if appropriate
			if(flag)
			{
				len = itz->second.m_frames[x].soundName.length();
				outFile.Write(itz->second.m_frames[x].soundName.c_str(), len);
			}
		}
	}
}

void DanFrame::LoadProjectLegacy(const wxString &fileName)
{
	wxFFile inFile(fileName, "rb");

	//
	// FILE FORMAT
	// 
	// What has to be saved:
	// State names
	// Texture Names/File locations
	// - if embedding, write image binary to file, load from mem
	// Sound Names/File locations
	// - same deal as images, should read soundbuffer data to embed it
	// Animation Frames
	// - sf::Sprite pointer will be invalidated, need to reload
	// - will have to reapply transforms to Sprite after loading image
	// - duration, tics, and hasSound read/applied separate
	// - sound name and sprite name must be read in
	//
	// 1 byte	- embedded flag
	// 4 bytes	- count of Texture files
	// 4 bytes	- length of Texture name (in std::map key)
	// 4 bytes	- length of Texture directory string
	// 4 bytes	- length of Texture file name
	// ? bytes	- file name string
	// ? bytes	- directory string
	// if(embedded flag)
	//		? bytes		- texture data (sf::Texture::loadFromMem(), TextureManager::Acquire(file name + directory))
	// else
	//		? bytes		- TextureManager::Precache(file name + directory)
	// TextureManager::Remap() to keyname
	//
	// 4 bytes	- count of Sound files
	// 4 bytes	- length of Sound name (in std::map key)
	// 4 bytes	- length of Sound directory string
	// 4 bytes	- length of Sound file name
	// ? bytes	- file name string
	// ? bytes	- directory string
	// if(embedded flag)
	//		? bytes		- Sound data (sf::SoundBuffer::loadFromMem(), SoundManager::Acquire(file name + directory))
	// else
	//		? bytes		- SoundeManager::Precache(file name + directory)
	// SoundManager::Remap() to keyname
	//
	// 4 bytes	- count of Animation Frames
	// 4 bytes	- length of state label name
	// 4 bytes	- duration (float)
	// 4 bytes	- tics (int)
	// 4 bytes	- X offset (float)
	// 4 bytes	- Y offset (float)
	// 1 byte	- hasSound flag
	// 4 bytes	- length of Sprite name string
	// if(hasSound flag)
	//		4 bytes		- length of Sound name string
	// ? bytes	- state label string
	// ? bytes	- Sprite name string
	// if(hasSound flag)
	//		? bytes		- Sound name string
	// frame.sprite = GetTexture(sprite name)
	// frame.offsets = X, Y

	// Holy crap this thing

	// Load Textures
	long index = 0;

	bool hasTextureEmbed = false;
	bool hasSoundEmbed = false;
	int textureCount = 0;
	int texKeyLen = 0;
	int texFileNameLen = 0;
	int texDirLen = 0;
	int texDataLen = 0;
	int imageWidth = 0;
	int imageHeight = 0;
	char *texKey = nullptr;
	char *texFileName = nullptr;
	char *texDir = nullptr;
	sf::Uint8 *textureData = nullptr;
	inFile.Read(&hasTextureEmbed, sizeof(BYTE));
	inFile.Read(&hasSoundEmbed, sizeof(BYTE));
	inFile.Read(&textureCount, sizeof(int));
	for(int i = 0; i < textureCount; i++)
	{
		inFile.Read(&texKeyLen, sizeof(int));
		inFile.Read(&texFileNameLen, sizeof(int));
		inFile.Read(&texDirLen, sizeof(int));
		if(hasTextureEmbed)
			inFile.Read(&texDataLen, sizeof(int));

		// Read Key
		texKey = new char[texKeyLen + 1];
		memset(texKey, 0, texKeyLen);
		inFile.Read(texKey, sizeof(char) * texKeyLen);
		texKey[texKeyLen] = '\0';

		// Read File name
		texFileName = new char[texFileNameLen + 1];
		memset(texFileName, 0, texFileNameLen);
		inFile.Read(texFileName, sizeof(char) * texFileNameLen);
		texFileName[texFileNameLen] = '\0';

		// Read Directory
		texDir = new char[texDirLen + 1];
		memset(texDir, 0, texDirLen);
		inFile.Read(texDir, sizeof(char) * texDirLen);
		texDir[texDirLen] = '\0';

		// Load texture (either from data, or from disk)
		sf::Texture texture = sf::Texture();
		std::string texFile = "";
		texFile += texDir;
		texFile += texFileName;
		if(hasTextureEmbed)
		{
			textureData = new sf::Uint8[texDataLen + 1];
			memset(textureData, 0, texDataLen);
			inFile.Read(&imageWidth, sizeof(int));
			inFile.Read(&imageHeight, sizeof(int));
			inFile.Read(textureData, texDataLen);
			textureData[texDataLen] = '\0';
			sf::Image image = sf::Image();
			image.create(imageWidth, imageHeight, textureData);
			texture.loadFromImage(image);
			Services::GetTextureManager()->Acquire(texture, texFile);
			Services::GetTextureManager()->Remap(std::string(StripStringDirectory(texFile)), std::string(texKey));
		}
		else
		{
			texture.loadFromFile(texFile);
			Services::GetTextureManager()->Precache(texFile);
			Services::GetTextureManager()->Remap(std::string(texFile), std::string(texKey));
		}
		index = SpritesListCtrl->InsertItem(SpritesListCtrl->GetItemCount(), std::string(texKey));
		SpritesListCtrl->SetItemData(index, CreateSortData(texKey, ComputeStringHash(SpritesListCtrl->GetItemText(index).ToStdString())));
		SpritesListCtrl->SetItem(index, COL_SPRITES_SOURCE, hasTextureEmbed ? "Embedded" : texFile);
		int width = Services::GetTextureManager()->GetTexture(texKey)->getSize().x;
		int height = Services::GetTextureManager()->GetTexture(texKey)->getSize().y;
		SpritesListCtrl->SetItem(index, COL_SPRITES_SIZE, std::to_string(int(4 * width * height)));
		SpritesListCtrl->SetItem(index, COL_SPRITES_DIMS, std::to_string(width) + "x" + std::to_string(height));

		if(texKey)			delete[] texKey;
		if(texFileName)		delete[] texFileName;
		if(texDir)			delete[] texDir;
		if(textureData)		delete[] textureData;
	}
	SpritesListCtrl->SortItems(ListStringComparison, 0);

	// Load Sounds
	int soundCount = 0;
	int sndKeyLen = 0;
	int sndFileNameLen = 0;
	int sndDirLen = 0;
	std::size_t sndDataLen = 0;
	int sndSampleCount = 0;
	int sndSampleRate = 0;
	int sndChannelCount = 0;
	char *sndKey = nullptr;
	char *sndFileName = nullptr;
	char *sndDir = nullptr;
	sf::Int16 *soundData = nullptr;
	inFile.Read(&soundCount, sizeof(int));
	for(int i = 0; i < soundCount; i++)
	{
		inFile.Read(&sndKeyLen, sizeof(int));
		inFile.Read(&sndFileNameLen, sizeof(int));
		inFile.Read(&sndDirLen, sizeof(int));

		// Read Key
		sndKey = new char[sndKeyLen + 1];
		memset(sndKey, 0, sndKeyLen);
		inFile.Read(sndKey, sizeof(char) * sndKeyLen);
		sndKey[sndKeyLen] = '\0';

		// Read File name
		sndFileName = new char[sndFileNameLen + 1];
		memset(sndFileName, 0, sndFileNameLen);
		inFile.Read(sndFileName, sizeof(char) * sndFileNameLen);
		sndFileName[sndFileNameLen] = '\0';

		// Read Directory
		sndDir = new char[sndDirLen + 1];
		memset(sndDir, 0, sndDirLen);
		inFile.Read(sndDir, sizeof(char) * sndDirLen);
		sndDir[sndDirLen] = '\0';

		// Load sound (either from data, or from disk)
		sf::SoundBuffer sound = sf::SoundBuffer();
		std::string sndFile = "";
		sndFile += sndDir;
		sndFile += sndFileName;
		if(hasSoundEmbed)
		{
			inFile.Read(&sndDataLen, sizeof(std::size_t));
			inFile.Read(&sndChannelCount, sizeof(int));
			inFile.Read(&sndSampleRate, sizeof(int));
			inFile.Read(&sndSampleCount, sizeof(int));
			soundData = new sf::Int16[sndDataLen];
			memset(soundData, 0, sndDataLen);
			//soundData[sndDataLen] = '\0';
			inFile.Read(soundData, sndDataLen);
			sound.loadFromSamples(soundData, sndSampleCount, sndChannelCount, sndSampleRate);
			Services::GetSoundManager()->Acquire(sound, sndFile);
			Services::GetSoundManager()->Remap(std::string(StripStringDirectory(sndFile)), std::string(sndKey));
		}
		else
		{
			sound.loadFromFile(sndFile);
			Services::GetSoundManager()->Precache(sndFile);
			Services::GetSoundManager()->Remap(std::string(sndFile), std::string(sndKey));
		}
		index = SoundsListCtrl->InsertItem(SoundsListCtrl->GetItemCount(), std::string(sndKey));
		SoundsListCtrl->SetItemData(index, CreateSortData(sndKey, ComputeStringHash(SoundsListCtrl->GetItemText(index).ToStdString())));
		SoundsListCtrl->SetItem(index, COL_SOUNDS_SOURCE, hasSoundEmbed ? "Embedded" : sndFile);
		int sndsz = Services::GetSoundManager()->GetSound(sndKey)->getSampleCount() * sizeof(sf::Int16);
		SoundsListCtrl->SetItem(index, COL_SOUNDS_SIZE, std::to_string(sndsz));

		if(sndKey)			delete[] sndKey;
		if(sndFileName)		delete[] sndFileName;
		if(sndDir)			delete[] sndDir;
		if(soundData)		delete[] soundData;
	}
	SoundsListCtrl->SortItems(ListStringComparison, 0);

	// Load States and Animation Frames
	int frameCount = 0;
	int stateLabelLen = 0;
	BYTE flowControl = END_NONE;
	int flowLabelLen = 0;
	float frameDuration = 0;
	int frameTics = 0;
	float frameX = 0.f;
	float frameY = 0.f;
	int spriteNameLen = 0;
	bool hasSoundFlag = false;
	int soundNameLen = 0;
	char *flowLabel = nullptr;
	char *stateLabel = nullptr;
	char *spriteName = nullptr;
	char *soundName = nullptr;
	inFile.Read(&frameCount, sizeof(int));
	for(int i = 0; i < frameCount; i++)
	{
		inFile.Read(&stateLabelLen, sizeof(int));
		inFile.Read(&flowControl, sizeof(BYTE));
		if(flowControl == END_GOTO)
			inFile.Read(&flowLabelLen, sizeof(int));
		inFile.Read(&frameDuration, sizeof(float));
		inFile.Read(&frameTics, sizeof(int));
		inFile.Read(&frameX, sizeof(float));
		inFile.Read(&frameY, sizeof(float));
		inFile.Read(&spriteNameLen, sizeof(int));
		inFile.Read(&hasSoundFlag, sizeof(BYTE));

		if(hasSoundFlag)
			inFile.Read(&soundNameLen, sizeof(int));

		if(flowControl == END_GOTO)
		{
			flowLabel = new char[flowLabelLen + 1];
			memset(flowLabel, 0, flowLabelLen);
			inFile.Read(flowLabel, sizeof(char) * flowLabelLen);
			flowLabel[flowLabelLen] = '\0';
		}

		stateLabel = new char[stateLabelLen + 1];
		memset(stateLabel, 0, stateLabelLen);
		inFile.Read(stateLabel, sizeof(char) * stateLabelLen);
		stateLabel[stateLabelLen] = '\0';

		spriteName = new char[spriteNameLen + 1];
		memset(spriteName, 0, spriteNameLen);
		inFile.Read(spriteName, sizeof(char) * spriteNameLen);
		spriteName[spriteNameLen] = '\0';

		if(hasSoundFlag)
		{
			soundName = new char[soundNameLen + 1];
			memset(soundName, 0, soundNameLen);
			inFile.Read(soundName, sizeof(char) * soundNameLen);
			soundName[soundNameLen] = '\0';
		}

		//sf::Sprite spr = sf::Sprite();
		//spr.setTexture(*Services::GetTextureManager()->GetTexture(spriteName), true);
		//spr.setPosition(frameX, frameY);
		if(!animator.IsValidState(stateLabel))
		{
			animator.CreateState(stateLabel);
			StateListCtrl->DanPushBack(wxString(stateLabel)); // No safety check. #YOLO
		}
		animator.GetState(stateLabel)->ending = flowControl;
		if(flowControl == END_GOTO)
			animator.GetState(stateLabel)->gotoLabel = flowLabel;
		animator.GetState(stateLabel)->AddFrame(Services::GetTextureManager()->GetTexture(spriteName), frameDuration);
		animator.GetState(stateLabel)->m_frames.back().sprite.setPosition(frameX, frameY);
		animator.GetState(stateLabel)->m_frames.back().duration = frameDuration;
		animator.GetState(stateLabel)->m_frames.back().tics = frameTics;
		animator.GetState(stateLabel)->m_frames.back().spriteName = std::string(spriteName);
		animator.GetState(stateLabel)->m_frames.back().hasSound = hasSoundFlag;
		if(hasSoundFlag)
			animator.GetState(stateLabel)->m_frames.back().soundName = std::string(soundName);
		else
			animator.GetState(stateLabel)->m_frames.back().soundName = "";
	}
}