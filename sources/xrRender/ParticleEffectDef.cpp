#include "stdafx.h"

#include "ParticleEffectDef.h"

//---------------------------------------------------------------------------
using namespace PAPI;
using namespace PS;

//------------------------------------------------------------------------------
// class CParticleEffectDef
//------------------------------------------------------------------------------
CPEDef::CPEDef( )
{
	m_Frame.InitDefault( );
	m_MaxParticles = 0;
	m_CachedShader = 0;
	m_fTimeLimit = 0.f;
	// collision
	m_fCollideOneMinusFriction = 1.f;
	m_fCollideResilience = 0.f;
	m_fCollideSqrCutoff = 0.f;
	// velocity scale
	m_VelocityScale.set(0.f, 0.f, 0.f);
	// align to path
	m_APDefaultRotation.set(-PI_DIV_2, 0.f, 0.f);
	// flags
	m_Flags.zero( );
}

CPEDef::~CPEDef( )
{ }
void CPEDef::CreateShader( )
{
	if (*m_ShaderName && *m_TextureName)
		m_CachedShader.create(*m_ShaderName, *m_TextureName);
}
void CPEDef::DestroyShader( )
{
	m_CachedShader.destroy( );
}
void CPEDef::SetName(pcstr name)
{
	m_Name = name;
}

void CPEDef::ExecuteAnimate(Particle* particles, u32 p_cnt, f32 dt)
{
	f32 speedFac = m_Frame.m_fSpeed * dt;
	for (u32 i = 0; i < p_cnt; i++)
	{
		Particle& m = particles[i];
		f32 f = (f32(m.frame) / 255.f + ((m.flags.is(Particle::ANIMATE_CCW)) ? -1.f : 1.f) * speedFac);
		if (f > m_Frame.m_iFrameCount)f -= m_Frame.m_iFrameCount;
		if (f < 0.f)					f += m_Frame.m_iFrameCount;
		m.frame = (u16)iFloor(f * 255.f);
	}
}

void CPEDef::ExecuteCollision(PAPI::Particle* particles, u32 p_cnt, f32 dt, CParticleEffect* owner, CollisionCallback cb)
{
	pVector pt, n;
	// Must traverse list in reverse order so Remove will work
	for (int i = p_cnt - 1; i >= 0; i--)
	{
		Particle& m = particles[i];

		bool pick_needed;
		int pick_cnt = 0;
		do
		{
			pick_needed = false;
			fVector3 	dir;
			dir.sub(m.pos, m.posB);
			f32 dist = dir.magnitude( );
			if (dist >= EPSILON_5)
			{
				dir.div(dist);
				collide::rq_result	RQ;
				collide::rq_target	RT = m_Flags.is(dfCollisionDyn) ? collide::rqtBoth : collide::rqtStatic;
				if (g_pGameLevel->ObjectSpace.RayPick(m.posB, dir, dist, RT, RQ, NULL))
				{
					pt.mad(m.posB, dir, RQ.range);
					if (RQ.O)
					{
						n.set(0.f, 1.f, 0.f);
					}
					else
					{
						CDB::TRI* T = g_pGameLevel->ObjectSpace.GetStaticTris( ) + RQ.element;
						fVector3* verts = g_pGameLevel->ObjectSpace.GetStaticVerts( );
						n.mknormal(verts[T->verts[0]], verts[T->verts[1]], verts[T->verts[2]]);
					}
					pick_cnt++;
					if (cb && (pick_cnt == 1)) if (!cb(owner, m, pt, n)) break;
					if (m_Flags.is(dfCollisionDel))
					{
						ParticleManager( )->RemoveParticle(owner->m_HandleEffect, i);
					}
					else
					{
						// Compute tangential and normal components of velocity
						f32 nmag = m.vel * n;
						pVector vn(n * nmag); 	// Normal Vn = (V.N)N
						pVector vt(m.vel - vn);	// Tangent Vt = V - Vn

						// Compute _new velocity heading out:
						// Don't apply friction if tangential velocity < cutoff
						if (vt.length2( ) <= m_fCollideSqrCutoff)
						{
							m.vel = vt - vn * m_fCollideResilience;
						}
						else
						{
							m.vel = vt * m_fCollideOneMinusFriction - vn * m_fCollideResilience;
						}
						m.pos = m.posB + m.vel * dt;
						pick_needed = true;
					}
				}
			}
			else
			{
				m.pos = m.posB;
			}
		}
		while (pick_needed && (pick_cnt < 2));
	}
}

//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
BOOL CPEDef::Load(IReader& F)
{
	R_ASSERT(F.find_chunk(PED_CHUNK_VERSION));
	u16 version = F.r_u16( );

	if (version != PED_VERSION)
		return FALSE;

	R_ASSERT(F.find_chunk(PED_CHUNK_NAME));
	F.r_stringZ(m_Name);

	R_ASSERT(F.find_chunk(PED_CHUNK_EFFECTDATA));
	m_MaxParticles = F.r_u32( );

	u32 sz = F.find_chunk(PED_CHUNK_ACTIONLIST); R_ASSERT(sz);
	m_Actions.w(F.pointer( ), sz);

	F.r_chunk(PED_CHUNK_FLAGS, &m_Flags);

	if (m_Flags.is(dfSprite))
	{
		R_ASSERT(F.find_chunk(PED_CHUNK_SPRITE));
		F.r_stringZ(m_ShaderName);
		F.r_stringZ(m_TextureName);
	}

	if (m_Flags.is(dfFramed))
	{
		R_ASSERT(F.find_chunk(PED_CHUNK_FRAME));
		F.r(&m_Frame, sizeof(SFrame));
	}

	if (m_Flags.is(dfTimeLimit))
	{
		R_ASSERT(F.find_chunk(PED_CHUNK_TIMELIMIT));
		m_fTimeLimit = F.r_float( );
	}

	if (m_Flags.is(dfCollision))
	{
		R_ASSERT(F.find_chunk(PED_CHUNK_COLLISION));
		m_fCollideOneMinusFriction = F.r_float( );
		m_fCollideResilience = F.r_float( );
		m_fCollideSqrCutoff = F.r_float( );
	}

	if (m_Flags.is(dfVelocityScale))
	{
		R_ASSERT(F.find_chunk(PED_CHUNK_VEL_SCALE));
		F.r_fvector3(m_VelocityScale);
	}

	if (m_Flags.is(dfAlignToPath))
	{
		if (F.find_chunk(PED_CHUNK_ALIGN_TO_PATH))
		{
			F.r_fvector3(m_APDefaultRotation);
		}
	}

	return TRUE;
}

void CPEDef::Save(IWriter& F)
{
	F.open_chunk(PED_CHUNK_VERSION);
	F.w_u16(PED_VERSION);
	F.close_chunk( );

	F.open_chunk(PED_CHUNK_NAME);
	F.w_stringZ(m_Name);
	F.close_chunk( );

	F.open_chunk(PED_CHUNK_EFFECTDATA);
	F.w_u32(m_MaxParticles);
	F.close_chunk( );

	F.open_chunk(PED_CHUNK_ACTIONLIST);
	F.w(m_Actions.pointer( ), m_Actions.size( ));
	F.close_chunk( );

	F.w_chunk(PED_CHUNK_FLAGS, &m_Flags, sizeof(m_Flags));

	if (m_Flags.is(dfSprite))
	{
		F.open_chunk(PED_CHUNK_SPRITE);
		F.w_stringZ(m_ShaderName);
		F.w_stringZ(m_TextureName);
		F.close_chunk( );
	}

	if (m_Flags.is(dfFramed))
	{
		F.open_chunk(PED_CHUNK_FRAME);
		F.w(&m_Frame, sizeof(SFrame));
		F.close_chunk( );
	}

	if (m_Flags.is(dfTimeLimit))
	{
		F.open_chunk(PED_CHUNK_TIMELIMIT);
		F.w_float(m_fTimeLimit);
		F.close_chunk( );
	}

	if (m_Flags.is(dfCollision))
	{
		F.open_chunk(PED_CHUNK_COLLISION);
		F.w_float(m_fCollideOneMinusFriction);
		F.w_float(m_fCollideResilience);
		F.w_float(m_fCollideSqrCutoff);
		F.close_chunk( );
	}

	if (m_Flags.is(dfVelocityScale))
	{
		F.open_chunk(PED_CHUNK_VEL_SCALE);
		F.w_fvector3(m_VelocityScale);
		F.close_chunk( );
	}

	if (m_Flags.is(dfAlignToPath))
	{
		F.open_chunk(PED_CHUNK_ALIGN_TO_PATH);
		F.w_fvector3(m_APDefaultRotation);
		F.close_chunk( );
	}
}
