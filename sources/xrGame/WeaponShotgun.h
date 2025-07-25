#pragma once

#include "WeaponCustomPistol.h"
#include "script_export_space.h"

class CWeaponShotgun : public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;

public:
	CWeaponShotgun( );
	virtual ~CWeaponShotgun( );

	virtual void	Load(pcstr section);

	virtual void	net_Destroy( );
	virtual void	net_Export(CNetPacket& P);
	virtual void	net_Import(CNetPacket& P);

	virtual void	Reload( );
	virtual void	Fire2Start( );
	virtual void	Fire2End( );
	virtual void	OnShot( );
	virtual void	OnShotBoth( );
	virtual void	switch2_Fire( );
	virtual void	switch2_Fire2( );
	void			switch2_StartReload( );
	void			switch2_AddCartgidge( );
	void			switch2_EndReload( );

	virtual void	UpdateSounds( );
	virtual void	PlayAnimOpenWeapon( );
	virtual void	PlayAnimAddOneCartridgeWeapon( );
	void			PlayAnimCloseWeapon( );

	virtual bool	Action(s32 cmd, u32 flags);

protected:
	virtual void	OnAnimationEnd(u32 state);
	void			TriStateReload( );
	virtual void	OnStateSwitch(u32 S);

	bool			HaveCartridgeInInventory(u8 cnt);
	virtual u8		AddCartridge(u8 cnt);

	SHudSound		sndShotBoth;
	ESoundTypes		m_eSoundShotBoth;

	MotionSVec		mhud_shot_boths;

	ESoundTypes		m_eSoundOpen;
	ESoundTypes		m_eSoundAddCartridge;
	ESoundTypes		m_eSoundClose;
	SHudSound		m_sndOpen;
	SHudSound		m_sndAddCartridge;
	SHudSound		m_sndClose;
	MotionSVec		mhud_open;
	MotionSVec		mhud_add_cartridge;
	MotionSVec		mhud_close;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CWeaponShotgun)
#undef script_type_list
#define script_type_list save_type_list(CWeaponShotgun)
