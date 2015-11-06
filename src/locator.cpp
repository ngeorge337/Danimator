#include "libs.h"
#include "util.h"
#include "static.h"
#include "resrcman.h"
#include "texman.h"
#include "soundman.h"
#include "locator.h"

TextureManager *Locator::texService = nullptr;
SoundManager *Locator::soundService = nullptr;


void Locator::Provide( TextureManager *service )
{
	texService = service;
}

TextureManager * Locator::GetTextureManager()
{
	return texService;
}

SoundManager * Locator::GetSoundManager()
{
	return soundService;
}

void Locator::Init()
{
	texService = new TextureManager("./");
	soundService = new SoundManager("./");
}
