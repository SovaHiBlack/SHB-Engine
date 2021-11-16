//	Module 		: xrServer_Objects_Abstract.h
//	Description : Server objects

#pragma once

#pragma pack(push,4)
#include "Server_space.h"
#include "../../xrCDB/xrCDB.h"
#include "ShapeData.h"

class CNetPacket;
class CDUInterface;

#include "xrEProps.h"
#include "..\ENGINE\DrawUtils.h"

#pragma warning(push)
#pragma warning(disable:4005)

class ISE_Shape
{
public:
	virtual void __stdcall			assign_shapes(CShapeData::shape_def* shapes, u32 cnt) = 0;
};

SERVER_ENTITY_DECLARE_BEGIN0(CSE_Visual)
void __stdcall					OnChangeVisual(PropValue* sender);
void __stdcall					OnChangeAnim(PropValue* sender);
public:
	CSharedString						visual_name;
	CSharedString						startup_animation;
	enum
	{
		flObstacle = (1 << 0)
	};
	Flags8							flags;
public:
	CSE_Visual(const char* name = 0);
	virtual							~CSE_Visual( );

	void							visual_read(CNetPacket& P, unsigned short version);
	void							visual_write(CNetPacket& P);

	void							set_visual(const char* name, bool load = true);
	const char* get_visual( ) const
	{
		return *visual_name;
	}
	virtual void					FillProps(const char* pref, PropItemsVec& items);

	virtual CSE_Visual* __stdcall	visual( ) = 0;
};
add_to_type_list(CSE_Visual)
#define script_type_list save_type_list(CSE_Visual)

SERVER_ENTITY_DECLARE_BEGIN0(CSE_Motion)
void __stdcall	OnChangeMotion(PropValue* sender);
public:
	CSharedString						motion_name;

	CSE_Motion(const char* name = 0);
	virtual							~CSE_Motion( );

	void							motion_read(CNetPacket& P);
	void							motion_write(CNetPacket& P);

	void							set_motion(const char* name);
	const char* get_motion( ) const
	{
		return *motion_name;
	}

	virtual void					FillProps(const char* pref, PropItemsVec& items);

	virtual CSE_Motion* __stdcall	motion( ) = 0;
};
add_to_type_list(CSE_Motion)
#define script_type_list save_type_list(CSE_Motion)

struct ISE_AbstractLEOwner
{
	virtual void __stdcall	get_bone_xform(const char* name, Fmatrix& xform) = 0;
};

struct ISE_Abstract
{
public:
	enum
	{
		flUpdateProperties = u32(1 << 0),
		flVisualChange = u32(1 << 1),
		flVisualAnimationChange = u32(1 << 2),
		flMotionChange = u32(1 << 3)
	};
	Flags32							m_editor_flags;
	inline	void						set_editor_flag(u32 mask)
	{
		m_editor_flags.set(mask, TRUE);
	}

	virtual void		__stdcall	Spawn_Write(CNetPacket& tNetPacket, BOOL bLocal) = 0;
	virtual BOOL		__stdcall	Spawn_Read(CNetPacket& tNetPacket) = 0;
	virtual void		__stdcall	FillProp(const char* pref, PropItemsVec& items) = 0;
	virtual const char* __stdcall	name( ) const = 0;
	virtual void		__stdcall	set_name(const char*) = 0;
	virtual const char* __stdcall	name_replace( ) const = 0;
	virtual void		__stdcall	set_name_replace(const char*) = 0;
	virtual Fvector3& __stdcall	position( ) = 0;
	virtual Fvector3& __stdcall	angle( ) = 0;
	virtual Flags16& __stdcall	flags( ) = 0;
	virtual ISE_Shape* __stdcall	shape( ) = 0;
	virtual CSE_Visual* __stdcall	visual( ) = 0;
	virtual CSE_Motion* __stdcall	motion( ) = 0;
	virtual bool		__stdcall	validate( ) = 0;
	virtual void 		__stdcall	on_render(CDUInterface* du, ISE_AbstractLEOwner* owner, bool bSelected, const Fmatrix& parent, int priority, bool strictB2F) = 0;
};

#pragma warning(pop)
#pragma pack(pop)
