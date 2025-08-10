#pragma once

#include "script_export_space.h"

class CInventory;
class CGameObject;
class CCameraBase;
class CActor;

class CHolderCustom
{
private:
	CGameObject* m_owner;
	CActor* m_ownerActor;

protected:
	CGameObject* Owner( )
	{
		return m_owner;
	}
	CActor* OwnerActor( )
	{
		return m_ownerActor;
	}

public:
	CHolderCustom( )
	{
		m_owner = NULL;
		m_ownerActor = NULL;
	}
	virtual					~CHolderCustom( )
	{ }
	virtual	void			UpdateEx(f32 fov)
	{ } //called by owner
	virtual CHolderCustom* cast_holder_custom( )
	{
		return this;
	}
	bool			Engaged( )
	{
		return m_owner != NULL;
	}
	virtual void			OnMouseMove(s32 x, s32 y) = 0;
	virtual void			OnKeyboardPress(s32 dik) = 0;
	virtual void			OnKeyboardRelease(s32 dik) = 0;
	virtual void			OnKeyboardHold(s32 dik) = 0;
	// Inventory for the car
	virtual CInventory* GetInventory( ) = 0;

	virtual void			cam_Update(f32 dt, f32 fov = 90.0f) = 0;

	virtual bool			Use(const fVector3& pos, const fVector3& dir, const fVector3& foot_pos) = 0;
	virtual bool			attach_Actor(CGameObject* actor);
	virtual void			detach_Actor( );
	virtual bool			allowWeapon( )	const = 0;
	virtual bool			HUDView( ) const = 0;
	virtual fVector3			ExitPosition( ) = 0;
	virtual fVector3			ExitVelocity( )
	{
		return fVector3( ).set(0.0f, 0.0f, 0.0f);
	}
	virtual CCameraBase* Camera( ) = 0;
	virtual void			Action(s32 id, u32 flags)
	{ }
	virtual void			SetParam(s32 id, fVector2 val)
	{ }
	virtual void			SetParam(s32 id, fVector3 val)
	{ }
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CHolderCustom)
#undef script_type_list
#define script_type_list save_type_list(CHolderCustom)
