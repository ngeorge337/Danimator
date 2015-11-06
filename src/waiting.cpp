#include "libs.h"
#include "util.h"
#include "waiting.h"

//std::vector<WaitingCommand> Waiter::m_waiting = std::vector<WaitingCommand>();

int Waiter::GetWaitingCount()
{
	return m_waiting.size();
}

void Waiter::Queue( WaitingCommand &cmd )
{
	m_waiting.push_back(cmd);
}

void Waiter::Update( float t )
{
	if(m_waiting.empty())
		return;
	for(auto it = m_waiting.begin(); it != m_waiting.end();)
	{
		if((*it).m_func(t))
		{
			it = m_waiting.erase(it);
		}
		else
			++it;
	}
}

void WaitingCommand::Done()
{
	done = true;
}

WaitingCommand::WaitingCommand( std::function<bool(float t)> func ) : m_func(func), done(false)
{

}
