#include "libs.h"
#include "animator.h"
#include "DanList.h"
#include "DanStateList.h"
#include "textualPanel.h"
#include "mainFrame.h"

// such coupling, many bad, wow
extern DanFrame *theFrame;


DanStateList::DanStateList(wxWindow *parent, wxWindowID id, const wxPoint &pos /*= wxDefaultPosition*/, const wxSize &size /*= wxDefaultSize*/, long style /*= wxLC_ICON*/, const wxValidator &validator /*= wxDefaultValidator*/, const wxString &name /*= wxListCtrlNameStr*/) : DanList(parent, id, pos, size, style, validator, name)
{

}

void DanStateList::OnIdle(wxIdleEvent& event)
{
}

void DanStateList::OnUpdateInfo()
{
	std::map<std::string, FState_t> &states = theFrame->GetAnimator().m_validStates;

	// refresh all the state information
	int i = -1;
	for(auto it = states.begin(); it != states.end(); ++it)
	{
		++i;
		int count = states[m_listItems[i].m_owningStateName].m_frames.size();
		DanSetItem(i, COL_STATE_FRAMES, wxString(std::to_string(count)));
		int totalTime = 0;
		for(int x = 0; x < count; x++)
			totalTime += states[m_listItems[i].m_owningStateName].m_frames[x].tics;
		DanSetItem(i, COL_STATE_DURATION, wxString(std::to_string(totalTime)));
	}
}