#pragma once

enum
{
	CHAN_AUTO = -1,
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
	void Init();

	sf::Sound &GetSoundSlot(int slot);
	sf::Music &GetMusic();

	void StopAllSounds();
	void StopSound(int slot);
	void PauseSound(int slot);
	void PauseAllSounds();
	void UnpauseAllSounds();
	void PlaySound(const std::string &sound, int slot = CHAN_AUTO, bool loop = false, float pitch = 1.f);
	void PlaySoundAt(const std::string &sound, sf::Vector2f pos, int slot = CHAN_AUTO, bool loop = false);
	void SetSoundVolume(float vol);

	void SetMusic(const std::string &songFile, bool loop = true);
	void PlayMusic();
	void PauseMusic();
	void StopMusic();
	void RestartMusic();


private:
	float m_defVolume;
	sf::Music m_music;
	std::vector<sf::Sound> m_channels;
	std::string m_musicFile;
};