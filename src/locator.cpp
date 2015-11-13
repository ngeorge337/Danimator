#include "libs.h"
#include "util.h"
#include "static.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "resrcman.h"
#include "texman.h"
#include "soundman.h"
#include "actionmanager.h"
#include "locator.h"

TextureManager *Locator::texService = nullptr;
SoundManager *Locator::soundService = nullptr;
ActionManager *Locator::actionService = nullptr;


void Locator::Provide( TextureManager *service )
{
	texService = service;
}

void Locator::Provide(SoundManager *service)
{
	soundService = service;
}

void Locator::Provide(ActionManager *service)
{
	actionService = service;
}

TextureManager * Locator::GetTextureManager()
{
	return texService;
}

SoundManager * Locator::GetSoundManager()
{
	return soundService;
}

ActionManager * Locator::GetActionManager()
{
	return actionService;
}

void Locator::Init()
{
	texService = new TextureManager("./");
	soundService = new SoundManager("./");
	actionService = new ActionManager();
}
