//---------------------------------------------------------------------------
#include "stdafx.h"

#include "..\xrParticles\psystem.h"
#include "ParticleGroup.h"
#include "PSLibrary.h"

using namespace PS;

//------------------------------------------------------------------------------
CPGDef::CPGDef()
{
	m_Flags.zero	();
	m_fTimeLimit	= 0.f;
}

CPGDef::~CPGDef()
{
	for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++)
		xr_delete	(*it);
	m_Effects.clear	();
}

void CPGDef::SetName(pcstr name)
{
	m_Name			= name;
}

//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
BOOL CPGDef::Load(IReader& F)
{
	R_ASSERT		(F.find_chunk(PGD_CHUNK_VERSION));
	u16 version		= F.r_u16();

	if (version!=PGD_VERSION){
		Log			("!Unsupported PG version. Load failed.");
		return FALSE;
	}

	R_ASSERT		(F.find_chunk(PGD_CHUNK_NAME));
	F.r_stringZ		(m_Name);

	F.r_chunk		(PGD_CHUNK_FLAGS,&m_Flags);

	if (F.find_chunk(PGD_CHUNK_EFFECTS)){
		m_Effects.resize(F.r_u32());
		for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++){
			*it				= xr_new<SEffect>();
			F.r_stringZ		((*it)->m_EffectName);
			F.r_stringZ		((*it)->m_OnPlayChildName);
			F.r_stringZ		((*it)->m_OnBirthChildName);
			F.r_stringZ		((*it)->m_OnDeadChildName);
			(*it)->m_Time0 	= F.r_float();
			(*it)->m_Time1 	= F.r_float();
			(*it)->m_Flags.assign	(F.r_u32());
		}
	}else{  //.??? ������ ����� ��������� �����
		R_ASSERT		(F.find_chunk(PGD_CHUNK_EFFECTS2));
		m_Effects.resize(F.r_u32());
		for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++){
			*it				= xr_new<SEffect>();
			F.r_stringZ		((*it)->m_EffectName);
			F.r_stringZ		((*it)->m_OnPlayChildName);
			(*it)->m_Time0 	= F.r_float();
			(*it)->m_Time1 	= F.r_float();
			(*it)->m_Flags.assign	(F.r_u32());
		}
	}
	
	if (F.find_chunk(PGD_CHUNK_TIME_LIMIT)){
		m_fTimeLimit= F.r_float();
	}

	return TRUE;
}

void CPGDef::Save(IWriter& F)
{
	F.open_chunk	(PGD_CHUNK_VERSION);
	F.w_u16			(PGD_VERSION);
	F.close_chunk	();

	F.open_chunk	(PGD_CHUNK_NAME);
	F.w_stringZ		(m_Name);
	F.close_chunk	();

	F.w_chunk		(PGD_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));

	F.open_chunk	(PGD_CHUNK_EFFECTS);
	F.w_u32			(m_Effects.size());
	for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++){
		F.w_stringZ	((*it)->m_EffectName);
		F.w_stringZ	((*it)->m_OnPlayChildName);
		F.w_stringZ	((*it)->m_OnBirthChildName);
		F.w_stringZ	((*it)->m_OnDeadChildName);
		F.w_float	((*it)->m_Time0);
		F.w_float	((*it)->m_Time1);
		F.w_u32		((*it)->m_Flags.get());
	}
	F.close_chunk	();

	F.open_chunk	(PGD_CHUNK_TIME_LIMIT);
	F.w_float		(m_fTimeLimit);
	F.close_chunk	();
}

//------------------------------------------------------------------------------
// Particle Group item
//------------------------------------------------------------------------------
void CParticleGroup::SItem::Set(IRenderVisual* e)
{
	_effect=e;
}
void CParticleGroup::SItem::Clear()
{
	VisualVec 		visuals;
	GetVisuals		(visuals);
	for (VisualVecIt it=visuals.begin(); it!=visuals.end(); it++)
		::Render->model_Delete(*it);
}
void CParticleGroup::SItem::StartRelatedChild(CParticleEffect* emitter, pcstr eff_name, PAPI::Particle& m)
{
	CParticleEffect*C		= static_cast<CParticleEffect*>(RImplementation.model_CreatePE(eff_name));
	fMatrix4x4 M;
	M.identity();
	fVector3 vel; 
	vel.sub(m.pos,m.posB);
	vel.div(fDT_STEP);
	if (emitter->m_RT_Flags.is(CParticleEffect::flRT_XFORM)){
		M.set				(emitter->m_XFORM);
		M.transform_dir		(vel);
	}

	fVector3 				p;
	M.transform_tiny		(p,m.pos);
	M.c.set					(p);
	C->Play					();
	C->UpdateParent			(M,vel,FALSE);
	_children_related.push_back(C);
}
void CParticleGroup::SItem::StopRelatedChild(u32 idx)
{
	VERIFY(idx<_children_related.size());
	IRenderVisual*& V 			= _children_related[idx];
	((CParticleEffect*)V)->Stop	(TRUE);
	_children_free.push_back	(V);
	_children_related[idx]		= _children_related.back();
	_children_related.pop_back	();
}
void CParticleGroup::SItem::StartFreeChild(CParticleEffect* emitter, pcstr nm, PAPI::Particle& m)
{
	CParticleEffect*C			= static_cast<CParticleEffect*>(RImplementation.model_CreatePE(nm));
	if(!C->IsLooped()){
		fMatrix4x4 M;
		M.identity();
		fVector3 vel;
		vel.sub(m.pos,m.posB); vel.div(fDT_STEP);
		if (emitter->m_RT_Flags.is(CParticleEffect::flRT_XFORM)){
			M.set				(emitter->m_XFORM);
			M.transform_dir		(vel);
		}
		
		fVector3 				p;
		M.transform_tiny		(p,m.pos);
		M.c.set					(p);
		C->Play					();
		C->UpdateParent			(M,vel,FALSE);
		_children_free.push_back(C);
	}else{
		Debug.fatal	(DEBUG_INFO,"Can't use looped effect '%s' as 'On Birth' child for group.",nm);
	}
}
void CParticleGroup::SItem::Play()
{
	CParticleEffect* E	= static_cast<CParticleEffect*>(_effect);
	if (E) E->Play();
}
void CParticleGroup::SItem::Stop(BOOL def_stop)
{
	// stop all effects
	CParticleEffect* E	= static_cast<CParticleEffect*>(_effect);
	if (E) E->Stop(def_stop);
	VisualVecIt it;
	for (it=_children_related.begin(); it!=_children_related.end(); it++)
		static_cast<CParticleEffect*>(*it)->Stop(def_stop);
	for (it=_children_free.begin(); it!=_children_free.end(); it++)
		static_cast<CParticleEffect*>(*it)->Stop(def_stop);
	// and delete if !deffered
	if (!def_stop){
		for (it=_children_related.begin(); it!=_children_related.end(); it++)	::Render->model_Delete(*it);
		for (it=_children_free.begin(); it!=_children_free.end(); it++)			::Render->model_Delete(*it);
		_children_related.clear();
		_children_free.clear	();
	}
}
BOOL CParticleGroup::SItem::IsPlaying()
{
	CParticleEffect* E	= static_cast<CParticleEffect*>(_effect);
	return E?E->IsPlaying():FALSE;
}
void CParticleGroup::SItem::UpdateParent(const fMatrix4x4& m, const fVector3& velocity, BOOL bXFORM)
{
	CParticleEffect* E	= static_cast<CParticleEffect*>(_effect);
	if (E) E->UpdateParent(m,velocity,bXFORM);
}
//------------------------------------------------------------------------------
void OnGroupParticleBirth(void* owner, u32 param, PAPI::Particle& m, u32 idx)
{
	CParticleGroup* PG 	= static_cast<CParticleGroup*>(owner); 	VERIFY(PG);
	CParticleEffect*PE	= static_cast<CParticleEffect*>(PG->items[param]._effect);
	PS::OnEffectParticleBirth(PE, param, m, idx);
	// if have child
	const CPGDef* PGD			= PG->GetDefinition();					VERIFY(PGD);
	const CPGDef::SEffect* eff	= PGD->m_Effects[param];
	if (eff->m_Flags.is(CPGDef::SEffect::flOnBirthChild))
		PG->items[param].StartFreeChild			(PE,*eff->m_OnBirthChildName,m);
	if (eff->m_Flags.is(CPGDef::SEffect::flOnPlayChild))
		PG->items[param].StartRelatedChild		(PE,*eff->m_OnPlayChildName,m);
}
void OnGroupParticleDead(void* owner, u32 param, PAPI::Particle& m, u32 idx)
{
	CParticleGroup* PG 	= static_cast<CParticleGroup*>(owner); VERIFY(PG);
	CParticleEffect*PE	= static_cast<CParticleEffect*>(PG->items[param]._effect);
	PS::OnEffectParticleDead(PE, param, m, idx);
	// if have child
	const CPGDef* PGD			= PG->GetDefinition();					VERIFY(PGD);
	const CPGDef::SEffect* eff	= PGD->m_Effects[param];
	if (eff->m_Flags.is(CPGDef::SEffect::flOnPlayChild))
		PG->items[param].StopRelatedChild		(idx);
	if (eff->m_Flags.is(CPGDef::SEffect::flOnDeadChild))
		PG->items[param].StartFreeChild			(PE,*eff->m_OnDeadChildName,m);
}
//------------------------------------------------------------------------------
struct zero_vis_pred : public std::unary_function<IRenderVisual*, bool>
{
	bool operator()(const IRenderVisual* x){ return x==0; }
};
void CParticleGroup::SItem::OnFrame(u32 u_dt, const CPGDef::SEffect& def, fBox3& box, bool& bPlaying)
{
	CParticleEffect* E		= static_cast<CParticleEffect*>(_effect);
	if (E){
		E->OnFrame			(u_dt);
		if (E->IsPlaying()){
			bPlaying		= true;
			if (E->vis.box.is_valid())     box.merge	(E->vis.box);
			if (def.m_Flags.is(CPGDef::SEffect::flOnPlayChild)&&def.m_OnPlayChildName.size()){
				PAPI::Particle* particles;
				u32 p_cnt;
				PAPI::ParticleManager()->GetParticles(E->GetHandleEffect(),particles,p_cnt);
				VERIFY(p_cnt==_children_related.size());
				if (p_cnt){
					for(u32 i = 0; i < p_cnt; i++){
						PAPI::Particle &m	= particles[i]; 
						CParticleEffect* C 	= static_cast<CParticleEffect*>(_children_related[i]);
						fMatrix4x4 M;
						M.translate(m.pos);
						fVector3 vel; 
						vel.sub(m.pos,m.posB);
						vel.div(fDT_STEP);
						C->UpdateParent		(M,vel,FALSE);
					}
				}
			}
		}
	}

	VisualVecIt it;
	if (!_children_related.empty()){
		for (it=_children_related.begin(); it!=_children_related.end(); it++){
			CParticleEffect* E	= static_cast<CParticleEffect*>(*it);
			if (E){
				E->OnFrame		(u_dt);
				if (E->IsPlaying()){
					bPlaying	= true;
					if (E->vis.box.is_valid())     box.merge	(E->vis.box);
				}else{
					if (def.m_Flags.is(CPGDef::SEffect::flOnPlayChildRewind)){
						E->Play	();
					}
				}
			}
		}
	}
	if (!_children_free.empty()){
		u32 rem_cnt				= 0;
		for (it=_children_free.begin(); it!=_children_free.end(); it++){
			CParticleEffect* E	= static_cast<CParticleEffect*>(*it);
			if (E){
				E->OnFrame		(u_dt);
				if (E->IsPlaying()){ 
					bPlaying	= true;
					if (E->vis.box.is_valid()) box.merge	(E->vis.box);
				}else{
					rem_cnt++	;
					::Render->model_Delete(*it);
				}
			}
		}
		// remove if stopped
		if (rem_cnt){
			VisualVecIt new_end=std::remove_if(_children_free.begin(),_children_free.end(),zero_vis_pred());
			_children_free.erase(new_end,_children_free.end());
		}
	}
//	Msg("C: %d CS: %d",_children.size(),_children_stopped.size());
}
void CParticleGroup::SItem::OnDeviceCreate()
{
	VisualVec 		visuals;
	GetVisuals		(visuals);
	for (VisualVecIt it=visuals.begin(); it!=visuals.end(); it++)
		static_cast<CParticleEffect*>(*it)->OnDeviceCreate();
}
void CParticleGroup::SItem::OnDeviceDestroy()
{
	VisualVec 		visuals;
	GetVisuals		(visuals);
	for (VisualVecIt it=visuals.begin(); it!=visuals.end(); it++)
		static_cast<CParticleEffect*>(*it)->OnDeviceDestroy();
}
u32	CParticleGroup::SItem::ParticlesCount()
{
	u32 p_count=0;
	VisualVec 		visuals;
	GetVisuals		(visuals);
	for (VisualVecIt it=visuals.begin(); it!=visuals.end(); it++)
		p_count		+= static_cast<CParticleEffect*>(*it)->ParticlesCount();
	return p_count;
}


//------------------------------------------------------------------------------
// Particle Group part
//------------------------------------------------------------------------------
CParticleGroup::CParticleGroup()
{
	m_RT_Flags.zero			();
	m_InitialPosition.set	(0,0,0);
}

CParticleGroup::~CParticleGroup()
{
	// Msg ("!!! destoy PG");
	for (u32 i=0; i<items.size(); i++) items[i].Clear();
	items.clear();
}

void CParticleGroup::OnFrame(u32 u_dt)
{
	if (m_Def&&m_RT_Flags.is(flRT_Playing)){
		f32 ct	= m_CurrentTime;
		f32 f_dt	= f32(u_dt)/1000.0f;
		for (CPGDef::EffectVec::const_iterator e_it=m_Def->m_Effects.begin(); e_it!=m_Def->m_Effects.end(); e_it++){	
			if ((*e_it)->m_Flags.is(CPGDef::SEffect::flEnabled)){
				VERIFY				(items.size()==m_Def->m_Effects.size());
				SItem& I			= items[e_it-m_Def->m_Effects.begin()];
				if (I.IsPlaying()){
					if ((ct<=(*e_it)->m_Time1)&&(ct+f_dt>=(*e_it)->m_Time1))	
						I.Stop((*e_it)->m_Flags.is(CPGDef::SEffect::flDefferedStop));
				}else{
					if (!m_RT_Flags.is(flRT_DefferedStop))
						if ((ct<=(*e_it)->m_Time0)&&(ct+f_dt>=(*e_it)->m_Time0))	
							I.Play();
				}
			}
		}
		m_CurrentTime 	+= f_dt;
		if ((m_CurrentTime>m_Def->m_fTimeLimit)&&(m_Def->m_fTimeLimit>0.f))
			if (!m_RT_Flags.is(flRT_DefferedStop)) Stop(true);

		bool bPlaying = false;
		fBox3 box;
		box.invalidate();
		for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) 
			i_it->OnFrame(u_dt,*m_Def->m_Effects[i_it-items.begin()],box,bPlaying);

		if (m_RT_Flags.is(flRT_DefferedStop)&&!bPlaying){
			m_RT_Flags.set		(flRT_Playing|flRT_DefferedStop,FALSE);
		}
		if (box.is_valid()){
			vis.box.set			(box);
			vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
		}
	} else {
		vis.box.set			(m_InitialPosition,m_InitialPosition);
		vis.box.grow		(EPSILON_3);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	}
}

void CParticleGroup::UpdateParent(const fMatrix4x4& m, const fVector3& velocity, BOOL bXFORM)
{
	m_InitialPosition		= m.c;
	for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) 
		i_it->UpdateParent(m,velocity,bXFORM);
}

BOOL CParticleGroup::Compile(CPGDef* def)
{
	m_Def 						= def;
	// destroy existing
	for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) 
		i_it->Clear();
	items.clear();
	// create new
	if (m_Def){
		items.resize			(m_Def->m_Effects.size());
		for (CPGDef::EffectVec::const_iterator e_it=m_Def->m_Effects.begin(); e_it!=m_Def->m_Effects.end(); e_it++){
			CParticleEffect* eff = (CParticleEffect*)RImplementation.model_CreatePE(*(*e_it)->m_EffectName);
			eff->SetBirthDeadCB	(OnGroupParticleBirth,OnGroupParticleDead,this,u32(e_it-m_Def->m_Effects.begin()));
			items[e_it-def->m_Effects.begin()].Set(eff);
		}
	}
	return TRUE;
}

void CParticleGroup::Play()
{
	m_CurrentTime   = 0;
	m_RT_Flags.set	(flRT_DefferedStop,FALSE);
	m_RT_Flags.set 	(flRT_Playing,TRUE);
}

void CParticleGroup::Stop(BOOL bDefferedStop)
{
	if (bDefferedStop){
		m_RT_Flags.set	(flRT_DefferedStop,TRUE);
	}else{
		m_RT_Flags.set	(flRT_Playing,FALSE);
	}
	for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) i_it->Stop(bDefferedStop);
}

void CParticleGroup::OnDeviceCreate()
{
	for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) i_it->OnDeviceCreate();
}

void CParticleGroup::OnDeviceDestroy()
{
	for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++) i_it->OnDeviceDestroy();
}

u32 CParticleGroup::ParticlesCount()
{
	int p_count=0;
	for (SItemVecIt i_it=items.begin(); i_it!=items.end(); i_it++)
		p_count 	+= i_it->ParticlesCount();
	return p_count;
}
