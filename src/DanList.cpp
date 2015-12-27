#include "libs.h"
#include "DanList.h"


void DanList::OnIdle(wxIdleEvent& event)
{

}

void DanList::DanRestoreItems(std::vector<rowData_t> &items)
{
	DanDeleteAllItems();
	m_listItems = items;
	for(int i = 0; i < m_listItems.size(); i++)
	{
		std::vector<wxListItem> listItems = m_listItems[i].GetRowData();
		InsertItem(listItems[0]);
		for(int x = 1; x < listItems.size(); x++)
			SetItem(listItems[x]);
	}
}

std::vector<rowData_t> DanList::DanGetItems()
{
	return m_listItems;
}

long DanList::DanSetItem(long index, long col, wxString text)
{
	if(index >= m_listItems.size())
		return -1;

	wxListItem info;
	info.m_text = text;
	info.m_mask = wxLIST_MASK_TEXT;
	info.m_itemId = index;
	info.m_col = col;

	int idx = SetItem(info);
	m_listItems[index].SetColumnOfThisRow(col, info);
	//m_listItems[index].m_row[col] = info;

	return idx;
}

void DanList::DanDeleteAllItems()
{
	DeleteAllItems();
	m_listItems.clear();
}

void DanList::DanDeleteItem(long index)
{
	if(!DeleteItem(index))
		return;
	if(m_listItems.size() <= 1)
		m_listItems.clear();
	else
	{
		auto it = m_listItems.begin();
		std::advance(it, index);
		m_listItems.erase(it);
	}
}

long DanList::DanPushFront(wxString text)
{
	wxListItem info;
	info.m_text = text;
	info.m_mask = wxLIST_MASK_TEXT;
	info.m_itemId = 0;

	int idx = InsertItem(info);

	if(m_listItems.size() < idx + 1)
		m_listItems.resize(idx + 1);

	rowData_t row;
	row.m_owningStateName = text.ToStdString();
	row.m_row.push_back(info);

	m_listItems.insert(m_listItems.begin(), row);

	return idx;
}

long DanList::DanPushBack(wxString text)
{
	wxListItem info;
	info.m_text = text;
	info.m_mask = wxLIST_MASK_TEXT;
	info.m_itemId = GetItemCount();

	int idx = InsertItem(info);

	if(m_listItems.size() < idx + 1)
		m_listItems.resize(idx + 1);

	rowData_t row;
	row.m_owningStateName = text.ToStdString();
	row.m_row.push_back(info);

	m_listItems[idx] = row;

	return idx;
}

long DanList::DanInsert(long index, wxString text)
{
	wxListItem info;
	info.m_text = text;
	info.m_mask = wxLIST_MASK_TEXT;
	info.m_itemId = index;

	int idx = InsertItem(info);

	if(m_listItems.size() < idx + 1)
		m_listItems.resize(idx + 1);

	rowData_t row;
	row.m_owningStateName = text.ToStdString();
	row.m_row.push_back(info);

	auto it = m_listItems.begin();
	std::advance(it, idx);
	m_listItems.insert(it, row);
	//m_listItems[idx] = row;

	return idx;
}

DanList::DanList(wxWindow *parent, wxWindowID id, const wxPoint &pos /*= wxDefaultPosition*/, const wxSize &size /*= wxDefaultSize*/, long style /*= wxLC_ICON*/, const wxValidator &validator /*= wxDefaultValidator*/, const wxString &name /*= wxListCtrlNameStr*/) : wxListView(parent, id, pos, size, style, validator, name)
{

}

void DanList::ChangeOwner(wxString oldName, wxString newName)
{
	for(int i = 0; i < m_listItems.size(); ++i)
	{
		if(m_listItems[i].m_owningStateName == oldName.ToStdString())
			m_listItems[i].m_owningStateName = newName.ToStdString();
	}
}
