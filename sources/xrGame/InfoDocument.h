// InfoDocument.h
// InfoDocument - документ, содержащий сюжетную информацию

#pragma once

#include "InventoryItemObject.h"
#include "InfoPortion_defs.h"

class CInfoDocument : public CInventoryItemObject
{
private:
	using inherited						= CInventoryItemObject;

public:
					CInfoDocument		( );
	virtual			~CInfoDocument		( );

	virtual BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	Load				(const char* section);
	virtual void	net_Destroy			( );
	virtual void	shedule_Update		(U32 dt);
	virtual void	UpdateCL			( );
	virtual void	renderable_Render	( );

	virtual void	OnH_A_Chield		( );
	virtual void	OnH_B_Independent	(bool just_before_destroy);

protected:
	//индекс информации, содержащейся в документе
	CSharedString							m_Info;
};
