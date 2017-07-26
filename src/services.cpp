#include "libs.h"
#include "util.h"
#include "static.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "resrcman.h"
#include "texman.h"
#include "soundman.h"
#include "fontman.h"
#include "shaderman.h"
#include "actionmanager.h"
#include "services.h"


Audio *Services::audioService = nullptr;
TextureManager *Services::texService = nullptr;
ShaderManager *Services::shaderService = nullptr;
FontManager *Services::fontService = nullptr;
SoundManager *Services::soundService = nullptr;
ActionManager *Services::actionService = nullptr;

void Services::Provide(ShaderManager *service)
{
	shaderService = service;
}

void Services::Provide(TextureManager *service)
{
	texService = service;
}

void Services::Provide(FontManager *service)
{
	fontService = service;
}

void Services::Provide(Audio *service)
{
	audioService = service;
}

ShaderManager * Services::GetShaderManager()
{
	return shaderService;
}

TextureManager * Services::GetTextureManager()
{
	return texService;
}

FontManager * Services::GetFontManager()
{
	return fontService;
}

SoundManager * Services::GetSoundManager()
{
	return soundService;
}

void Services::Provide(ActionManager *service)
{
	actionService = service;
}

ActionManager * Services::GetActionManager()
{
	return actionService;
}

void Services::Init()
{
	texService = new TextureManager("./textures");
	texService->AddSearchDirectory("./sprites");
	shaderService = new ShaderManager("./shaders");
	fontService = new FontManager("./fonts");
	soundService = new SoundManager("./sounds");
	audioService = new Audio();
	actionService = new ActionManager();
}

Audio * Services::GetAudioSystem()
{
	return audioService;
}
