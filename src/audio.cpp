#include "libs.h"
#include "util.h"
#include "static.h"
#include "file.h"
#include "configfile.h"
#include "ccmd.h"
#include "cvar.h"
#include "externcvar.h"
#include "console.h"
#include "token.h"
#include "strmanip.h"
#include "resrcman.h"
#include "soundman.h"
#include "services.h"
#include "audio.h"

CUSTOM_CVAR(Int, v_volume, 100, 0, 100, "Master volume level", CVAR_ARCHIVE)
{
	sf::Listener::setGlobalVolume(v_volume);
}
CUSTOM_CVAR(Int, v_musicvolume, 100, 0, 100, "Music volume level", CVAR_ARCHIVE)
{
	if(Services::GetAudioSystem() != nullptr && Services::GetAudioSystem()->GetMusic().getStatus() != sf::Music::Stopped)
		Services::GetAudioSystem()->GetMusic().setVolume(v_musicvolume);
}
CUSTOM_CVAR(Int, v_gamevolume, 100, 0, 100, "Game sound volume level", CVAR_ARCHIVE)
{
	if(Services::GetAudioSystem() != nullptr)
		Services::GetAudioSystem()->SetSoundVolume(v_gamevolume);
}

/*
CVAR(Int, v_volume, 50, 0, 100, "Master volume level", CVAR_ARCHIVE);
CVAR(Int, v_musicvolume, 25, 0, 100, "Music volume level", CVAR_ARCHIVE);
CVAR(Int, v_gamevolume, 50, 0, 100, "Game sound volume level", CVAR_ARCHIVE);
*/

CCMD(stopsound, "", "")
{
	Services::GetAudioSystem()->StopAllSounds();
}

void Audio::RestartMusic()
{
	StopMusic();
	PlayMusic();
}

void Audio::StopMusic()
{
	if(m_music.getStatus() != sf::Music::Stopped)
		m_music.stop();
}

void Audio::PauseMusic()
{
	if(m_music.getStatus() == sf::Music::Playing)
		m_music.pause();
}

void Audio::PlayMusic()
{
	m_music.play();
}

void Audio::SetMusic(const std::string &songFile, bool loop /*= true*/)
{
	if(songFile == "" || songFile.empty())
	{
		StopMusic();
		return;
	}
	std::string musDir = "./music/";
	if((musDir + songFile) == m_musicFile)
		return;
	m_musicFile = musDir + songFile;
	if(!m_music.openFromFile(m_musicFile))
		return;

	m_music.setVolume(v_musicvolume);
	m_music.setLoop(loop);
	m_music.play();
}

void Audio::PlaySoundAt(const std::string &sound, sf::Vector2f pos, int slot /*= CHAN_AUTO*/, bool loop /*= false*/)
{
	if(slot < -1)
		return;

	if(slot > m_channels.size())
	{
		m_channels.push_back(sf::Sound());
		m_channels.back().setVolume(m_defVolume);
	}

	if(slot == CHAN_AUTO)
	{
		for(int i = DEFAULT_CHANNELS; i < m_channels.size(); i++)
		{
			if(m_channels[i].getStatus() == sf::Sound::Stopped)
			{
				slot = i;
			}
		}
		if(slot == CHAN_AUTO)
		{
			if(m_channels.size() >= 255)
				return;

			m_channels.emplace_back();
			slot = m_channels.size() - 1;
		}
	}

	if(m_channels[slot].getStatus() == sf::Sound::Playing || m_channels[slot].getStatus() == sf::Sound::Paused)
		m_channels[slot].stop();

	m_channels[slot].setBuffer(*Services::GetSoundManager()->GetSound(sound));
	m_channels[slot].setRelativeToListener(false);
	m_channels[slot].setPosition(pos.x, pos.y, 0);
	m_channels[slot].setLoop(loop);
	m_channels[slot].play();
}

void Audio::UnpauseAllSounds()
{
	if(m_channels.empty())
		return;

	for(int i = 0; i < m_channels.size(); i++)
	{
		if(m_channels[i].getStatus() == sf::Sound::Paused)
			m_channels[i].play();
	}
}

void Audio::PlaySound(const std::string &sound, int slot /*= CHAN_AUTO*/, bool loop /*= false*/, float pitch)
{
	if(slot < -1)
		return;

	if(slot > m_channels.size())
	{
		m_channels.push_back(sf::Sound());
		m_channels.back().setVolume(m_defVolume);
	}

	if(slot == CHAN_AUTO)
	{
		for(int i = DEFAULT_CHANNELS; i < m_channels.size(); i++)
		{
			if(m_channels[i].getStatus() == sf::Sound::Stopped)
			{
				slot = i;
			}
		}
		if(slot == CHAN_AUTO)
		{
			if(m_channels.size() >= 255)
				return;

			m_channels.emplace_back();
			slot = m_channels.size() - 1;
		}
	}

	if(m_channels[slot].getStatus() == sf::Sound::Playing || m_channels[slot].getStatus() == sf::Sound::Paused)
		m_channels[slot].stop();

	m_channels[slot].setBuffer(*Services::GetSoundManager()->GetSound(sound));
	m_channels[slot].setRelativeToListener(true);
	m_channels[slot].setPosition(0, 0, 0);
	m_channels[slot].setLoop(loop);
	m_channels[slot].setPitch(pitch);
	m_channels[slot].play();
}

void Audio::PauseSound(int slot)
{
	if(slot > m_channels.size() || slot < 0)
		return;

	m_channels[slot].pause();
}

void Audio::PauseAllSounds()
{
	if(m_channels.empty())
		return;

	for(int i = 0; i < m_channels.size(); i++)
	{
		m_channels[i].pause();
	}
}

void Audio::StopSound(int slot)
{
	if(slot > m_channels.size() || slot < 0)
		return;

	m_channels[slot].stop();
}

void Audio::StopAllSounds()
{
	if(m_channels.empty())
		return;

	for(int i = 0; i < m_channels.size(); i++)
	{
		m_channels[i].stop();
	}
}

sf::Music & Audio::GetMusic()
{
	return m_music;
}

sf::Sound & Audio::GetSoundSlot(int slot)
{
	if(m_channels.empty() || slot > m_channels.size())
	{
		m_channels.push_back(sf::Sound());
		m_channels.back().setVolume(m_defVolume);
		return m_channels.back();
	}

	return m_channels[slot];
}

void Audio::Init()
{
	m_channels.clear();

	for(int i = 0; i < DEFAULT_CHANNELS + 4; i++)
	{
		m_channels.emplace_back();
		SetSoundVolume(v_gamevolume);
	}
	m_music.setRelativeToListener(true);
	m_music.setPosition(0, 0, 0);
	m_music.setVolume(v_musicvolume);
}

void Audio::SetSoundVolume(float vol)
{
	m_defVolume = vol;
	if(!m_channels.empty())
	{
		for(int i = 0; i < m_channels.size(); i++)
		{
			m_channels[i].setVolume(m_defVolume);
		}
	}
}