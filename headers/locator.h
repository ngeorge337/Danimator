#pragma once

class ResourceManager;
class TextureManager;
class SoundManager;
class ActionManager;

class Locator
{
public:
	static void Init();
	static TextureManager *GetTextureManager();
	static SoundManager *GetSoundManager();
	static ActionManager *GetActionManager();

	static void Provide(TextureManager *service);
	static void Provide(SoundManager *service);
	static void Provide(ActionManager *service);

private:
	static TextureManager *texService;
	static SoundManager *soundService;
	static ActionManager *actionService;
};