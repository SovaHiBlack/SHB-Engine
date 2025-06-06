#pragma once

#include "..\XR_3DA\feel_touch.h"
#include "InventoryItemObject.h"

#include "InfoPortion_defs.h"
#include "CharacterInfo_defs.h"

#include "PDA_msg.h"

class CInventoryOwner;
class CPda;

DEF_VECTOR(PDA_LIST, CPda*);

class CPda : public CInventoryItemObject, public Feel::Touch
{
	typedef	CInventoryItemObject inherited;

public:
	CPda( );
	virtual									~CPda( );

	virtual BOOL 							net_Spawn(CSE_Abstract* DC);
	virtual void 							Load(pcstr section);
	virtual void 							net_Destroy( );

	virtual void 							OnH_A_Chield( );
	virtual void 							OnH_B_Independent(bool just_before_destroy);

	virtual void 							shedule_Update(u32 dt);

	virtual void 							feel_touch_new(CObject* O);
	virtual void 							feel_touch_delete(CObject* O);
	virtual BOOL 							feel_touch_contact(CObject* O);

	virtual u16								GetOriginalOwnerID( )
	{
		return m_idOriginalOwner;
	}
	virtual CInventoryOwner* GetOriginalOwner( );
	virtual CObject* GetOwnerObject( );

	void							TurnOn( )
	{
		m_bTurnedOff = false;
	}
	void							TurnOff( )
	{
		m_bTurnedOff = true;
	}

	bool 							IsActive( )
	{
		return IsOn( );
	}
	bool 							IsOn( )
	{
		return !m_bTurnedOff;
	}
	bool 							IsOff( )
	{
		return m_bTurnedOff;
	}

	void							ActivePDAContacts(xr_vector<CPda*>& res);
	CPda* GetPdaFromOwner(CObject* owner);
	u32								ActiveContactsNum( )
	{
		return m_active_contacts.size( );
	}

	virtual void							save(CNetPacket& output_packet);
	virtual void							load(IReader& input_packet);

	virtual pcstr							Name( );

protected:
	void									UpdateActiveContacts( );

	xr_vector<CObject*>						m_active_contacts;
	f32										m_fRadius;

	u16										m_idOriginalOwner;
	shared_str								m_SpecificChracterOwner;
	xr_string								m_sFullName;

	bool									m_bTurnedOff;
};
