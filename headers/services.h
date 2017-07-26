#pragma once

class Audio;
class ResourceManager;
class TextureManager;
class ShaderManager;
class FontManager;
class SoundManager;
class ActionManager;

class Services
{
public:
	static void Init();
	static Audio *GetAudioSystem();
	static TextureManager *GetTextureManager();
	static ShaderManager *GetShaderManager();
	static FontManager *GetFontManager();
	static SoundManager *GetSoundManager();
	static ActionManager *GetActionManager();
	
	static void Provide(Audio *service);
	static void Provide(TextureManager *service);
	static void Provide(ShaderManager *service);
	static void Provide(FontManager *service);
	static void Provide(SoundManager *service);
	static void Provide(ActionManager *service);

private:
	static Audio *audioService;
	static TextureManager *texService;
	static ShaderManager *shaderService;
	static FontManager *fontService;
	static SoundManager *soundService;
	static ActionManager *actionService;
};