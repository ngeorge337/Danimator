#pragma once

enum
{
	CHAN_AUTO		= -1,
	CHAN_BODY,
	CHAN_VOICE,
	CHAN_ATTACK,
	CHAN_ENV,
	CHAN_EXTRA1,
	CHAN_EXTRA2,

	DEFAULT_CHANNELS,
};

class Audio
{
public:
	static void Init();

	static sf::Sound &GetSoundSlot(int slot);
	static sf::Music &GetMusic();

	static void ResetSlots();

	static void StopAllSounds();
	static void StopSound(int slot);
	static void PauseSound(int slot);
	static void PlaySound(const std::string &sound, int slot = CHAN_AUTO, bool loop = false);
	static void PlaySoundAt(const std::string &sound, sf::Vector2f pos, int slot = CHAN_AUTO, bool loop = false);
	static void SetSoundVolume(float vol);

	static void SetMusic(const std::string &songFile, bool loop = true);
	static void PlayMusic();
	static void PauseMusic();
	static void StopMusic();
	static void RestartMusic();


private:
	static float m_defVolume;
	static sf::Music m_music;
	static std::vector<sf::Sound> m_channels;
	static std::string m_musicFile;
};