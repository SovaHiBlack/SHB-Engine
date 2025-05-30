////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_Abstract.cpp
//	Created 	: 19.09.2002
//  Modified 	: 14.07.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma pack(push,4)

#include "..\xrNetServer\net_utils.h"
#include "xrServer_Objects_Abstract.h"
#include "xrMessages.h"

////////////////////////////////////////////////////////////////////////////
// CSE_Visual
////////////////////////////////////////////////////////////////////////////
CSE_Visual::CSE_Visual		   	(pcstr name)
{
	if(name)
	{
		string_path					tmp;
		strcpy						(tmp, name);
		if(strext(tmp)) 
			*strext(tmp)			=0;
		xr_strlwr					(tmp);
		visual_name					= tmp;
	}else
		visual_name					= NULL;

    startup_animation			= "$editor";
	flags.zero					();
}

CSE_Visual::~CSE_Visual			()
{
}

void CSE_Visual::set_visual	   	(pcstr name, bool load)
{
	string_path					tmp;
    strcpy						(tmp,name);
    if (strext(tmp))		 	*strext(tmp) = 0;
	xr_strlwr					(tmp);
	visual_name					= tmp; 
}

void CSE_Visual::visual_read   	(CNetPacket& tNetPacket, u16 version)
{
	tNetPacket.r_stringZ		(visual_name);
	if (version>103)
		flags.assign			(tNetPacket.r_u8());
}

void CSE_Visual::visual_write  	(CNetPacket& tNetPacket)
{
	tNetPacket.w_stringZ		(visual_name);
	tNetPacket.w_u8				(flags.get());
}

void CSE_Visual::OnChangeVisual	(PropValue* sender)
{
	ISE_Abstract* abstract		= smart_cast<ISE_Abstract*>(this); VERIFY(abstract);
	abstract->set_editor_flag	(ISE_Abstract::flVisualChange);
}

void CSE_Visual::OnChangeAnim(PropValue* sender)
{
	ISE_Abstract* abstract		= smart_cast<ISE_Abstract*>(this); VERIFY(abstract);
	abstract->set_editor_flag	(ISE_Abstract::flVisualAnimationChange);
}

void CSE_Visual::FillProps		(pcstr pref, PropItemVec &items)
{
	ISE_Abstract* abstract		= smart_cast<ISE_Abstract*>(this); VERIFY(abstract);
	ChooseValue *V 				= PHelper().CreateChoose(items, PrepareKey(pref,abstract->name(),"Model\\Visual"),		&visual_name,		smVisual);
	V->OnChangeEvent.bind		(this,&CSE_Visual::OnChangeVisual);
	V							= PHelper().CreateChoose(items,	PrepareKey(pref,abstract->name(),"Model\\Animation"),	&startup_animation, smSkeletonAnims,0,(void*)*visual_name);
	V->OnChangeEvent.bind		(this,&CSE_Visual::OnChangeAnim);
	PHelper().CreateFlag8		(items, PrepareKey(pref,abstract->name(),"Model\\Obstacle"),	&flags,	flObstacle);
}

////////////////////////////////////////////////////////////////////////////
// CSE_Animated
////////////////////////////////////////////////////////////////////////////
CSE_Motion::CSE_Motion			(pcstr name)
{
	motion_name					= name;
}

CSE_Motion::~CSE_Motion			()
{
}

void CSE_Motion::set_motion		(pcstr name)
{
	motion_name					= name;
}

void CSE_Motion::motion_read	(CNetPacket& tNetPacket)
{
	tNetPacket.r_stringZ		(motion_name);
}

void CSE_Motion::motion_write	(CNetPacket& tNetPacket)
{
	tNetPacket.w_stringZ			(motion_name);
}

void CSE_Motion::OnChangeMotion	(PropValue* sender)
{
	ISE_Abstract* abstract		= smart_cast<ISE_Abstract*>(this); VERIFY(abstract);
	abstract->set_editor_flag	(ISE_Abstract::flMotionChange);
}

void CSE_Motion::FillProps(pcstr pref, PropItemVec &items)
{
	ISE_Abstract* abstract		= smart_cast<ISE_Abstract*>(this); VERIFY(abstract);
	ChooseValue *V				= PHelper().CreateChoose(items, PrepareKey(pref,abstract->name(),"Motion"),&motion_name, smGameAnim);
	V->OnChangeEvent.bind		(this,&CSE_Motion::OnChangeMotion);
}

#pragma pack(pop,4)
