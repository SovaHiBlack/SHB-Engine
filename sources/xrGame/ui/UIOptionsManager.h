///////////////////////////////////
// class CUIOptionsManager
///////////////////////////////////

#pragma once

class CUIOptionsItem;

class CUIOptionsManager{
	friend class CUIOptionsItem;
public:	
	CUIOptionsManager();

	void SeveBackupValues(pcstr group);
	void SetCurrentValues(pcstr group);
	void SaveValues(pcstr group);
	bool IsGroupChanged(pcstr group);
	void UndoGroup(pcstr group);

	void OptionsPostAccept();
	void DoVidRestart();
	void DoSndRestart();

	void SendMessage2Group(pcstr group, pcstr message);

protected:	
	void RegisterItem(CUIOptionsItem* item, pcstr group);
	void UnRegisterGroup(pcstr group);
	void UnRegisterItem(CUIOptionsItem* item);


	typedef	xr_string									group_name;
	typedef xr_vector<CUIOptionsItem*>					items_list;
	typedef xr_map<group_name, items_list>				groups;
	typedef xr_map<group_name, items_list>::iterator	groups_it;

	groups	m_groups;

	bool	m_b_vid_restart;
	bool	m_b_snd_restart;
};