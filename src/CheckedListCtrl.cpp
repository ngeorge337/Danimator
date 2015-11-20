#include "libs.h"
#include "CheckedListCtrl.h"
#include "art/checked.xpm"
#include "art/unchecked.xpm"

IMPLEMENT_CLASS(wxCheckedListCtrl, wxListCtrl)

BEGIN_EVENT_TABLE(wxCheckedListCtrl, wxListCtrl)
EVT_LEFT_DOWN(wxCheckedListCtrl::OnMouseEvent)
END_EVENT_TABLE()

wxCheckedListCtrl::wxCheckedListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt,
const wxSize& sz, long style) :
wxListCtrl(parent, id, pt, sz, style), m_imageList(16, 16, TRUE)
{
	SetImageList(&m_imageList, wxIMAGE_LIST_SMALL);

	m_imageList.Add(wxICON(unchecked));
	m_imageList.Add(wxICON(checked));

	InsertColumn(0, _("Item"), wxLIST_FORMAT_LEFT, 200);
	InsertColumn(1, _("Value"), wxLIST_FORMAT_LEFT, 80);
}

void wxCheckedListCtrl::OnMouseEvent(wxMouseEvent& event)
{
	if(event.LeftDown())
	{
		int flags;
		long item = HitTest(event.GetPosition(), flags);
		if(item > -1 && (flags & wxLIST_HITTEST_ONITEMICON))
		{
			SetChecked(item, !IsChecked(item));
		}
		else
			event.Skip();
	}
	else
	{
		event.Skip();
	}
}

bool wxCheckedListCtrl::IsChecked(long item) const
{
	wxListItem info;
	info.m_mask = wxLIST_MASK_IMAGE;
	info.m_itemId = item;

	if(GetItem(info))
	{
		return (info.m_image == 1);
	}
	else
		return FALSE;
}

void wxCheckedListCtrl::SetChecked(long item, bool checked)
{
	SetItemImage(item, (checked ? 1 : 0), -1);
}