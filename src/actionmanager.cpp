#include "libs.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "textualPanel.h"
#include "mainFrame.h"
#include "actionmanager.h"

CVAR(Int, dan_maxundo, 30, 0, 0, "Maximum undo levels that are stored", CVAR_ARCHIVE);

extern DanFrame *theFrame;


void ActionManager::Finish()
{
	theFrame->UpdateAll();
}

void ActionManager::Wipe()
{
	m_commands.clear();
	m_undone.clear();
}

void ActionManager::Redo()
{
	if(m_undone.empty())
		return;

	bool setMe = false;
	int offs = 0;
	std::string sname = "";

	//m_commands.push_back(m_undone.back());
	InsertUndo(CMDTYPE_FRAMES, m_undone.back().actMessage);
	if(theFrame->GetAnimator().GetCurrentState() != nullptr)
	{
		//if(theFrame->GetAnimator().GetCurrentState()->name == m_undone.back().lastState)	// we're restoring over the current state so we need to set to this to fix
		//{
		sname = theFrame->GetAnimator().GetCurrentState()->name;
		offs = theFrame->GetAnimator().GetCurrentState()->GetOffset();
		theFrame->GetAnimator().m_currentState = nullptr;
		setMe = true;
		//}
	}

	theFrame->GetAnimator().m_validStates = m_undone.back().oldState;
	if(setMe && theFrame->GetAnimator().IsValidState(m_undone.back().lastState))
	{
		theFrame->GetAnimator().SetState(sname, offs);
	}

	theFrame->ActionStatus("Redo: " + m_undone.back().actMessage);
	theFrame->StateListCtrl->DanRestoreItems(m_undone.back().oldStateList);

	Finish();

	m_undone.pop_back();
}

void ActionManager::Undo()
{
	if(m_commands.empty())
		return;

	bool setMe = false;
	int offs = 0;
	std::string sname = "";

	InsertRedo(CMDTYPE_FRAMES, m_commands.back().actMessage);
	if(theFrame->GetAnimator().GetCurrentState() != nullptr)
	{
		//if(theFrame->GetAnimator().GetCurrentState()->name == m_undone.back().lastState)	// we're restoring over the current state so we need to set to this to fix
		//{
		sname = theFrame->GetAnimator().GetCurrentState()->name;
		offs = theFrame->GetAnimator().GetCurrentState()->GetOffset();
		theFrame->GetAnimator().m_currentState = nullptr;
		setMe = true;
		//}
	}

	theFrame->GetAnimator().m_validStates = m_commands.back().oldState;
	if(setMe && theFrame->GetAnimator().IsValidState(m_commands.back().lastState))
	{
		theFrame->GetAnimator().SetState(sname, offs);
	}


	theFrame->ActionStatus("Undo: " + m_commands.back().actMessage);
	theFrame->StateListCtrl->DanRestoreItems(m_commands.back().oldStateList);
		
	Finish();

	m_commands.pop_back();
}

void ActionManager::Insert(int type, const std::string &actionMessage)
{
	if(dan_maxundo <= 0)
	{
		Wipe();
		return;
	}
	action_t act;
	act.lastState = "";
	act.actionType = type;
	act.actMessage = actionMessage;
	switch(type)
	{
	case CMDTYPE_FRAMES:
		act.oldState = theFrame->GetAnimator().m_validStates;
		if(theFrame->GetAnimator().GetCurrentState() != nullptr)
			act.lastState = theFrame->GetAnimator().GetCurrentState()->name;
		act.oldStateList = theFrame->StateListCtrl->DanGetItems();
		break;
		// No way to retrieve wxListItem (which is dumb). I'll leave these undo options out for now
		// since it would require copying all the strings around, AND storing the texture/sound data...
		// That could actually get really complicated.
	case CMDTYPE_TEXTURELOAD:
		return;
		break;
	case CMDTYPE_SOUNDLOAD:
		return;
		break;
	default:
		wxMessageBox("ActionManager::Insert() -- Unknown Action Type!", "ActionManager Error", wxOK | wxICON_ERROR);
		return;
		break;
	}
	m_commands.push_back(act);
	if(!m_undone.empty())
		m_undone.clear();

	if(m_commands.size() > dan_maxundo)
	{
		while(m_commands.size() > dan_maxundo)
			m_commands.pop_front();
	}
}

void ActionManager::InsertRedo(int type, const std::string &actionMessage)
{
	if(dan_maxundo <= 0)
	{
		Wipe();
		return;
	}
	action_t act;
	act.lastState = "";
	act.actionType = type;
	act.actMessage = actionMessage;
	switch(type)
	{
	case CMDTYPE_FRAMES:
		act.oldState = theFrame->GetAnimator().m_validStates;
		if(theFrame->GetAnimator().GetCurrentState() != nullptr)
			act.lastState = theFrame->GetAnimator().GetCurrentState()->name;
		act.oldStateList = theFrame->StateListCtrl->DanGetItems();
		break;
		// No way to retrieve wxListItem (which is dumb). I'll leave these undo options out for now
		// since it would require copying all the strings around, AND storing the texture/sound data...
		// That could actually get really complicated.
	case CMDTYPE_TEXTURELOAD:
		return;
		break;
	case CMDTYPE_SOUNDLOAD:
		return;
		break;
	default:
		wxMessageBox("ActionManager::Insert() -- Unknown Action Type!", "ActionManager Error", wxOK | wxICON_ERROR);
		return;
		break;
	}
	m_undone.push_back(act);
}

void ActionManager::InsertUndo(int type, const std::string &actionMessage)
{
	if(dan_maxundo <= 0)
	{
		Wipe();
		return;
	}
	action_t act;
	act.lastState = "";
	act.actionType = type;
	act.actMessage = actionMessage;
	switch(type)
	{
	case CMDTYPE_FRAMES:
		act.oldState = theFrame->GetAnimator().m_validStates;
		if(theFrame->GetAnimator().GetCurrentState() != nullptr)
			act.lastState = theFrame->GetAnimator().GetCurrentState()->name;
		act.oldStateList = theFrame->StateListCtrl->DanGetItems();
		break;
		// No way to retrieve wxListItem (which is dumb). I'll leave these undo options out for now
		// since it would require copying all the strings around, AND storing the texture/sound data...
		// That could actually get really complicated.
	case CMDTYPE_TEXTURELOAD:
		return;
		break;
	case CMDTYPE_SOUNDLOAD:
		return;
		break;
	default:
		wxMessageBox("ActionManager::Insert() -- Unknown Action Type!", "ActionManager Error", wxOK | wxICON_ERROR);
		return;
		break;
	}
	m_commands.push_back(act);
}