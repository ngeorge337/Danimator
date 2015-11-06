#pragma once

class ResourceManager;
class TextureManager;
class SoundManager;

class Locator
{
public:
	static void Init();
	static TextureManager *GetTextureManager();
	static SoundManager *GetSoundManager();

	static void Provide(TextureManager *service);
	static void Provide(SoundManager *service);

private:
	static TextureManager *texService;
	static SoundManager *soundService;
};