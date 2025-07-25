#include "stdafx.h"

#include "BreakableObject.h"
#include "xrserver_objects_alife.h"
#include "PHStaticGeomShell.h"
#include "PhysicsShell.h"
#include "Physics.h"
#include "..\XR_3DA\xr_collide_form.h"
#include "../xrNetServer/net_utils.h"
#include "clsid_game.h"
#include "..\XR_3DA\skeletoncustom.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
u32		CBreakableObject		::	m_remove_time		=0		;
f32	CBreakableObject::m_damage_threshold = 5.0f;
f32	CBreakableObject::m_health_threshhold = 0.0f;
f32	CBreakableObject::m_immunity_factor = 0.1f;
CBreakableObject::CBreakableObject	()
{
	Init();
}

CBreakableObject::~CBreakableObject( )
{ }

void CBreakableObject::Load		(pcstr section)
{
	inherited::Load			(section);
	m_remove_time=pSettings	->r_u32(section,"remove_time")*1000;
	m_health_threshhold=pSettings	->r_float(section,"hit_break_threthhold");
	m_damage_threshold=pSettings	->r_float(section,"collision_break_threthhold");
	m_immunity_factor  =pSettings	->r_float(section,"immunity_factor");
	this->shedule.t_min	= 1000;
	this->shedule.t_max	= 1000;
}

BOOL CBreakableObject::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract			*e		= (CSE_Abstract*)(DC);
	CSE_ALifeObjectBreakable *obj	= smart_cast<CSE_ALifeObjectBreakable*>(e);
	R_ASSERT				(obj);
	inherited::net_Spawn	(DC);
	VERIFY(!collidable.model);
	collidable.model = xr_new<CCF_Skeleton>(this);
	// set bone id
	R_ASSERT				(Visual()&&smart_cast<CKinematics*>(Visual()));
//	CKinematics* K			= smart_cast<CKinematics*>(Visual());
	fHealth					= obj->m_health;
	processing_deactivate	();
	setVisible				(TRUE);
	setEnabled				(TRUE);
	CreateUnbroken			();
	bRemoved				=false;
	//Break					();
//	shedule_unregister		();
	return					(TRUE);
}

void CBreakableObject::shedule_Update	(u32 dt)
{
	inherited::shedule_Update		(dt);
	if(m_pPhysicsShell&&!bRemoved&&Device.dwTimeGlobal-m_break_time>m_remove_time) SendDestroy();
}
void CBreakableObject::UpdateCL()
{
	inherited::UpdateCL();
	if (m_pPhysicsShell && m_pPhysicsShell->isFullActive( ))
	{
		m_pPhysicsShell->InterpolateGlobalTransform(&XFORM( ));
	}
}

void CBreakableObject::enable_notificate()
{
	if (b_resived_damage)
	{
		ProcessDamage( );
	}
}

void	CBreakableObject::Hit					(SHit* pHDS)
{
	CheckHitBreak(pHDS->damage(),pHDS->hit_type);
	if(m_pPhysicsShell)
	{
		if(pHDS->hit_type==ALife::eHitTypeExplosion)
		{
			ApplyExplosion(pHDS->dir,pHDS->impulse);
		}
		else
		{
			//. hack: slipch ???
			if ((pHDS->impulse> EPSILON_5)&&(BI_NONE != pHDS->bone()))
				m_pPhysicsShell->applyImpulseTrace(pHDS->p_in_bone_space,pHDS->dir,pHDS->impulse,pHDS->bone());
		}
	}
}

void CBreakableObject::net_Export(CNetPacket& P)
{
	VERIFY					(Local());
}

void CBreakableObject::net_Import(CNetPacket& P)
{
	VERIFY					(Remote());
}

BOOL CBreakableObject::UsedAI_Locations()
{
	return					(FALSE);
}

void CBreakableObject::CreateUnbroken()
{
	m_pUnbrokenObject=P_BuildStaticGeomShell(smart_cast<CGameObject*>(this),ObjectContactCallback);
}

void CBreakableObject::DestroyUnbroken()
{
	if(!m_pUnbrokenObject) return;
	m_pUnbrokenObject->Deactivate();
	xr_delete(m_pUnbrokenObject);
}

void CBreakableObject::CreateBroken()
{
	processing_activate();
	m_Shell=P_create_splited_Shell();
	m_Shell->preBuild_FromKinematics(smart_cast<CKinematics*>(Visual()));
	m_Shell->mXFORM.set(XFORM());
	//m_Shell->SetAirResistance(0.002f*skel_airr_lin_factor,
	//	0.3f*skel_airr_ang_factor);
	m_Shell->set_PhysicsRefObject(this);
	m_Shell->Build();
	m_Shell->setMass(m_Shell->getMass()*0.1f*100.f);
	dMass m;
	dMassSetBox(&m,m_Shell->getMass()/100.f,1.f,1.f,1.f);
	m_Shell->addEquelInertiaToEls(m);
	m_Shell->SmoothElementsInertia(0.3f);
	fObb b;
	Visual()->vis.box.getradius(b.m_halfsize);
	m_Shell->SetMaxAABBRadius(_max(_max(b.m_halfsize.x,b.m_halfsize.y),b.m_halfsize.z)*2.f);//+2.f
}

void CBreakableObject::ActivateBroken()
{
	m_pPhysicsShell=m_Shell;
	CKinematics* K=smart_cast<CKinematics*>(Visual());
	m_pPhysicsShell->set_Kinematics(K);
	m_pPhysicsShell->RunSimulation();
	m_pPhysicsShell->SetCallbacks(m_pPhysicsShell->GetBonesCallback());
	K->CalculateBones_Invalidate();
	K->CalculateBones();
	m_pPhysicsShell->GetGlobalTransformDynamic(&XFORM());
}

void CBreakableObject::net_Destroy()
{
	DestroyUnbroken();
	if(m_Shell)
	{
		m_Shell->Deactivate	();
		xr_delete			(m_Shell);
		SheduleUnregister	();
	}
	
	m_pPhysicsShell=NULL;
	inherited::net_Destroy();
	xr_delete(collidable.model);
	Init();
	//Visual()->vis.box.set(m_saved_box);
	Render->model_Delete(renderable.visual,TRUE);
	cNameVisual_set("");

}
void CBreakableObject::Split()
{
	//for (u16 k=0; k<K->LL_BoneCount(); k++){
	
		//		fMatrix4x4& M = K->LL_GetTransform(k);
		//		fMatrix4x4 R; R.setHPB(-0.1,-0.1,-0.1);
		//		M.mulB		(R);
		//		fMatrix4x4 S;	S.scale(0.98f,0.98f,0.98f);
		//		M.mulB		(S);
	//}
}

void CBreakableObject::Break()
{
	if(m_pPhysicsShell)return;
	DestroyUnbroken();
	CreateBroken();
	ActivateBroken();
	u16 el_num=m_pPhysicsShell->get_ElementsNumber();
	for(u16 i=0;i<el_num;i++)
	{
		fVector3 pos;
		fVector3 dir;
		pos.set(Random.randF(-0.3f,0.3f),Random.randF(-0.3f,0.3f),Random.randF(-0.3f,0.3f));
		dir.set(Random.randF(-0.3f,0.3f),Random.randF(-0.3f,0.3f),Random.randF(-0.3f,0.3f));
		dir.normalize();
		m_pPhysicsShell->get_ElementByStoreOrder(i)->applyImpulseTrace(pos,dir,Random.randF(0.5f,3.f),0);
	}

	m_break_time=Device.dwTimeGlobal;
	SheduleRegister	();
}

void CBreakableObject::SendDestroy()
{
	if (Local())	DestroyObject	();
//	CNetPacket		P;
//	u_EventGen		(P,GE_DESTROY,ID());
//	Msg				("ge_destroy: [%d] - %s",ID(),*cName());
//	if (Local())	u_EventSend			(P);
	bRemoved=true;
}

void CBreakableObject::ObjectContactCallback(bool&/**do_colide/**/,bool bo1,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/)
{
	dxGeomUserData* usr_data_1= retrieveGeomUserData(c.geom.g1);
	dxGeomUserData* usr_data_2=retrieveGeomUserData(c.geom.g2);
	CBreakableObject* this_object;
	dBodyID	body;
	f32 norm_sign;
	if(
		usr_data_1&&
		usr_data_1->ph_ref_object&&
		usr_data_1->ph_ref_object->CLS_ID == CLSID_OBJECT_BREAKABLE
		) {
				body=dGeomGetBody(c.geom.g2);
				if(!body) return;
				this_object=static_cast<CBreakableObject*>(usr_data_1->ph_ref_object);
				norm_sign=-1.f;
		}
	else if(
		usr_data_2&&
		usr_data_2->ph_ref_object&&
		usr_data_2->ph_ref_object->CLS_ID == CLSID_OBJECT_BREAKABLE
		){
				body=dGeomGetBody(c.geom.g1);
				if(!body) return;
				this_object=static_cast<CBreakableObject*>(usr_data_2->ph_ref_object);
				norm_sign=1.f;
		}
		else return;

	if(!this_object->m_pUnbrokenObject) return;
	f32 c_damage = E_NlS(body, c.geom.normal, norm_sign);
	if(this_object->m_damage_threshold<c_damage&&
		this_object->m_max_frame_damage<c_damage
		){
			this_object->b_resived_damage=true;
			this_object->m_max_frame_damage=c_damage;
			this_object->m_contact_damage_pos.set(c.geom.pos[0],c.geom.pos[1],c.geom.pos[2]);
			this_object->m_contact_damage_dir.set(-c.geom.normal[0]*norm_sign,-c.geom.normal[1]*norm_sign,-c.geom.normal[2]*norm_sign);
		}
}

void CBreakableObject::ProcessDamage()
{
	CNetPacket			P;
	SHit				HS;
	HS.GenHeader		(GE_HIT, ID());
	HS.whoID			= (ID());
	HS.weaponID			= (ID());
	HS.dir				= (m_contact_damage_dir);
	HS.power			= (m_max_frame_damage);
	HS.boneID			= (PKinematics(Visual())->LL_GetBoneRoot());
	HS.p_in_bone_space	= (m_contact_damage_pos);
	HS.impulse			= (0.0f);
	HS.hit_type			= (ALife::eHitTypeStrike);
	HS.Write_Packet		(P);
	
	u_EventSend			(P);

	m_max_frame_damage		= 0.0f;
	b_resived_damage		=false;
}

void CBreakableObject::CheckHitBreak(f32 power, ALife::EHitType hit_type)
{
	if (hit_type != ALife::eHitTypeStrike)
	{
		f32 res_power = power * m_immunity_factor;
		if (power > m_health_threshhold)
		{
			fHealth -= res_power;
		}
	}

	if (fHealth <= 0.0f)
	{
		Break( );
		return;
	}

	if (hit_type == ALife::eHitTypeStrike)
	{
		Break( );
	}
}

void CBreakableObject::ApplyExplosion(const fVector3& dir, f32 impulse)
{
	if (!m_pPhysicsShell)
	{
		return;
	}

	fVector3 pos;
	pos.set(0.0f, 0.0f, 0.0f);
	u16 el_num = m_pPhysicsShell->get_ElementsNumber( );
	for (u16 i = 0; i < el_num; i++)
	{
		fVector3 max_area_dir;
		CPhysicsElement* element = m_pPhysicsShell->get_ElementByStoreOrder(i);
		element->get_MaxAreaDir(max_area_dir);
		f32 sign = max_area_dir.dotproduct(dir) > 0.0f ? 1.0f : -1.0f;
		max_area_dir.mul(sign);
		element->applyImpulseTrace(pos, max_area_dir, impulse / el_num, 0);
	}
}

void CBreakableObject::Init()
{
	fHealth					= 1.0f;
	m_pUnbrokenObject		= NULL;
	m_Shell					= NULL;
	bRemoved				= false;
	m_max_frame_damage		= 0.0f;
	b_resived_damage		= false;
}
