////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_Abstract.h
//	Created 	: 19.09.2002
//  Modified 	: 18.06.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_AbstractH
#define xrServer_Objects_AbstractH

#pragma pack(push,4)
#include "xrServer_Space.h"
#ifdef XRGAME_EXPORTS
#	include "..\xrCDB\xrCDB.h"
#else // XRGAME_EXPORTS
#	include "../xrCDB/xrCDB.h"
#endif // XRGAME_EXPORTS
#include "ShapeData.h"

class CNetPacket;
class CDUInterface;

#ifndef XRGAME_EXPORTS
   #include "Sound.h"
#endif

#include "xrEProps.h"
#include "..\XR_3DA\DrawUtils.h"

#pragma warning(push)
#pragma warning(disable:4005)

class ISE_Shape{
public:
	virtual void __stdcall			assign_shapes	(CShapeData::shape_def* shapes, u32 cnt)=0;
};

SERVER_ENTITY_DECLARE_BEGIN0(CSE_Visual)
    void __stdcall					OnChangeVisual	(PropValue* sender);  
    void __stdcall					OnChangeAnim	(PropValue* sender);  
public:
	shared_str						visual_name;
	shared_str						startup_animation;
	enum{
		flObstacle					= (1<<0)
	};
	flags8							flags;
public:
									CSE_Visual		(pcstr name=0);
	virtual							~CSE_Visual		();

	void							visual_read		(CNetPacket& P, u16 version);
	void							visual_write	(CNetPacket& P);

    void							set_visual		(pcstr name, bool load=true);
	pcstr							get_visual		() const {return *visual_name;};
	virtual void					FillProps		(pcstr pref, PropItemVec &items);

	virtual CSE_Visual* __stdcall	visual			() = 0;
};
add_to_type_list(CSE_Visual)
#define script_type_list save_type_list(CSE_Visual)

SERVER_ENTITY_DECLARE_BEGIN0(CSE_Motion)
	void __stdcall	OnChangeMotion	(PropValue* sender);  
public:
	shared_str						motion_name;
public:
									CSE_Motion 		(pcstr name=0);
	virtual							~CSE_Motion		();

	void							motion_read		(CNetPacket& P);
	void							motion_write	(CNetPacket& P);

    void							set_motion		(pcstr name);
	pcstr							get_motion		() const {return *motion_name;};

	virtual void					FillProps		(pcstr pref, PropItemVec &items);

	virtual CSE_Motion* __stdcall	motion			() = 0;
};
add_to_type_list(CSE_Motion)
#define script_type_list save_type_list(CSE_Motion)

struct ISE_AbstractLEOwner{
	virtual void		__stdcall	get_bone_xform	(pcstr name, fMatrix4x4& xform) = 0;
};

struct ISE_Abstract {
public:
	enum {
		flUpdateProperties			= u32(1 << 0),
		flVisualChange				= u32(1 << 1),
		flVisualAnimationChange		= u32(1 << 2),
		flMotionChange				= u32(1 << 3),
	};
	flags32							m_editor_flags;
	IC	void						set_editor_flag	(u32 mask)	{m_editor_flags.set	(mask,TRUE);}

public:
	virtual void		__stdcall	Spawn_Write		(CNetPacket& tNetPacket, BOOL bLocal) = 0;
	virtual BOOL		__stdcall	Spawn_Read		(CNetPacket& tNetPacket) = 0;
	virtual void		__stdcall	FillProp		(pcstr pref, PropItemVec &items) = 0;
	virtual pcstr		__stdcall	name			() const = 0;
	virtual void		__stdcall	set_name		(pcstr) = 0;
	virtual pcstr		__stdcall	name_replace	() const = 0;
	virtual void		__stdcall	set_name_replace(pcstr) = 0;
	virtual fVector3&	__stdcall	position		() = 0;
	virtual fVector3&	__stdcall	angle			() = 0;
	virtual flags16&	__stdcall	flags			() = 0;
	virtual ISE_Shape*  __stdcall	shape			() = 0;
	virtual CSE_Visual* __stdcall	visual			() = 0;
	virtual CSE_Motion* __stdcall	motion			() = 0;
	virtual bool		__stdcall	validate		() = 0;
	virtual void 		__stdcall	on_render		(CDUInterface* du, ISE_AbstractLEOwner* owner, bool bSelected, const fMatrix4x4& parent,int priority, bool strictB2F) = 0;
};

#pragma warning(pop)

#pragma pack(pop)
#endif // xrServer_Objects_AbstractH