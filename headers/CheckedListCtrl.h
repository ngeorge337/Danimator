#pragma once

class wxCheckedListCtrl : wxListCtrl
{
public:
	wxCheckedListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt,
					  const wxSize& sz, long style);

	void OnMouseEvent(wxMouseEvent& event);
	bool IsChecked(long item) const;
	void SetChecked(long item, bool checked);

	DECLARE_CLASS(wxCheckedListCtrl)
	DECLARE_EVENT_TABLE()

private:
	wxImageList m_imageList;

};