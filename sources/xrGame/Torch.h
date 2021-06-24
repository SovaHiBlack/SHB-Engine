#pragma once

#include "InventoryItemObject.h"
#include "HudSound.h"
#include "script_export_space.h"

class CLAItem;
class CMonsterEffector;

class CTorch : public CInventoryItemObject
{
private:
	using inherited								= CInventoryItemObject;

protected:
	float										fBrightness;
	CLAItem*									lanim;
	float										time2hide;

	U16											guid_bone;
	shared_str									light_trace_bone;

	float										m_delta_h;
	Fvector2									m_prev_hp;
	bool										m_switched_on;
	ref_light									light_render;
	ref_light									light_omni;
	ref_glow									glow_render;
	Fvector3										m_focus;

private:
	inline bool		can_use_dynamic_lights		( );

public:
					CTorch						( );
	virtual			~CTorch						( );

	virtual void	Load						(const char* section);
	virtual BOOL	net_Spawn					(CSE_Abstract* DC);
	virtual void	net_Destroy					( );
	virtual void	net_Export					(CNetPacket& P);				// export to server
	virtual void	net_Import					(CNetPacket& P);				// import from server

	virtual void	OnH_A_Chield				( );
	virtual void	OnH_B_Independent			(bool just_before_destroy);

	virtual void	UpdateCL					( );

	void			Switch						( );
	void			Switch						(bool light_on);

	virtual bool	can_be_attached				( ) const;

public:
	void			SwitchNightVision			( );
	void			SwitchNightVision			(bool light_on);
	void			UpdateSwitchNightVision		( );

protected:
	bool										m_bNightVisionEnabled;
	bool										m_bNightVisionOn;

	HUD_SOUND									m_NightVisionOnSnd;
	HUD_SOUND									m_NightVisionOffSnd;
	HUD_SOUND									m_NightVisionIdleSnd;
	HUD_SOUND									m_NightVisionBrokenSnd;

	enum EStats
	{
		eTorchActive							= (1 << 0),
		eNightVisionActive						= (1 << 1),
		eAttached								= (1 << 2)
	};

public:
	virtual bool	use_parent_ai_locations		( ) const
	{
		return !H_Parent( );
	}
	virtual void	create_physic_shell			( );
	virtual void	activate_physic_shell		( );
	virtual void	setup_physic_shell			( );

	virtual void	afterDetach					( );
	virtual void	renderable_Render			( );

	static void		script_register				(lua_State*);
};

add_to_type_list(CTorch)
#undef script_type_list
#define script_type_list save_type_list(CTorch)
