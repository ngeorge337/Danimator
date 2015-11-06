#pragma once

class WaitingCommand
{
	friend class Waiter;
public:
	WaitingCommand(std::function<bool(float t)> func);
	void Done();

private:
	std::function<bool(float t)> m_func;
	bool done;
};

class Waiter
{
public:
	void Update(float t);
	void Queue(WaitingCommand &cmd);

	int GetWaitingCount();
private:
	std::vector<WaitingCommand> m_waiting;
};