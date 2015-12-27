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
	int type = m_undone.back().actionType;

	Insert(m_undone.back().actionType, m_undone.back().actMessage, false);
	switch(type)
	{
	case CMDTYPE_FRAMES:
		if(theFrame->GetAnimator().GetCurrentState() != nullptr)
		{
			sname = theFrame->GetAnimator().GetCurrentState()->name;
			offs = theFrame->GetAnimator().GetCurrentState()->GetOffset();
			theFrame->GetAnimator().m_currentState = nullptr;
			setMe = true;
		}

		theFrame->GetAnimator().m_validStates = m_undone.back().oldState;
		if(setMe && theFrame->GetAnimator().IsValidState(m_undone.back().lastState))
		{
			theFrame->GetAnimator().SetState(sname, offs);
		}
		theFrame->StateListCtrl->DanRestoreItems(m_undone.back().oldStateList);
		break;
	case CMDTYPE_TEXTURES:
		if(theFrame->textualPanel->GetActiveTexture() != nullptr)
		{
			theFrame->textualPanel->GetActiveTexture()->m_layers = m_undone.back().oldTexLayers;
		}
		break;
	default:
		break;
	}

	theFrame->ActionStatus("Redo: " + m_undone.back().actMessage);

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
	int type = m_commands.back().actionType;

	InsertRedo(m_commands.back().actionType, m_commands.back().actMessage);
	switch(type)
	{
	case CMDTYPE_FRAMES:
		if(theFrame->GetAnimator().GetCurrentState() != nullptr)
		{
			sname = theFrame->GetAnimator().GetCurrentState()->name;
			offs = theFrame->GetAnimator().GetCurrentState()->GetOffset();
			theFrame->GetAnimator().m_currentState = nullptr;
			setMe = true;
		}

		theFrame->GetAnimator().m_validStates = m_commands.back().oldState;
		if(setMe && theFrame->GetAnimator().IsValidState(m_commands.back().lastState))
		{
			theFrame->GetAnimator().SetState(sname, offs);
		}
		theFrame->StateListCtrl->DanRestoreItems(m_commands.back().oldStateList);
		break;
	case CMDTYPE_TEXTURES:
		if(theFrame->textualPanel->GetActiveTexture() != nullptr)
		{
			theFrame->textualPanel->GetActiveTexture()->m_layers = m_commands.back().oldTexLayers;
		}
		break;
	default:
		break;
	}

	theFrame->ActionStatus("Undo: " + m_commands.back().actMessage);

	Finish();
	m_commands.pop_back();
}

void ActionManager::Insert(int type, const std::string &actionMessage, bool clearRedo)
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
	case CMDTYPE_TEXTURES:
		if(theFrame->textualPanel->GetActiveTexture() != nullptr)
			act.oldTexLayers = theFrame->textualPanel->GetActiveTexture()->GetAllLayers();
		break;
	default:
		wxMessageBox("ActionManager::Insert() -- Unknown Action Type!", "ActionManager Error", wxOK | wxICON_ERROR);
		return;
		break;
	}
	m_commands.push_back(act);
	if(clearRedo)
	{
		theFrame->ActionStatus(actionMessage);
		if(!m_undone.empty())
			m_undone.clear();
	}

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
	case CMDTYPE_TEXTURES:
		if(theFrame->textualPanel->GetActiveTexture() != nullptr)
			act.oldTexLayers = theFrame->textualPanel->GetActiveTexture()->GetAllLayers();
		break;
	default:
		wxMessageBox("ActionManager::Insert() -- Unknown Action Type!", "ActionManager Error", wxOK | wxICON_ERROR);
		return;
		break;
	}
	m_undone.push_back(act);
}