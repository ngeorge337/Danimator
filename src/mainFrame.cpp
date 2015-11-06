#include "libs.h"
#include "animator.h"
#include "mainFrame.h"
#include "codewindow.h"
#include "preferences.h"


DanFrame::DanFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size), playingMode(false), playFrame(0), saved(true), projectName(""), inStartup(true)
{
	BuildMenuBar();
	BuildStatusBar();

	// Add the panel so we can "mount" controls to it
	DanPanel = new wxPanel(this);
	DanSizer = new wxFlexGridSizer(0, 3, 0, 2);

	BuildStatesList();
	BuildResourceLists();
	BuildCanvasControls();
	
	// Finalize Sizers
	DanSizer->Add(stateSizer, 1, wxEXPAND);
	DanSizer->Add(ResourcesNotebook, 1, wxEXPAND);
	DanSizer->Add(renderSizer, 1, wxEXPAND);
	DanSizer->AddGrowableCol(2, 1);
	DanSizer->AddGrowableRow(0, 1);
	DanPanel->SetSizerAndFit(DanSizer);

	SetStartupMode(true);

	CenterOnScreen();
}

void DanFrame::SetControls(bool controls)
{
	//StateListCtrl->Enable(controls);
	newStateButton->Enable(controls);
	delStateButton->Enable(controls);
	SpritesListCtrl->Enable(controls);
	loadSpriteButton->Enable(controls);
	delSpriteButton->Enable(controls);
	SoundsListCtrl->Enable(controls);
	loadSoundButton->Enable(controls);
	delSoundButton->Enable(controls);
	xSpin->Enable(controls);
	ySpin->Enable(controls);
	currentSpriteCtrl->Enable(controls);
	currentSoundCtrl->Enable(controls);
	prevFrameButton->Enable(controls);
	nextFrameButton->Enable(controls);
	playFromStartButton->Enable(controls);
	addFrameButton->Enable(controls);
	delFrameButton->Enable(controls);
	durationSpin->Enable(controls);
	endChoice->Enable(controls);
	menuFile->Enable(ID_NEWPROJECT, controls);
	menuFile->Enable(ID_OPENPROJECT, controls);
}

void DanFrame::SetStatelessControls(bool controls)
{
	//StateListCtrl->Enable(controls);
	newStateButton->Enable(controls);
	delStateButton->Enable(controls);
	SpritesListCtrl->Enable(controls);
	loadSpriteButton->Enable(controls);
	delSpriteButton->Enable(controls);
	SoundsListCtrl->Enable(controls);
	loadSoundButton->Enable(controls);
	delSoundButton->Enable(controls);
	xSpin->Enable(controls);
	ySpin->Enable(controls);
	currentSpriteCtrl->Enable(controls);
	currentSoundCtrl->Enable(controls);
	prevFrameButton->Enable(controls);
	nextFrameButton->Enable(controls);
	playFromStartButton->Enable(controls);
	playButton->Enable(controls);
	addFrameButton->Enable(controls);
	delFrameButton->Enable(controls);
	durationSpin->Enable(controls);
	endChoice->Enable(controls);
	timelineSlider->Enable(controls);
}

void DanFrame::SetStartupMode(bool set)
{
	bool controlMode = !set; // Going into startup mode = controls off (false)
	//StateListCtrl->Enable(set);
	//newStateButton->Enable(set);
	delStateButton->Enable(controlMode);
	SpritesListCtrl->Enable(controlMode);
	loadSpriteButton->Enable(controlMode);
	delSpriteButton->Enable(controlMode);
	SoundsListCtrl->Enable(controlMode);
	loadSoundButton->Enable(controlMode);
	delSoundButton->Enable(controlMode);
	xSpin->Enable(controlMode);
	ySpin->Enable(controlMode);
	if(controlMode)
		EnableTimelineIfValid();
	else
		timelineSlider->Disable();
	currentSpriteCtrl->Enable(controlMode);
	currentSoundCtrl->Enable(controlMode);
	prevFrameButton->Enable(controlMode);
	nextFrameButton->Enable(controlMode);
	playButton->Enable(controlMode);
	playFromStartButton->Enable(controlMode);
	loopCheckBox->Enable(controlMode);
	addFrameButton->Enable(controlMode);
	delFrameButton->Enable(controlMode);
	durationSpin->Enable(controlMode);
	endChoice->Enable(controlMode);
	viewCodeButton->Enable(controlMode);
	exportCodeButton->Enable(controlMode);

	menuFile->Enable(ID_SAVEPROJECT, controlMode);
	menuFile->Enable(ID_SAVEPROJECTAS, controlMode);

	inStartup = set;
}

void DanFrame::SetPlayingMode(bool mode, bool fromStart /*= false*/)
{
	playingMode = mode;
	if(mode)
	{
		playFrame = animator.GetCurrentState()->frameOffset;
		SetControls(false);
		playButton->SetBitmap(wxBITMAP_PNG(StopAnim));
		playButton->SetToolTip(_T("Stop Animation"));
		if(fromStart) animator.GetCurrentState()->Reset(0);
		else animator.GetCurrentState()->Reset(animator.GetCurrentState()->frameOffset);
	}
	else if(!mode)
	{
		Audio::StopAllSounds();
		SetControls(true);
		playButton->SetBitmap(wxBITMAP_PNG(PlayAnim));
		playButton->SetToolTip(_T("Play Animation"));
		animator.GetCurrentState()->frameOffset = playFrame;
		timelineSlider->SetValue(animator.GetCurrentState()->frameOffset + 1);
		xSpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().sprite.getPosition().x);
		ySpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().sprite.getPosition().y);
		currentSpriteCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().spriteName);
		currentSoundCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().soundName);
		durationSpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().tics);
		endChoice->SetSelection(animator.GetCurrentState()->ending);
		if(endChoice->GetSelection() == END_GOTO)
			endChoice->SetString(END_GOTO, "Goto " + animator.GetCurrentState()->gotoLabel);
		else
			endChoice->SetString(END_GOTO, "Goto...");
	}
}

Animator & DanFrame::GetAnimator()
{
	return animator;
}

wxString DanFrame::GetAllDecorateCode()
{
	wxString ret = "";
	for(auto it = animator.m_validStates.begin(); it != animator.m_validStates.end(); ++it)
	{
		ret += GetDecorateCode(it->first);
		ret += "\n";
	}
	return ret;
}

wxString DanFrame::GetDecorateCode(const std::string &stateName)
{
	wxString output = "";
	std::string lastFrame = "";
	FState_t *state;
	//int index = 0;
	if(!animator.IsValidState(stateName))
		return wxString("");
	state = animator.GetState(stateName);
	if(state->m_frames.empty())
		return wxString("");

	// Add the statename
	output += stateName + ":\n";
	for(int i = 0; i < state->m_frames.size(); i++)
	{
		// Add the frames one at a time
		// State Frame Duration Offset(x, y) A_PlaySound(SoundName, CHAN_AUTO)
		// Flow [label]
		// SHTG A 4 Offset(92, 140) A_PlaySound("Gun/Pump", CHAN_AUTO)
		// Goto Ready
		output += "\t";
		output += animator.GetState(stateName)->GetFrame(i).spriteName.substr(0, 4) + " ";
		output += animator.GetState(stateName)->GetFrame(i).spriteName.substr(4, 1) + " ";
		output += std::to_string(animator.GetState(stateName)->GetFrame(i).tics);
		if(animator.GetState(stateName)->GetFrame(i).sprite.getPosition().x != 0 || animator.GetState(stateName)->GetFrame(i).sprite.getPosition().y != 0)
			output += " Offset(" + std::to_string(int(animator.GetState(stateName)->GetFrame(i).sprite.getPosition().x)) + ", " + std::to_string(int(animator.GetState(stateName)->GetFrame(i).sprite.getPosition().y)) + ")";
		if(animator.GetState(stateName)->GetFrame(i).hasSound)
			output += " A_PlaySound(\"" + animator.GetState(stateName)->GetFrame(i).soundName + "\", CHAN_AUTO)";
		output += "\n";
	}
	if(animator.GetState(stateName)->ending != END_NONE)
	{
		output += "\t";
		switch(animator.GetState(stateName)->ending)
		{
		case END_LOOP: output += "Loop"; break;
		case END_WAIT: output += "Wait"; break;
		case END_STOP: output += "Stop"; break;
		case END_GOTO: output += "Goto " + animator.GetState(stateName)->gotoLabel; break;
		default:
			break;
		}
	}

	return output;
}

void DanFrame::EnableTimelineIfValid()
{
	if(animator.GetCurrentState() == nullptr)
	{
		timelineSlider->Disable();
		return;
	}

	if(animator.GetCurrentState()->m_frames.size() >= 2)
		timelineSlider->Enable();
	else
		timelineSlider->Disable();
}

void DanFrame::UpdateSpins()
{
	xSpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().sprite.getPosition().x);
	ySpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().sprite.getPosition().y);
}


void DanFrame::UpdateFrameInfo()
{
	currentSpriteCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().spriteName);
	currentSoundCtrl->SetValue(animator.GetCurrentState()->GetCurrentFrame().soundName);
	durationSpin->SetValue(animator.GetCurrentState()->GetCurrentFrame().tics);
}

void DanFrame::UpdateFlow()
{
	endChoice->SetSelection(animator.GetCurrentState()->ending);

	if(endChoice->GetSelection() == END_GOTO)
		endChoice->SetString(END_GOTO, "Goto " + animator.GetCurrentState()->gotoLabel);
	else
		endChoice->SetString(END_GOTO, "Goto...");
}


void DanFrame::UpdateTimeline()
{
	if(animator.GetCurrentState() != nullptr)
	{
		if(animator.GetCurrentState()->m_frames.size() >= 2)
			timelineSlider->SetMax(animator.GetCurrentState()->m_frames.size());
		else
			timelineSlider->SetMax(2);
	}
	EnableTimelineIfValid();
}

void DanFrame::ResetFrame()
{
	animator.GetCurrentState()->Reset(timelineSlider->GetValue() - 1);
}

void DanFrame::SaveProject(const wxString &fileName)
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
	TextureManager *tm = Locator::GetTextureManager();
	SoundManager *sm = Locator::GetSoundManager();

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

void DanFrame::LoadProject(const wxString &fileName)
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
			Locator::GetTextureManager()->Acquire(texture, texFile);
			Locator::GetTextureManager()->Remap(std::string(StripStringDirectory(texFile)), std::string(texKey));
		}
		else
		{
			texture.loadFromFile(texFile);
			Locator::GetTextureManager()->Precache(texFile);
			Locator::GetTextureManager()->Remap(std::string(texFile), std::string(texKey));
		}
		SpritesListCtrl->Append(std::string(texKey));

		if(texKey)			delete[] texKey;
		if(texFileName)		delete[] texFileName;
		if(texDir)			delete[] texDir;
		if(textureData)		delete[] textureData;
	}

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
			Locator::GetSoundManager()->Acquire(sound, sndFile);
			Locator::GetSoundManager()->Remap(std::string(StripStringDirectory(sndFile)), std::string(sndKey));
		}
		else
		{
			sound.loadFromFile(sndFile);
			Locator::GetSoundManager()->Precache(sndFile);
			Locator::GetSoundManager()->Remap(std::string(sndFile), std::string(sndKey));
		}
		SoundsListCtrl->Append(std::string(sndKey));

		if(sndKey)			delete[] sndKey;
		if(sndFileName)		delete[] sndFileName;
		if(sndDir)			delete[] sndDir;
		if(soundData)		delete[] soundData;
	}

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
		//spr.setTexture(*Locator::GetTextureManager()->GetTexture(spriteName), true);
		//spr.setPosition(frameX, frameY);
		if(!animator.IsValidState(stateLabel))
		{
			animator.CreateState(stateLabel);
			StateListCtrl->Append(stateLabel); // No safety check. #YOLO
		}
		animator.GetState(stateLabel)->ending = flowControl;
		if(flowControl == END_GOTO)
			animator.GetState(stateLabel)->gotoLabel = flowLabel;
		animator.GetState(stateLabel)->AddFrame(Locator::GetTextureManager()->GetTexture(spriteName), frameDuration);
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