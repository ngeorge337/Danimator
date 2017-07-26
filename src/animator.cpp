#include "libs.h"
#include "util.h"
#include "static.h"
#include "token.h"
#include "strmanip.h"
#include "file.h"
#include "configfile.h"
#include "ccmd.h"
#include "cvar.h"
#include "console.h"
#include "file.h"
#include "configfile.h"
#include "resrcman.h"
#include "texman.h"
#include "services.h"
#include "audio.h"
#include "animator.h"
#include "textualPanel.h"
#include "mainFrame.h"

extern DanFrame *theFrame;

bool FState_t::Tick( float t )
{
	//return true if the state is complete, false if not

	if(m_frames.empty())
		return true;

	if(justEntered)
	{
		justEntered = false;
		if(m_frames[frameOffset].hasSound && !m_frames[frameOffset].soundName.empty())
		{
			if(theFrame->allowSoundCheckBox->GetValue() == true)
				Services::GetAudioSystem()->PlaySound(m_frames[frameOffset].soundName, CHAN_AUTO, false);
		}
	}
	
	m_ticks += t;
	if(m_ticks >= m_frames[frameOffset].duration)
	{
		//m_ticks -= m_frames[frameOffset].duration;
		m_ticks = 0;
		frameOffset++;
		if(frameOffset >= m_frames.size()) // Reached the end of the animation frames
		{
			Services::GetAudioSystem()->StopAllSounds();
			frameOffset = 0;
			m_ticks = 0;
			return true;
		}
		else
		{
			if(!m_frames[frameOffset].soundName.empty() && m_frames[frameOffset].hasSound)
				Services::GetAudioSystem()->PlaySound(m_frames[frameOffset].soundName, CHAN_AUTO, false);
		}
	}
	return false;
}

Frame_t &FState_t::GetCurrentFrame()
{
	if(m_frames.empty())
		return AddEmptyFrame();
	return m_frames[frameOffset];
}

Frame_t &FState_t::AddFrame(std::shared_ptr<sf::Texture> _texture, float _duration)
{
	if(m_frames.empty())
	{
		frameOffset = 0;
		m_ticks = 0;
	}
	Frame_t f;
	f.duration = _duration;
	_texture->setSmooth(r_bilinear);
	f.sprite.setTexture(*_texture, true);
	f.sprite.setColor(m_overrideColor);
	f.sprite.setScale(_scaleX, _scaleY);
	m_frames.push_back(f);
	return m_frames.back();
}

void FState_t::Reset( int offset )
{
	frameOffset = offset;
	if(frameOffset >= m_frames.size())
		frameOffset = m_frames.size() - 1;
	m_ticks = 0;
	justEntered = true;
}

void FState_t::SetScale( float _x, float _y )
{
	_scaleX = _x;
	_scaleY = _y;
	if(m_frames.empty())
		return;
	for(int i = 0; i < m_frames.size(); i++)
	{
		m_frames[i].sprite.setScale(_x, _y);
	}
}

void FState_t::SetColor( sf::Color clr )
{
	m_overrideColor = clr;
	if(m_frames.empty())
		return;
	for(int i = 0; i < m_frames.size(); i++)
	{
		m_frames[i].sprite.setColor(clr);
	}
}

void FState_t::FlipX(bool flip)
{
	if(flippedX == flip)
		return;
	if(m_frames.empty())
		return;

	sf::Vector2f offs(0.f, 0.f);
	flippedX = flip;
	for(int i = 0; i < m_frames.size(); i++)
	{
		float scalex_ = fabs(m_frames[i].sprite.getScale().x);
		sf::FloatRect bounds = m_frames[i].sprite.getLocalBounds();
		if(flip)
		{
			offs = sf::Vector2f(bounds.width * scalex_, 0.f);
			scalex_ *= -1.f;
		}

		m_frames[i].sprite.setOrigin(bounds.left, bounds.top);
		m_frames[i].sprite.setPosition(offs);
		m_frames[i].sprite.setScale(scalex_, m_frames[i].sprite.getScale().y);
		offs = sf::Vector2f();
	}
}

void FState_t::FlipY(bool flip)
{
	if(flippedY == flip)
		return;
	if(m_frames.empty())
		return;

	sf::Vector2f offs(0.f, 0.f);
	flippedY = flip;
	for(int i = 0; i < m_frames.size(); i++)
	{
		float scaley_ = fabs(m_frames[i].sprite.getScale().y);
		sf::FloatRect bounds = m_frames[i].sprite.getLocalBounds();
		if(flip)
		{
			offs = sf::Vector2f(0.f, bounds.height * scaley_);
			scaley_ *= -1.f;
		}

		m_frames[i].sprite.setOrigin(bounds.left, bounds.top);
		m_frames[i].sprite.setPosition(offs);
		m_frames[i].sprite.setScale(m_frames[i].sprite.getScale().x, scaley_);
		offs = sf::Vector2f();
	}
}

int FState_t::GetOffset()
{
	return frameOffset;
}

const sf::Vector2f FState_t::GetScale()
{
	return sf::Vector2f(_scaleX, _scaleY);
}

void FState_t::SetOffset(int f)
{
	Reset(f);
}

Frame_t &FState_t::GetFrame(int offset)
{
	if(offset < 0 || m_frames.size() <= offset)
		return GetCurrentFrame();

	return m_frames[offset];
}

Frame_t & FState_t::AddEmptyFrame()
{
	if(m_frames.empty())
	{
		frameOffset = 0;
		m_ticks = 0;
	}

	m_frames.push_back(Frame_t());
	return m_frames.back();
}

void FState_t::RemoveFrame(int pos /*= -1*/)
{
	if(pos < -1 || pos >= m_frames.size())
		return;
	if(pos == -1)
		pos = frameOffset;
	m_frames.erase(m_frames.begin() + pos);
	//frameOffset--;
}

Frame_t & FState_t::InsertEmptyFrame(int pos /*= -1*/)
{
	if(m_frames.empty())
		return AddEmptyFrame();
	if(pos <= -1)
		pos = frameOffset;
	auto it = m_frames.begin();
	std::advance(it, pos + 1);
	m_frames.insert(it, Frame_t());
	return m_frames[pos];
}

void Animator::Tick( float t )
{
	if(m_currentState == nullptr)
		return;

	//if(!IsValidState(m_currentState->name))	// redundancy check
	//	GotoDefault();

	if(m_currentState->Tick(t))	// has the state completed?
	{
		if(theFrame->loopCheckBox->GetValue())
			m_currentState->Reset();
		else
			theFrame->SetPlayingMode(false);
	}
	//else
	//	m_frameStates.top().Reset();
}

void Animator::SetState( const std::string &_name, int offset /*= 0*/ )
{
	if(offset == -1 && m_currentState != nullptr) // use current offset if we set -1
	{
		offset = m_currentState->frameOffset;
	}

	if(m_currentState == nullptr)
	{
		if(!IsValidState(_name))
		{
			Console->LogMessage("Animator::SetState() -- Could not set state '%s', state does not exist!", _name);
			return;
		}
		m_currentState = GetState(_name);
		m_currentState->Reset(offset);
	}

	if(CompNoCase(m_currentState->name, _name))
	{
		m_currentState->Reset(offset);
	}
	else
	{
		m_currentState = GetState(_name);
		m_currentState->Reset(offset);
	}
}

FState_t *Animator::GetCurrentState()
{
	if(m_currentState == nullptr)
	{
		return nullptr;
	}

	return m_currentState;
}

FState_t *Animator::GetState( const std::string &_name )
{
	return &m_validStates[_name];
}

FState_t *Animator::CreateState( const std::string &_name )
{
	FState_t s;
	s.name = _name;
	m_validStates.insert(std::pair<std::string, FState_t>(_name, s));
	return &m_validStates[_name];
}

Animator::Animator() : m_currentState(nullptr), m_validStates(std::map<std::string, FState_t>())//, m_currentAlias("")
{
}

void Animator::ClearStates()
{
	m_validStates.clear();
	m_currentState = nullptr;
}

void Animator::ResetStates()
{
	m_currentState = nullptr;
}

void Animator::SetStateFlipX( bool x )
{
	for(auto it = m_validStates.begin(); it != m_validStates.end(); ++it)
	{
		(*it).second.FlipX(x);
	}
}

void Animator::SetStateFlipY( bool y )
{
	for(auto it = m_validStates.begin(); it != m_validStates.end(); ++it)
	{
		(*it).second.FlipY(y);
	}
}

bool Animator::IsValidState(const std::string &_name)
{
	return m_validStates.find(_name) != m_validStates.end();
}

void Animator::DeleteState(const std::string &_name)
{
	if(IsValidState(_name))
	{
		if(m_currentState != nullptr)
		{
			if(m_currentState->name == _name)
				m_currentState = nullptr;
		}
		m_validStates.erase(_name);
	}
}

void Animator::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if(m_currentState == nullptr)
		return;

	// Ghosting
	float alphaIncrement = 0.f;
	int ghostCount = 0;
	float currentAlpha = 0.f;
	if(theFrame->allowGhostCheckBox->GetValue() && !theFrame->playingMode)
	{
		ghostCount = theFrame->ghostFramesSpin->GetValue();
		alphaIncrement = 1.f / float(ghostCount + 1);

		if(m_currentState->frameOffset - ghostCount < 0)
			ghostCount += m_currentState->frameOffset - ghostCount;

		if(ghostCount > 0)
		{
			for(int i = 0; i < ghostCount; i++)
			{
				int index = m_currentState->frameOffset - (ghostCount - i);
				currentAlpha += alphaIncrement;
				m_currentState->m_frames[index].sprite.setColor(sf::Color(255, 255, 255, FloatToByte(currentAlpha)));
				//m_currentState->m_frames[m_currentState->frameOffset - (i - ghostCount)].sprite.setPosition(sf::Vector2f(0, 0));
				target.draw(m_currentState->m_frames[index].sprite);
				m_currentState->m_frames[index].sprite.setColor(sf::Color::White);
			}
		}
	}

	//m_currentState->GetCurrentFrame().sprite.setPosition(sf::Vector2f(0, 0));
	target.draw(m_currentState->GetCurrentFrame().sprite);
}

// This updates any references to texture images we might have
// necessary for loading new images (overwriting old ones), or
// for updating sprites with newer composite texture output
void Animator::EnsureSpritePointers()
{
	for(auto it = m_validStates.begin(); it != m_validStates.end(); ++it)
	{
		for(int i = 0; i < it->second.m_frames.size(); i++)
		{
			it->second.m_frames[i].sprite.setTexture(*Services::GetTextureManager()->GetTexture(it->second.m_frames[i].spriteName), true);
		}
	}
}

void Animator::RenameState(const std::string &name, const std::string &newName)
{
	if(m_validStates.find(name) == m_validStates.end())
		return;

	bool setme = false;
	int fs = GetCurrentState()->GetOffset();
	if(CompNoCase(m_currentState->name, name))
	{
		m_currentState = nullptr;
		setme = true;
	}

	m_validStates[newName] = FState_t(m_validStates[name]);
	m_validStates[newName].name = newName;
	if(setme)
		SetState(newName, fs);
	m_validStates.erase(name);
}

void Animator::DuplicateState(const std::string &name, const std::string &newName)
{
	if(m_validStates.find(name) == m_validStates.end())
		return;

	m_validStates[newName] = FState_t(m_validStates[name]);
	m_validStates[newName].name = newName;
}

void Animator::SetNull()
{
	m_currentState = nullptr;
}

void Animator::UpdateSpriteNames(const std::string &oldname, const std::string &newname)
{
	for(auto it = m_validStates.begin(); it != m_validStates.end(); ++it)
	{
		for(int i = 0; i < it->second.m_frames.size(); i++)
		{
			it->second.m_frames[i].sprite.setTexture(*Services::GetTextureManager()->GetTexture(newname), true);
			it->second.m_frames[i].spriteName = newname;
		}
	}
}

void Animator::UpdateSoundNames(const std::string &oldname, const std::string &newname)
{
	for(auto it = m_validStates.begin(); it != m_validStates.end(); ++it)
	{
		for(int i = 0; i < it->second.m_frames.size(); i++)
		{
			if(it->second.m_frames[i].hasSound)
				if(it->second.m_frames[i].soundName == oldname)
					it->second.m_frames[i].soundName = newname;
		}
	}
}

Frame_t::Frame_t() : duration(1.f), tics(1), sprite(sf::Sprite()), spriteName("TNT1A0"), soundName(""), hasSound(false)
{
	sprite.setTexture(*Services::GetTextureManager()->GetTexture("TNT1A0"), true);
	duration = TICS(1);
}
