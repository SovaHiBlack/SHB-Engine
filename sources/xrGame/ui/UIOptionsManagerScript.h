#pragma once

#include "../script_export_space.h"

class CUIOptionsManagerScript {
public:
	void SaveBackupValues(pcstr group);
	void SetCurrentValues(pcstr group);
	void SaveValues(pcstr group);
	bool IsGroupChanged(pcstr group);
	void UndoGroup(pcstr group);
	void OptionsPostAccept();
	void SendMessage2Group(pcstr group, pcstr message);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIOptionsManagerScript)
#undef script_type_list
#define script_type_list save_type_list(CUIOptionsManagerScript)