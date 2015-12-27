#pragma once

// srsly why is wxListCtrl such an inaccessible piece of garbage ancient programming standardadldfg kdjdsklrjgdjfnsdklrj
class DanList : public wxListView
{
public:
	DanList(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
			long style = wxLC_ICON, const wxValidator &validator = wxDefaultValidator, const wxString &name = wxListCtrlNameStr);

	long DanInsert(long index, wxString text);
	long DanPushBack(wxString text);
	long DanPushFront(wxString text);
	void DanDeleteItem(long index);
	void DanDeleteAllItems();
	long DanSetItem(long index, long col, wxString text);
	std::vector<rowData_t> DanGetItems();
	void DanRestoreItems(std::vector<rowData_t> &items);
	void ChangeOwner(wxString oldName, wxString newName);

	virtual void OnIdle(wxIdleEvent& event);

protected:
	std::vector<rowData_t> m_listItems;
};