#include "stdafx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHShellSplitter.h"
#include "PHFracture.h"
#include "PHJointDestroyInfo.h"
#include "ExtendedGeom.h"

#include "PHElement.h"
#include "PHShell.h"

void CPHShell::applyHit(const fVector3& pos, const fVector3& dir, f32 val,const u16 id,ALife::EHitType hit_type)
{
	if(id==u16(-1)) return;//
#pragma	todo("Kosya to kosya:this code shold treat all hit types")
	if(!m_pKinematics)
	{
		applyImpulseTrace(pos,dir,val);
		return;
	}
	switch(hit_type) {
		case ALife::eHitTypeExplosion:
			ExplosionHit(pos,dir,val,id);
		break;
	default: applyImpulseTrace(pos,dir,val,id);
	}
}

void CPHShell::ExplosionHit(const fVector3& pos, const fVector3& dir, f32 val,const u16 id)
{
	if(!isActive()) return;
	EnableObject(0);
	//fVector3 local_pos;local_pos.set(0.f,0.f,0.f);
	ELEMENT_I i=elements.begin(),e=elements.end();
	f32 impulse=val/_sqrt(_sqrt((f32)elements.size()));
	for(;i!=e;i++)
	{	
		//fVector3 max_area_dir;	
		CPHElement* element=(*i);
		//element->get_MaxAreaDir(max_area_dir);
		//f32	sign=max_area_dir.dotproduct(dir)>0.f ? 1.f : -1.f;
		//max_area_dir.mul(sign);
		u16 gn=element->CPHGeometryOwner::numberOfGeoms();
		f32 g_impulse=impulse/gn;
		for(u16 j=0;j<gn;++j)
		{
			
			fVector3 r_dir;
			fVector3 r_pos;
			fVector3 r_box;
			f32 rad=element->getRadius();
			r_box.set(rad,rad,rad);
			r_pos.random_point(r_box);
			r_dir.random_dir();
			if(!fis_zero(pos.magnitude(), EPSILON_3))
			{
				r_dir.mul(0.5f);
				r_dir.add(dir);
			}

			r_dir.normalize_safe();//safe???
			element->applyImpulseTrace(r_pos,r_dir,g_impulse,element->CPHGeometryOwner::Geom(j)->bone_id());
		}
	}
}
