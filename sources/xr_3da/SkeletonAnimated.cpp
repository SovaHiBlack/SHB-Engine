#include "stdafx.h"

#include "SkeletonAnimated.h"
#include "mesh.h"

extern s32	psSkeletonUpdate;

//////////////////////////////////////////////////////////////////////////
// BoneInstance methods
void		CBlendInstance::construct( )
{
	ZeroMemory(this, sizeof(*this));
}

void		CBlendInstance::blend_add(CBlend* H)
{
	if (Blend.size( ) == MAX_BLENDED)
	{
		if (H->fall_at_end)
		{
			return;
		}

		BlendSVecIt _d = Blend.begin( );
		for (BlendSVecIt it = Blend.begin( ) + 1; it != Blend.end( ); it++)
		{
			if ((*it)->blendAmount < (*_d)->blendAmount)
			{
				_d = it;
			}
		}

		Blend.erase(_d);
	}

	VERIFY(Blend.size( ) < MAX_BLENDED);
	Blend.push_back(H);
}

void		CBlendInstance::blend_remove(CBlend* H)
{
	CBlend** I = std::find(Blend.begin( ), Blend.end( ), H);
	if (I != Blend.end( ))
	{
		Blend.erase(I);
	}
}

// Motion control
void CKinematicsAnimated::Bone_Motion_Start(CBoneData* bd, CBlend* handle)
{
	LL_GetBlendInstance(bd->GetSelfID( )).blend_add(handle);
	for (vecBonesIt I = bd->children.begin( ); I != bd->children.end( ); I++)
	{
		Bone_Motion_Start(*I, handle);
	}
}

void CKinematicsAnimated::Bone_Motion_Stop(CBoneData* bd, CBlend* handle)
{
	LL_GetBlendInstance(bd->GetSelfID( )).blend_remove(handle);
	for (vecBonesIt I = bd->children.begin( ); I != bd->children.end( ); I++)
	{
		Bone_Motion_Stop(*I, handle);
	}
}

void CKinematicsAnimated::Bone_Motion_Start_IM(CBoneData* bd, CBlend* handle)
{
	LL_GetBlendInstance(bd->GetSelfID( )).blend_add(handle);
}

void CKinematicsAnimated::Bone_Motion_Stop_IM(CBoneData* bd, CBlend* handle)
{
	LL_GetBlendInstance(bd->GetSelfID( )).blend_remove(handle);
}

#ifdef DEBUG
std::pair<pcstr, pcstr> CKinematicsAnimated::LL_MotionDefName_dbg(MotionID ID)
{
	shared_motions& s_mots = m_Motions[ID.slot].motions;
	accel_map::iterator _I, _E = s_mots.motion_map( )->end( );
	for (_I = s_mots.motion_map( )->begin( ); _I != _E; ++_I)
	{
		if (_I->second == ID.idx)
		{
			return std::make_pair(*_I->first, *s_mots.id( ));
		}
	}

	return std::make_pair((pcstr)0, (pcstr)0);
}
#endif // DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MotionID CKinematicsAnimated::LL_MotionID(pcstr B)
{
	MotionID motion_ID;
	for (s32 k = s32(m_Motions.size( )) - 1; k >= 0; --k)
	{
		shared_motions* s_mots = &m_Motions[k].motions;
		accel_map::iterator I = s_mots->motion_map( )->find(LPSTR(B));
		if (I != s_mots->motion_map( )->end( ))
		{
			motion_ID.set(u16(k), I->second);
			break;
		}
	}

	return motion_ID;
}

u16 CKinematicsAnimated::LL_PartID(pcstr B)
{
	if (0 == m_Partition)
	{
		return BI_NONE;
	}

	for (u16 id = 0; id < MAX_PARTS; id++)
	{
		CPartDef& P = (*m_Partition)[id];
		if (0 == P.Name)
		{
			continue;
		}

		if (0 == stricmp(B, *P.Name))
		{
			return id;
		}
	}

	return BI_NONE;
}

// cycles
MotionID CKinematicsAnimated::ID_Cycle_Safe(pcstr  N)
{
	MotionID motion_ID;
	for (s32 k = s32(m_Motions.size( )) - 1; k >= 0; --k)
	{
		shared_motions* s_mots = &m_Motions[k].motions;
		accel_map::iterator I = s_mots->cycle( )->find(LPSTR(N));
		if (I != s_mots->cycle( )->end( ))
		{
			motion_ID.set(u16(k), I->second);
			break;
		}
	}

	return motion_ID;
}

MotionID CKinematicsAnimated::ID_Cycle(shared_str  N)
{
	MotionID motion_ID = ID_Cycle_Safe(N);
	R_ASSERT3(motion_ID.valid( ), "! MODEL: can't find cycle: ", N.c_str( ));
	return motion_ID;
}

MotionID CKinematicsAnimated::ID_Cycle_Safe(shared_str  N)
{
	MotionID motion_ID;
	for (s32 k = s32(m_Motions.size( )) - 1; k >= 0; --k)
	{
		shared_motions* s_mots = &m_Motions[k].motions;
		accel_map::iterator I = s_mots->cycle( )->find(N);
		if (I != s_mots->cycle( )->end( ))
		{
			motion_ID.set(u16(k), I->second);
			break;
		}
	}

	return motion_ID;
}

MotionID CKinematicsAnimated::ID_Cycle(pcstr  N)
{
	MotionID motion_ID = ID_Cycle_Safe(N);
	R_ASSERT3(motion_ID.valid( ), "! MODEL: can't find cycle: ", N);
	return motion_ID;
}

void CKinematicsAnimated::LL_FadeCycle(u16 part, f32 falloff, u8 mask_channel /*= (1<<0)*/)
{
	BlendSVec& Blend = blend_cycles[part];
	for (u32 I = 0; I < Blend.size( ); I++)
	{
		CBlend& B = *Blend[I];
		if (!(mask_channel & (1 << B.channel)))
		{
			continue;
		}

		B.blend = CBlend::eFalloff;
		B.blendFalloff = falloff;
		if (B.stop_at_end)
		{	// callback не должен приходить!
			B.playing = FALSE;
		}
	}
}

void CKinematicsAnimated::LL_CloseCycle(u16 part, u8 mask_channel /*= (1<<0)*/)
{
	if (BI_NONE == part)
	{
		return;
	}

	if (part >= MAX_PARTS)
	{
		return;
	}

	// destroy cycle(s)
	BlendSVecIt	I = blend_cycles[part].begin( ), E = blend_cycles[part].end( );
	for (; I != E; I++)
	{
		CBlend& B = *(*I);
		if (!(mask_channel & (1 << B.channel)))
		{
			continue;
		}

		B.blend = CBlend::eFREE_SLOT;

		CPartDef& P = (*m_Partition)[B.bone_or_part];
		for (u32 i = 0; i < P.bones.size( ); i++)
		{
			Bone_Motion_Stop_IM((*bones)[P.bones[i]], *I);
		}

		blend_cycles[part].erase(I);// ?
		E = blend_cycles[part].end( ); I--;
	}
	//blend_cycles[part].clear	(); // ?
}

void CKinematicsAnimated::IBlendSetup(CBlend& B, u16 part, u8 channel, MotionID motion_ID, BOOL  bMixing, f32 blendAccrue, f32 blendFalloff, f32 Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam)
{
	VERIFY(B.channel < MAX_CHANNELS);
	// Setup blend params
	if (bMixing)
	{
		B.blend = CBlend::eAccrue;
		B.blendAmount = EPSILON_7;
	}
	else
	{
		//B.blend		= CBlend::eFixed;
		B.blend = CBlend::eAccrue;
		B.blendAmount = 1;
	}

	B.blendAccrue = blendAccrue;
	B.blendFalloff = 0; // blendFalloff used for previous cycles
	B.blendPower = 1;
	B.speed = Speed;
	B.motionID = motion_ID;
	B.timeCurrent = 0;
	B.timeTotal = m_Motions[B.motionID.slot].bone_motions[LL_GetBoneRoot( )]->at(motion_ID.idx).GetLength( );
	B.bone_or_part = part;
	B.stop_at_end = noloop;
	B.playing = TRUE;
	B.Callback = Callback;
	B.CallbackParam = CallbackParam;

	B.channel = channel;
	B.fall_at_end = B.stop_at_end && (channel != 0);
}

void CKinematicsAnimated::IFXBlendSetup(CBlend& B, MotionID motion_ID, f32 blendAccrue, f32 blendFalloff, f32 Power, f32 Speed, u16 bone)
{
	B.blend = CBlend::eAccrue;
	B.blendAmount = EPSILON_7;
	B.blendAccrue = blendAccrue;
	B.blendFalloff = blendFalloff;
	B.blendPower = Power;
	B.speed = Speed;
	B.motionID = motion_ID;
	B.timeCurrent = 0;
	B.timeTotal = m_Motions[B.motionID.slot].bone_motions[bone]->at(motion_ID.idx).GetLength( );
	B.bone_or_part = bone;

	B.playing = TRUE;
	B.stop_at_end = FALSE;
	//
	B.Callback = 0;
	B.CallbackParam = 0;

	B.channel = 0;
	B.fall_at_end = FALSE;
}

CBlend* CKinematicsAnimated::LL_PlayCycle(u16 part, MotionID motion_ID, BOOL  bMixing, f32 blendAccrue, f32 blendFalloff, f32 Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam, u8 channel/*=0*/)
{
	// validate and unroll
	if (!motion_ID.valid( ))
	{
		return 0;
	}

	if (BI_NONE == part)
	{
		for (u16 i = 0; i < MAX_PARTS; i++)
		{
			LL_PlayCycle(i, motion_ID, bMixing, blendAccrue, blendFalloff, Speed, noloop, Callback, CallbackParam, channel);
		}

		return 0;
	}

	if (part >= MAX_PARTS)
	{
		return 0;
	}

	if (0 == m_Partition->part(part).Name)
	{
		return 0;
	}

//	shared_motions* s_mots	= &m_Motions[motion.slot];
//	CMotionDef* m_def		= s_mots->motion_def(motion.idx);

	// Process old cycles and create _new_
	if (channel == 0)
	{
		_DBG_SINGLE_USE_MARKER;
		if (bMixing)
		{
			LL_FadeCycle(part, blendFalloff, 1 << channel);
		}
		else
		{
			LL_CloseCycle(part, 1 << channel);
		}
	}

	CPartDef& P = (*m_Partition)[part];
	CBlend* B = IBlend_Create( );

	_DBG_SINGLE_USE_MARKER;
	IBlendSetup(*B, part, channel, motion_ID, bMixing, blendAccrue, blendFalloff, Speed, noloop, Callback, CallbackParam);
	for (u32 i = 0; i < P.bones.size( ); i++)
	{
		Bone_Motion_Start_IM((*bones)[P.bones[i]], B);
	}

	blend_cycles[part].push_back(B);
	return B;
}

CBlend* CKinematicsAnimated::LL_PlayCycle(u16 part, MotionID motion_ID, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel /*=0*/)
{
	VERIFY(motion_ID.valid( ));
	CMotionDef* m_def = m_Motions[motion_ID.slot].motions.motion_def(motion_ID.idx);
	VERIFY(m_def);
	return LL_PlayCycle(part, motion_ID, bMixIn, m_def->Accrue( ), m_def->Falloff( ), m_def->Speed( ), m_def->StopAtEnd( ), Callback, CallbackParam, channel);
}

CBlend* CKinematicsAnimated::PlayCycle(pcstr  N, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel  /*= 0*/)
{
	MotionID motion_ID = ID_Cycle(N);
	if (motion_ID.valid( ))
	{
		return PlayCycle(motion_ID, bMixIn, Callback, CallbackParam, channel);
	}
	else
	{
		Debug.fatal(DEBUG_INFO, "! MODEL: can't find cycle: %s", N); return 0;
	}
}

CBlend* CKinematicsAnimated::PlayCycle(MotionID motion_ID, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel /*= 0*/)
{
	VERIFY(motion_ID.valid( ));
	CMotionDef* m_def = m_Motions[motion_ID.slot].motions.motion_def(motion_ID.idx);
	VERIFY(m_def);
	return LL_PlayCycle(m_def->bone_or_part, motion_ID, bMixIn, m_def->Accrue( ), m_def->Falloff( ), m_def->Speed( ), m_def->StopAtEnd( ), Callback, CallbackParam, channel);
}

// fx'es
MotionID CKinematicsAnimated::ID_FX_Safe(pcstr  N)
{
	MotionID motion_ID;
	for (s32 k = s32(m_Motions.size( )) - 1; k >= 0; --k)
	{
		shared_motions* s_mots = &m_Motions[k].motions;
		accel_map::iterator I = s_mots->fx( )->find(LPSTR(N));
		if (I != s_mots->fx( )->end( ))
		{
			motion_ID.set(u16(k), I->second);
			break;
		}
	}

	return motion_ID;
}

MotionID CKinematicsAnimated::ID_FX(pcstr  N)
{
	MotionID motion_ID = ID_FX_Safe(N);
	R_ASSERT3(motion_ID.valid( ), "! MODEL: can't find FX: ", N);
	return motion_ID;
}

CBlend* CKinematicsAnimated::PlayFX(MotionID motion_ID, f32 power_scale)
{
	VERIFY(motion_ID.valid( ));
	CMotionDef* m_def = m_Motions[motion_ID.slot].motions.motion_def(motion_ID.idx);
	VERIFY(m_def);
	return LL_PlayFX(m_def->bone_or_part, motion_ID, m_def->Accrue( ), m_def->Falloff( ), m_def->Speed( ), m_def->Power( ) * power_scale);
}

CBlend* CKinematicsAnimated::PlayFX(pcstr  N, f32 power_scale)
{
	MotionID motion_ID = ID_FX(N);
	return PlayFX(motion_ID, power_scale);
}
//u16 part,u8 channel, MotionID motion_ID, BOOL  bMixing, f32 blendAccrue, f32 blendFalloff, f32 Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam)

CBlend* CKinematicsAnimated::LL_PlayFX(u16 bone, MotionID motion_ID, f32 blendAccrue, f32 blendFalloff, f32 Speed, f32 Power)
{
	if (!motion_ID.valid( ))
	{
		return 0;
	}

	if (blend_fx.size( ) >= MAX_BLENDED)
	{
		return 0;
	}

	if (BI_NONE == bone)
	{
		bone = iRoot;
	}

	CBlend* B = IBlend_Create( );
	_DBG_SINGLE_USE_MARKER;
	IFXBlendSetup(*B, motion_ID, blendAccrue, blendFalloff, Power, Speed, bone);
	Bone_Motion_Start((*bones)[bone], B);

	blend_fx.push_back(B);
	return B;
}

void CKinematicsAnimated::DestroyCycle(CBlend& B)
{
	B.blend = CBlend::eFREE_SLOT;
	CPartDef& P = m_Partition->part(B.bone_or_part);
	for (u32 i = 0; i < P.bones.size( ); i++)
	{
		Bone_Motion_Stop_IM((*bones)[P.bones[i]], &B);
	}
}

IC void UpdateBlendTime(CBlend& B, f32 dt)
{
	if (B.playing)
	{
		B.timeCurrent += dt * B.speed; // stop@end - time is not going 
	}
}

//returns true if play time out
IC bool UpdatePlayBlend(CBlend& B, f32 dt)
{
	B.blendAmount += dt * B.blendAccrue * B.blendPower;

	if (B.blendAmount > B.blendPower)
	{
		B.blendAmount = B.blendPower;
	}

	if (B.stop_at_end && (B.timeCurrent > (B.timeTotal - SAMPLE_SPF/*-EPSILON_5*/)))
	{
		B.timeCurrent = B.timeTotal - SAMPLE_SPF /*- EPSILON_5*/;		// stop@end - time frozen at the end
		if (B.playing && B.Callback)
		{	// callback only once
			B.Callback(&B);
		}

		B.playing = FALSE;
		return true;
	}

	return false;
}

IC bool UpdateFalloffBlend(CBlend& B, f32 dt)
{
	B.blendAmount -= dt * B.blendFalloff * B.blendPower;
	return (B.blendAmount <= 0);
}

void CKinematicsAnimated::UpdateTracks( )
{
	_DBG_SINGLE_USE_MARKER;
	if (Update_LastTime == Device.dwTimeGlobal)
	{
		return;
	}

	u32 DT = Device.dwTimeGlobal - Update_LastTime;
	if (DT > 66)
	{
		DT = 66;
	}

	f32 dt = f32(DT) / 1000.0f;
	Update_LastTime = Device.dwTimeGlobal;

	BlendSVecIt I, E;

	// Cycles
	for (u16 part = 0; part < MAX_PARTS; part++)
	{
		if (0 == m_Partition->part(part).Name)
		{
			continue;
		}

		I = blend_cycles[part].begin( ); E = blend_cycles[part].end( );
		for (; I != E; I++)
		{
			CBlend& B = *(*I);
			if (B.dwFrame == Device.dwFrame)
			{
				continue;
			}

			B.dwFrame = Device.dwFrame;
			UpdateBlendTime(B, dt);
			switch (B.blend)
			{
				case CBlend::eFREE_SLOT:
				{
					NODEFAULT;
				}
	/*
				case CBlend::eFixed:
					if(UpdatePlayBlend(B,dt)&&B.fall_at_end)
					{
							B.blend = CBlend::eFalloff;
							B.blendFalloff = 2.f;
					}
					break;
	*/
				case CBlend::eAccrue:
				{
					if (UpdatePlayBlend(B, dt))
					{
						if (B.fall_at_end)
						{
							B.blend = CBlend::eFalloff;
							B.blendFalloff = 2.0f;
						}
						//else if(B.blendAmount==B.blendPower)
							//B.blend			= CBlend::eFixed;
					}
				}
				break;
				case CBlend::eFalloff:
				{
					if (UpdateFalloffBlend(B, dt))
					{
						DestroyCycle(B);
						blend_cycles[part].erase(I);
						E = blend_cycles[part].end( );
						I--;
					}
				}
				break;
				default:
				{
					NODEFAULT;
				}
			}
		}
	}

	// FX
	I = blend_fx.begin( ); E = blend_fx.end( );
	for (; I != E; I++)
	{
		CBlend& B = *(*I);
		if (!B.playing)
		{
			continue;
		}

		B.timeCurrent += dt * B.speed;
		switch (B.blend)
		{
			case CBlend::eFREE_SLOT:
			{
				NODEFAULT;
			}
	/*
			case CBlend::eFixed:
				{
	//				B.blendAmount = B.blendPower;
	//				// calc time to falloff
	//				f32 time2falloff = B.timeTotal - 1/(B.blendFalloff*B.speed);
	//				if (B.timeCurrent >= time2falloff) {
	//					// switch to falloff
	//					B.blend		= CBlend::eFalloff;
	//				}
	//
					B.blend		= CBlend::eFalloff;
				}
				break;
	*/
			case CBlend::eAccrue:
			{
				B.blendAmount += dt * B.blendAccrue * B.blendPower * B.speed;
				if (B.blendAmount >= B.blendPower)
				{
					// switch to fixed
					B.blendAmount = B.blendPower;
					B.blend = CBlend::eFalloff;//CBlend::eFixed;
				}
			}
			break;
			case CBlend::eFalloff:
			{
				B.blendAmount -= dt * B.blendFalloff * B.blendPower * B.speed;
				if (B.blendAmount <= 0)
				{
					// destroy fx
					B.blend = CBlend::eFREE_SLOT;
					Bone_Motion_Stop((*bones)[B.bone_or_part], *I);
					blend_fx.erase(I);
					E = blend_fx.end( );
					I--;
				}
			}
			break;
			default:
			{
				NODEFAULT;
			}
		}
	}
}

void CKinematicsAnimated::Release( )
{
	// xr_free bones
//.	for (u32 i=0; i<bones->size(); i++)
//.	{
//.		CBoneDataAnimated* B	= (CBoneDataAnimated*)(*bones)[i];
//.		B->Motions.clear		();
//.	}

	// destroy shared data
//.	xr_delete(partition);
//.	xr_delete(motion_map);
//.	xr_delete(m_cycle);
//.	xr_delete(m_fx);

	inherited::Release( );
}

CKinematicsAnimated::~CKinematicsAnimated( )
{
	IBoneInstances_Destroy( );
}

void CKinematicsAnimated::IBoneInstances_Create( )
{
	inherited::IBoneInstances_Create( );
	u32 size = bones->size( );
	blend_instances = xr_alloc<CBlendInstance>(size);
	for (u32 i = 0; i < size; i++)
	{
		blend_instances[i].construct( );
	}
}

void CKinematicsAnimated::IBoneInstances_Destroy( )
{
	inherited::IBoneInstances_Destroy( );
	if (blend_instances)
	{
		xr_free(blend_instances);
		blend_instances = NULL;
	}
}

#define PCOPY(a)	a = pFrom->a
void CKinematicsAnimated::Copy(IRenderVisual* P)
{
	inherited::Copy(P);

	CKinematicsAnimated* pFrom = (CKinematicsAnimated*)P;
	PCOPY(m_Motions);
	PCOPY(m_Partition);

	IBlend_Startup( );
}

void CKinematicsAnimated::Spawn( )
{
	inherited::Spawn( );

	IBlend_Startup( );

	for (u32 i = 0; i < bones->size( ); i++)
	{
		blend_instances[i].construct( );
	}
}

void CKinematicsAnimated::ChannelFactorsStartup( )
{
	for (u8 i = 0; MAX_CHANNELS > i; ++i)
	{
		channel_factors[i] = 1.0f;
	}
}

void CKinematicsAnimated::LL_SetChannelFactor(u16 channel, f32 factor)
{
	channel_factors[channel] = factor;
}

void CKinematicsAnimated::IBlend_Startup( )
{
	_DBG_SINGLE_USE_MARKER;
	CBlend B; ZeroMemory(&B, sizeof(B));
	B.blend = CBlend::eFREE_SLOT;
	blend_pool.clear( );
	for (u32 i = 0; i < MAX_BLENDED_POOL; i++)
	{
		blend_pool.push_back(B);
	}

	// cycles+fx clear
	for (i = 0; i < MAX_PARTS; i++)
	{
		blend_cycles[i].clear( );
	}

	blend_fx.clear( );
	ChannelFactorsStartup( );
}

CBlend* CKinematicsAnimated::IBlend_Create( )
{
	UpdateTracks( );
	_DBG_SINGLE_USE_MARKER;
	CBlend* I = blend_pool.begin( ), * E = blend_pool.end( );
	for (; I != E; I++)
	{
		if (I->blend == CBlend::eFREE_SLOT)
		{
			return I;
		}
	}

	FATAL("Too many blended motions requisted");
	return 0;
}

void CKinematicsAnimated::Load(pcstr N, IReader* data, u32 dwFlags)
{
	inherited::Load(N, data, dwFlags);

	// Globals
	blend_instances = NULL;
	m_Partition = NULL;
	Update_LastTime = 0;

	// Load animation
	if (data->find_chunk(OGF_S_MOTION_REFS))
	{
		string_path items_nm;
		data->r_stringZ(items_nm, sizeof(items_nm));
		u32 set_cnt = _GetItemCount(items_nm);
		R_ASSERT(set_cnt < MAX_ANIM_SLOT);
		m_Motions.reserve(set_cnt);
		string_path nm;
		for (u32 k = 0; k < set_cnt; k++)
		{
			_GetItem(items_nm, k, nm);
			strcat(nm, ".omf");
			string_path	fn;
			if (!FS.exist(fn, "$level$", nm))
			{
				if (!FS.exist(fn, "$game_meshes$", nm))
				{
					Debug.fatal(DEBUG_INFO, "Can't find motion file '%s'.", nm);
				}
			}

			// Check compatibility
			m_Motions.push_back(SMotionsSlot( ));
			if (!g_pMotionsContainer->has(nm)) //optimize fs operations
			{
				IReader* MS = FS.r_open(fn);
				m_Motions.back( ).motions.create(nm, MS, bones);
				FS.r_close(MS);
			}

			m_Motions.back( ).motions.create(nm, NULL, bones);
		}
	}
	else
	{
		string_path nm;
		strconcat(sizeof(nm), nm, N, ".ogf");
		m_Motions.push_back(SMotionsSlot( ));
		m_Motions.back( ).motions.create(nm, data, bones);
	}

	R_ASSERT(m_Motions.size( ));

	m_Partition = m_Motions[0].motions.partition( );

	// initialize motions
	for (MotionsSlotVecIt m_it = m_Motions.begin( ); m_it != m_Motions.end( ); m_it++)
	{
		SMotionsSlot& MS = *m_it;
		MS.bone_motions.resize(bones->size( ));
		for (u32 i = 0; i < bones->size( ); i++)
		{
			CBoneData* BD = (*bones)[i];
			MS.bone_motions[i] = MS.motions.bone_motions(BD->name);
		}
	}

	// Init blend pool
	IBlend_Startup( );

//.	if (motions.cycle()->size()<2)			
//.		Msg("* WARNING: model '%s' has only one motion. Candidate for SkeletonRigid???",N);
}

//------------------------------------------------------------------------------
// calculate
//------------------------------------------------------------------------------
IC void	KEY_Interp(CKey& D, const CKey& K1, const CKey& K2, f32 delta)
{
	VERIFY(_valid(delta));
	VERIFY(delta >= 0.0f && delta <= 1.0f);
	D.Q.slerp(K1.Q, K2.Q, delta);
	D.T.lerp(K1.T, K2.T, delta);
}

struct ConsistantKey
{
	const CKey* K;
	f32	w;

	IC void	set(const CKey* _K, f32 _w)
	{
		K = _K;
		w = _w;
	}
};

IC bool operator < (const ConsistantKey& A, const ConsistantKey& B)	// note: inverse operator
{
	return (A.w > B.w);
}

IC void MakeKeysSelected(ConsistantKey* keys, s32 count)
{
	// sort in decreasing order
	std::sort(keys, keys + count);
}

IC	void QR2Quat(const CKeyQR& K, fQuaternion& Q)
{
	Q.x = f32(K.x) * KEY_QuantI;
	Q.y = f32(K.y) * KEY_QuantI;
	Q.z = f32(K.z) * KEY_QuantI;
	Q.w = f32(K.w) * KEY_QuantI;
}

IC void QT2T(const CKeyQT& K, const CMotion& M, fVector3& T)
{
	T.x = f32(K.x) * M._sizeT.x + M._initT.x;
	T.y = f32(K.y) * M._sizeT.y + M._initT.y;
	T.z = f32(K.z) * M._sizeT.z + M._initT.z;
}

IC void Dequantize(CKey& K, const CBlend& BD, const CMotion& M)
{
	CKey* D = &K;
	const CBlend* B = &BD;
	f32 time = B->timeCurrent * f32(SAMPLE_FPS);

	u32 frame = iFloor(time);
	f32 delta = time - f32(frame);
	u32 count = M.get_count( );
	// rotation
	if (M.test_flag(flRKeyAbsent))
	{
		const CKeyQR* K = &M._keysR[0];
		QR2Quat(*K, D->Q);
	}
	else
	{
		const CKeyQR* K1r = &M._keysR[(frame + 0) % count];
		const CKeyQR* K2r = &M._keysR[(frame + 1) % count];
		fQuaternion	Q1;
		fQuaternion Q2;
		QR2Quat(*K1r, Q1);
		QR2Quat(*K2r, Q2);
		D->Q.slerp(Q1, Q2, clampr(delta, 0.0f, 1.0f));
	}

	// translate
	if (M.test_flag(flTKeyPresent))
	{
		const CKeyQT* K1t = &M._keysT[(frame + 0) % count];
		const CKeyQT* K2t = &M._keysT[(frame + 1) % count];

		fVector3 T1;
		fVector3 T2;
		QT2T(*K1t, M, T1);
		QT2T(*K2t, M, T2);
		/*
		T1.x		= f32(K1t->x)*M._sizeT.x+M._initT.x;
		T1.y		= f32(K1t->y)*M._sizeT.y+M._initT.y;
		T1.z		= f32(K1t->z)*M._sizeT.z+M._initT.z;

		T2.x		= f32(K2t->x)*M._sizeT.x+M._initT.x;
		T2.y		= f32(K2t->y)*M._sizeT.y+M._initT.y;
		T2.z		= f32(K2t->z)*M._sizeT.z+M._initT.z;
		*/
		D->T.lerp(T1, T2, delta);
		/*
		if ((_abs(D->T.y)>10000) || (_abs(D->T.x)>10000) || (_abs(D->T.z)>10000))
		{
		Log("xxx");
		Log("Blend--------");
		Log("blendAmount", B->blendAmount);
		Log("timeCurrent", B->timeCurrent);
		Log("timeTotal", B->timeTotal);
		Log("bone_or_part", B->bone_or_part);

		Log("blendAccrue", B->blendAccrue);
		Log("blendFalloff", B->blendFalloff);
		Log("blendPower", B->blendPower);
		Log("speed", B->speed);
		Log("playing", B->playing);
		Log("stop_at_end", B->stop_at_end);
		Log("motionID", (u32)B->motionID.idx);
		Log("blend", B->blend);

		Log("dwFrame", B->dwFrame);
		Log("Device.dwFrame", Device.dwFrame);
		Log("Blend-------end");

		Log("Bone",LL_BoneName_dbg(SelfID));
		Log("parent",*parent);
		Msg("K1t %d,%d,%d",K1t->x,K1t->y,K1t->z);
		Msg("K2t %d,%d,%d",K2t->x,K2t->y,K2t->z);

		Log("count",count);
		Log("time",time);
		Log("frame",frame);
		Log("T1",T1);
		Log("T2",T2);
		Log("delta",delta);
		Log("Dt",D->T);
		VERIFY(0);

		}
		*/
	} //if (M.test_flag(flTKeyPresent))
	else
	{
		D->T.set(M._initT);
	}
}

IC void MixInterlerp(CKey& Result, const CKey* R, const f32* BA, s32 b_count)
{
	VERIFY(MAX_BLENDED >= b_count);
	switch (b_count)
	{
		case 0:
		{
			Result.Q.set(0.0f, 0.0f, 0.0f, 0.0f);
			Result.T.set(0.0f, 0.0f, 0.0f);
		}
		break;
		case 1:
		{
			Result = R[0];
			/*
			if(Result.T.y>10000)
			{
			Log("1");
			Log("BLEND_INST",BLEND_INST.Blend.size());
			Log("Bone",LL_BoneName_dbg(SelfID));
			Msg("Result.Q %f,%f,%f,%f",Result.Q.x,Result.Q.y,Result.Q.z,Result.Q.w);
			Log("Result.T",Result.T);
			VERIFY(0);
			}
			*/
		}
		break;
		case 2:
		{
			f32 w0 = BA[0];
			f32 w1 = BA[1];
			f32 ws = w0 + w1;
			f32 w;
			if (fis_zero(ws))
			{
				w = 0;
			}
			else
			{
				w = w1 / ws;
			}

#ifdef DEBUG
			//.					if (fis_zero(w0+w1) || (!_valid(w))){
			//.						Debug.fatal		(DEBUG_INFO,"TO ALEXMX VERY IMPORTANT: (TOTAL: %f) w: %f, w0: %f, w1: %f, ws:%f, BIS: %d",w0+w1,w,w0,w1,ws,BLEND_INST.Blend.size());
			//.					}
#endif

			KEY_Interp(Result, R[0], R[1], clampr(w, 0.0f, 1.0f));
			/*
			if(Result.T.y>10000){
			Log("2");
			Log("BLEND_INST",BLEND_INST.Blend.size());
			Log("Bone",LL_BoneName_dbg(SelfID));
			Msg("Result.Q %f,%f,%f,%f",Result.Q.x,Result.Q.y,Result.Q.z,Result.Q.w);
			Log("Result.T",Result.T);
			Log("parent",*parent);
			VERIFY(0);
			}
			*/
		}
		break;
		default:
		{
			//s32 	count 	= Blend.size();
			f32 total = 0.0f;
			ConsistantKey S[MAX_BLENDED];
			for (s32 i = 0; i < b_count; i++)
			{
				S[i].set(R + i, BA[i]);
			}

			std::sort(S, S + b_count);
			CKey tmp;
			total = S[0].w;
			tmp = *S[0].K;
			for (s32 cnt = 1; cnt < b_count; cnt++)
			{
				total += S[cnt].w;
				f32	d;
				if (fis_zero(total))
				{
					d = 0.0f;
				}
				else
				{
					d = S[cnt].w / total;
				}

				clampr(d, 0.0f, 1.0f);

#ifdef DEBUG
				//.						if ((total==0) || (!_valid(S[cnt].w/total))){
				//.							Debug.fatal		(DEBUG_INFO,"TO ALEXMX VERY IMPORTANT: (TOTAL: %f) w: %f, total: %f, count: %d, real count: %d",total,S[cnt].w,total,count,BLEND_INST.Blend.size());
				//.						}
#endif

				KEY_Interp(Result, tmp, *S[cnt].K, d);
				tmp = Result;
			}
		}
		break;
	}
}

IC void key_sub(CKey& rk, const CKey& k0, const CKey& k1)//sub right
{
	fQuaternion q;
	q.inverse(k1.Q);
	rk.Q.mul(k0.Q, q);
	//rk.Q.normalize();//?
	rk.T.sub(k0.T, k1.T);
}

IC void key_identity(CKey& k)
{
	k.Q.identity( );
	k.T.set(0.0f, 0.0f, 0.0f);
}

IC void key_add(CKey& res, const CKey& k0, const CKey& k1)//add right
{
	res.Q.set(fQuaternion( ).mul(k0.Q, k1.Q));
	//res.Q.normalize();
	res.T.add(k0.T, k1.T);
}

IC void q_scale(fQuaternion& q, f32 v)
{
	f32 angl;
	fVector3 ax;
	q.get_axis_angle(ax, angl);
	q.rotation(ax, angl * v);
	q.normalize( );
}

IC void key_scale(CKey& res, const CKey& k, f32 v)
{
	res = k;
	q_scale(res.Q, v);
	res.T.mul(v);
}

IC void key_mad(CKey& res, const CKey& k0, const CKey& k1, f32 v)
{
	CKey k;
	key_scale(k, k1, v);
	key_add(res, k, k0);
}

//IC void MixInterlerp( CKey &Result, const CKey	*R, const CKey	*BR, const f32* BA, s32 b_count )
//{
//	CKey keys	[MAX_BLENDED];
//
//	MixInterlerp(Result,keys,BA,b_count);
//}

IC void keys_substruct(CKey* R, const CKey* BR, s32 b_count)
{
	for (s32 i = 0; i < b_count; i++)
	{
		CKey r;
		key_sub(r, R[i], BR[i]);
		R[i] = r;
	}
}

IC void q_scalem(fMatrix4x4& m, f32 v)
{
	fQuaternion q;
	q.set(m);
	q_scale(q, v);
	m.rotation(q);
}

//sclale base' * q by scale_factor returns result in matrix  m_res
IC void q_scale_vs_basem(fMatrix4x4& m_res, const fQuaternion& q, const fQuaternion& base, f32 scale_factor)
{
	fMatrix4x4 mb;
	fMatrix4x4 imb;
	mb.rotation(base);
	imb.invert(mb);

	fMatrix4x4 m;
	m.rotation(q);
	m_res.mul(imb, m);
	q_scalem(m_res, scale_factor);
}

IC void q_add_scaled_basem(fQuaternion& q, const fQuaternion& base, const fQuaternion& q0, const fQuaternion& q1, f32 v1)
{
	//VERIFY(0.f =< v && 1.f >= v );
	fMatrix4x4 m0;
	m0.rotation(q0);
	fMatrix4x4 m;
	fMatrix4x4 ml1;
	q_scale_vs_basem(ml1, q1, base, v1);
	m.mul(m0, ml1);
	q.set(m);
	q.normalize( );
}

IC f32 DET(const fMatrix4x4& a)
{
	return ((a._11 * (a._22 * a._33 - a._23 * a._32) - a._12 * (a._21 * a._33 - a._23 * a._31) + a._13 * (a._21 * a._32 - a._22 * a._31)));
}

IC bool check_scale(const fMatrix4x4& m)
{
	f32 det = DET(m);
	return (0.8f < det && det < 1.3f);
}

IC bool check_scale(const fQuaternion& q)
{
	fMatrix4x4 m;
	m.rotation(q);
	return check_scale(m);
}

IC void MixFactors(f32* F, s32 b_count)
{
	f32 sum = 0;
	for (s32 i = 0; i < b_count; i++)
	{
		sum += F[i];
	}

	for (s32 i2 = 0; i2 < b_count; i2++)
	{
		F[i2] /= sum;
	}
}

IC void MixinAdd(CKey& Result, const CKey* R, const f32* BA, s32 b_count)
{
	for (s32 i = 0; i < b_count; i++)
	{
		key_mad(Result, Result, R[i], BA[i]);
	}
}

IC void MixAdd(CKey& Result, const CKey* R, const f32* BA, s32 b_count)
{
	key_identity(Result);
	MixinAdd(Result, R, BA, b_count);
}

IC void MixChannels(CKey& Result, const CKey* R, const f32* BA, s32 b_count)
{
	VERIFY(b_count > 0);
	//Result.Q.set(R[0].Q);
	//Result.T.set(R[0].T);
	Result = R[0];
	++R;
	++BA;
	--b_count;
	//BA[0] = 1;
	MixinAdd(Result, R, BA, b_count);
}

// calculate single bone with key blending and callbck calling
void CKinematicsAnimated::CLBone(const CBoneData* bd, CBoneInstance& BONE_INST, const fMatrix4x4* parent, const CBlendInstance::BlendSVec& Blend, u8 channel_mask /*= (1<<0)*/)
{
	u16 SelfID = bd->GetSelfID( );
	if (LL_GetBoneVisible(SelfID))
	{
		if (BONE_INST.Callback_overwrite)
		{
			if (BONE_INST.Callback)
			{
				BONE_INST.Callback(&BONE_INST);
			}
		}
		else
		{
			CKey				R[MAX_CHANNELS][MAX_BLENDED];	//all keys 
			CKey				BK[MAX_CHANNELS][MAX_BLENDED];	//base keys
			f32					BA[MAX_CHANNELS][MAX_BLENDED];	//all factors

			s32					b_counts[MAX_CHANNELS] = {0,0,0,0}; //channel counts
			//f32				BCA[MAX_CHANNELS]		= {0,0,0,0}; //channel factors

			BlendSVecCIt		BI;
			for (BI = Blend.begin( ); BI != Blend.end( ); BI++)
			{
				CBlend* B = *BI;
				s32& b_count = b_counts[B->channel];
				CKey* D = &R[B->channel][b_count];
				if (!(channel_mask & (1 << B->channel)))
				{
					continue;
				}

				u8 channel = B->channel;
				BA[channel][b_count] = B->blendAmount;
				//BCA[channel]				+= B->blendAmount;
				CMotion& M = *LL_GetMotion(B->motionID, SelfID);
				Dequantize(*D, *B, M);

				QR2Quat(M._keysR[0], BK[channel][b_count].Q);

				if (M.test_flag(flTKeyPresent))
				{
					QT2T(M._keysT[0], M, BK[channel][b_count].T);
				}
				else
				{
					BK[channel][b_count].T.set(M._initT);
				}

				++b_count;
			///               PSGP.blerp				(D,&K1,&K2,delta);
			}

			// Blend them together
			CKey	channels[MAX_CHANNELS];
			f32	BC[MAX_CHANNELS];
			u16			ch_count = 0;

			for (u16 j = 0; MAX_CHANNELS > j; ++j)
			{
				if (j != 0 && b_counts[j] == 0)
				{
					continue;
				}

				//data for channel mix cycle based on ch_count
				CKey& C = channels[ch_count];
				BC[ch_count] = channel_factors[j];//3.f;//BCA[j]*

				if (j != 0)
				{
					keys_substruct(R[j], BK[j], b_counts[j]);
				}

				MixInterlerp(C, R[j], BA[j], b_counts[j]);

				++ch_count;
			}
			CKey	Result;
			//Mix channels
			//MixInterlerp(Result,channels,BCA,ch_count);
			MixChannels(Result, channels, BC, ch_count);
			fMatrix4x4					RES;
			RES.mk_xform(Result.Q, Result.T);
			BONE_INST.mTransform.mul_43(*parent, RES);

#ifdef DEBUG

			if (!check_scale(RES))
			{
				VERIFY(check_scale(BONE_INST.mTransform));
			}
	/*
			if(!is_similar(BONE_INST.mPrevTransform,RES,0.3f))
			{
				Msg("bone %s",*bd->name)	;
			}
			BONE_INST.mPrevTransform.set(RES);
	*/
#endif

			/*
			if(BONE_INST.mTransform.c.y>10000)
			{
			Log("BLEND_INST",BLEND_INST.Blend.size());
			Log("Bone",LL_BoneName_dbg(SelfID));
			Msg("Result.Q %f,%f,%f,%f",Result.Q.x,Result.Q.y,Result.Q.z,Result.Q.w);
			Log("Result.T",Result.T);
			Log("lp parent",(u32)parent);
			Log("parent",*parent);
			Log("RES",RES);
			Log("mT",BONE_INST.mTransform);

			CBlend*			B		=	*BI;
			CMotion&		M		=	*LL_GetMotion(B->motionID,SelfID);
			f32			time	=	B->timeCurrent*f32(SAMPLE_FPS);
			u32				frame	=	iFloor(time);
			u32				count	=	M.get_count();
			f32			delta	=	time-f32(frame);

			Log("flTKeyPresent",M.test_flag(flTKeyPresent));
			Log("M._initT",M._initT);
			Log("M._sizeT",M._sizeT);

			// translate
			if (M.test_flag(flTKeyPresent))
			{
			CKeyQT*	K1t	= &M._keysT[(frame+0)%count];
			CKeyQT*	K2t	= &M._keysT[(frame+1)%count];

			fVector3 T1,T2,Dt;
			T1.x		= f32(K1t->x)*M._sizeT.x+M._initT.x;
			T1.y		= f32(K1t->y)*M._sizeT.y+M._initT.y;
			T1.z		= f32(K1t->z)*M._sizeT.z+M._initT.z;
			T2.x		= f32(K2t->x)*M._sizeT.x+M._initT.x;
			T2.y		= f32(K2t->y)*M._sizeT.y+M._initT.y;
			T2.z		= f32(K2t->z)*M._sizeT.z+M._initT.z;

			Dt.lerp	(T1,T2,delta);

			Msg("K1t %d,%d,%d",K1t->x,K1t->y,K1t->z);
			Msg("K2t %d,%d,%d",K2t->x,K2t->y,K2t->z);

			Log("count",count);
			Log("frame",frame);
			Log("T1",T1);
			Log("T2",T2);
			Log("delta",delta);
			Log("Dt",Dt);

			}else
			{
			D->T.set	(M._initT);
			}
			VERIFY(0);
			}
			*/
			if (BONE_INST.Callback)
			{
				BONE_INST.Callback(&BONE_INST);
			}
		}

		BONE_INST.mRenderTransform.mul_43(BONE_INST.mTransform, bd->m2b_transform);
	}
}

void CKinematicsAnimated::Bone_GetAnimPos(fMatrix4x4& pos, u16 id, u8 mask_channel, bool ignore_callbacks)
{
	CBoneInstance bi = LL_GetBoneInstance(id);
	BoneChain_Calculate(&LL_GetData(id), bi, mask_channel, ignore_callbacks);
	pos.set(bi.mTransform);
}

void CKinematicsAnimated::Bone_Calculate(CBoneData* bd, fMatrix4x4* parent)
{
	u16 SelfID = bd->GetSelfID( );
	CBlendInstance& BLEND_INST = LL_GetBlendInstance(SelfID);
	CBoneInstance& BONE_INST = LL_GetBoneInstance(SelfID);
	CLBone(bd, BONE_INST, parent, BLEND_INST.blend_vector( ), u8(-1));
	// Calculate children
	for (xr_vector<CBoneData*>::iterator C = bd->children.begin( ); C != bd->children.end( ); C++)
	{
		Bone_Calculate(*C, &BONE_INST.mTransform);
	}
}

void CKinematicsAnimated::BoneChain_Calculate(const CBoneData* bd, CBoneInstance& bi, u8 mask_channel, bool ignore_callbacks)
{
	u16 SelfID = bd->GetSelfID( );
	CBlendInstance& BLEND_INST = LL_GetBlendInstance(SelfID);
	CBlendInstance::BlendSVec& Blend = BLEND_INST.blend_vector( );
	//ignore callbacks
	BoneCallback bc = bi.Callback;
	BOOL		 ow = bi.Callback_overwrite;
	if (ignore_callbacks)
	{
		bi.Callback = 0;
		bi.Callback_overwrite = 0;
	}
//
	if (SelfID == LL_GetBoneRoot( ))
	{
		CLBone(bd, bi, &Fidentity, Blend, mask_channel);
		//restore callback
		bi.Callback = bc;
		bi.Callback_overwrite = ow;
//
		return;
	}

	u16 ParentID = bd->GetParentID( );
	CBoneData* ParrentDT = &LL_GetData(ParentID);
	CBoneInstance parrent_bi = LL_GetBoneInstance(ParentID);
	BoneChain_Calculate(ParrentDT, parrent_bi, mask_channel, ignore_callbacks);
	CLBone(bd, bi, &parrent_bi.mTransform, Blend, mask_channel);
	//restore callback
	bi.Callback = bc;
	bi.Callback_overwrite = ow;
//
}

void CKinematicsAnimated::OnCalculateBones( )
{
	UpdateTracks( );
}
