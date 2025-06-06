#pragma once

#include "WeaponCustomPistol.h"
#include "script_export_space.h"

class CUIFrameWindow;
class CUIStatic;
class CBinocularsVision;

class CWeaponBinoculars : public CWeaponCustomPistol
{
private:
	typedef CWeaponCustomPistol inherited;

protected:
	SHudSound		sndZoomIn;
	SHudSound		sndZoomOut;
	f32				m_fRTZoomFactor; //run-time zoom factor
	bool			m_bVision;

public:
	CWeaponBinoculars( );
	virtual			~CWeaponBinoculars( );

	void			Load(pcstr section);

	virtual void	OnZoomIn( );
	virtual void	OnZoomOut( );
	virtual	void	ZoomInc( );
	virtual	void	ZoomDec( );
	virtual void	net_Destroy( );
	virtual BOOL	net_Spawn(CSE_Abstract* DC);

	virtual void	save(CNetPacket& output_packet);
	virtual void	load(IReader& input_packet);

	virtual bool	Action(s32 cmd, u32 flags);
	virtual void	UpdateCL( );
	virtual void	OnDrawUI( );
	virtual bool	use_crosshair( ) const
	{
		return false;
	}
	virtual void	GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count);
	virtual void	net_Relcase(CObject* object);

protected:
	CBinocularsVision* m_binoc_vision;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CWeaponBinoculars)
#undef script_type_list
#define script_type_list save_type_list(CWeaponBinoculars)
