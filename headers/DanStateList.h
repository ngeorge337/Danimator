#pragma once

enum
{
	COL_STATE_FRAMES = 1,
	COL_STATE_DURATION,

	COL_SPRITES_SOURCE = 1,
	COL_SPRITES_SIZE,
	COL_SPRITES_DIMS,

	COL_SOUNDS_SOURCE = 1,
	COL_SOUNDS_SIZE,
};

class DanStateList : public DanList
{
public:
	DanStateList(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
				 long style = wxLC_ICON, const wxValidator &validator = wxDefaultValidator, const wxString &name = wxListCtrlNameStr);

	virtual void OnIdle(wxIdleEvent& event);

	void OnUpdateInfo();

	//wxDECLARE_EVENT_TABLE();
private:
};