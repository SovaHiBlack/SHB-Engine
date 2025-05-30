#include "stdafx.h"

#include "Rain.h"
#include "igame_persistent.h"
#include "environment.h"

#include "Render.h"
#include "igame_level.h"
#include "xr_area.h"
#include "Object.h"

static const s32	max_desired_items = 2500;
static const f32	source_radius = 12.5f;
static const f32	source_offset = 40.0f;
static const f32	max_distance = source_offset * 1.25f;
static const f32	sink_offset = -(max_distance - source_offset);
static const f32	drop_length = 5.0f;
static const f32	drop_width = 0.30f;
static const f32	drop_angle = 3.0f;
static const f32	drop_max_angle = deg2rad(10.0f);
static const f32	drop_max_wind_vel = 20.0f;
static const f32	drop_speed_min = 40.0f;
static const f32	drop_speed_max = 80.0f;

const s32	max_particles = 1000;
const s32	particles_cache = 400;
const f32 particles_time = 0.3f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffect_Rain::CEffect_Rain( )
{
	state = stIdle;

	snd_Ambient.create("ambient\\rain", st_Effect, sg_Undefined);

	IReader* F = FS.r_open("$game_meshes$", "dm\\rain.dm");
	VERIFY3(F, "Can't open file.", "dm\\rain.dm");
	DM_Drop = ::Render->model_CreateDM(F);

	//
	SH_Rain.create("effects\\rain", "fx\\fx_rain");
	hGeom_Rain.create(FVF::F_LIT, RCache.Vertex.Buffer( ), RCache.QuadIB);
	hGeom_Drops.create(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, RCache.Vertex.Buffer( ), RCache.Index.Buffer( ));
	p_create( );
	FS.r_close(F);
}

CEffect_Rain::~CEffect_Rain( )
{
	snd_Ambient.destroy( );

	// Cleanup
	p_destroy( );
	::Render->model_Delete(DM_Drop);
}

// Born
void	CEffect_Rain::Born(Item& dest, f32 radius)
{
	fVector3 axis;
	axis.set(0.0f, -1.0f, 0.0f);
	f32 gust = g_pGamePersistent->Environment( ).wind_strength_factor / 10.0f;
	f32 k = g_pGamePersistent->Environment( ).CurrentEnv.wind_velocity * gust / drop_max_wind_vel;
	clamp(k, 0.0f, 1.0f);
	f32	pitch = drop_max_angle * k - PI_DIV_2;
	axis.setHP(g_pGamePersistent->Environment( ).CurrentEnv.wind_direction, pitch);

	fVector3& view = Device.vCameraPosition;
	f32 angle = ::Random.randF(0, PI_MUL_2);
	f32 dist = ::Random.randF( );
	dist = _sqrt(dist) * radius;
	f32 x = dist * _cos(angle);
	f32 z = dist * _sin(angle);
	dest.D.random_dir(axis, deg2rad(drop_angle));
	dest.P.set(x + view.x - dest.D.x * source_offset, source_offset + view.y, z + view.z - dest.D.z * source_offset);
	dest.fSpeed = ::Random.randF(drop_speed_min, drop_speed_max);

	f32 height = max_distance;
	RenewItem(dest, height, RayPick(dest.P, dest.D, height, collide::rqtBoth));
}

BOOL CEffect_Rain::RayPick(const fVector3& s, const fVector3& d, f32& range, collide::rq_target tgt)
{
	BOOL bRes = TRUE;

	collide::rq_result	RQ;
	CObject* E = g_pGameLevel->CurrentViewEntity( );
	bRes = g_pGameLevel->ObjectSpace.RayPick(s, d, range, tgt, RQ, E);
	if (bRes)
	{
		range = RQ.range;
	}

	return bRes;
}

void CEffect_Rain::RenewItem(Item& dest, f32 height, BOOL bHit)
{
	dest.uv_set = Random.randI(2);
	if (bHit)
	{
		dest.dwTime_Life = Device.dwTimeGlobal + iFloor(1000.0f * height / dest.fSpeed) - Device.dwTimeDelta;
		dest.dwTime_Hit = Device.dwTimeGlobal + iFloor(1000.0f * height / dest.fSpeed) - Device.dwTimeDelta;
		dest.Phit.mad(dest.P, dest.D, height);
	}
	else
	{
		dest.dwTime_Life = Device.dwTimeGlobal + iFloor(1000.0f * height / dest.fSpeed) - Device.dwTimeDelta;
		dest.dwTime_Hit = Device.dwTimeGlobal + iFloor(2 * 1000.0f * height / dest.fSpeed) - Device.dwTimeDelta;
		dest.Phit.set(dest.P);
	}
}

void CEffect_Rain::OnFrame( )
{
	if (!g_pGameLevel)
	{
		return;
	}

	// Parse states
	f32	factor = g_pGamePersistent->Environment( ).CurrentEnv.rain_density;
	f32	hemi_factor = 1.0f;

	CObject* E = g_pGameLevel->CurrentViewEntity( );
	if (E && E->renderable_ROS( ))
	{
		hemi_factor = 1.0f - 2.0f * (0.3f - _min(_min(1.f, E->renderable_ROS( )->get_luminocity_hemi( )), 0.3f));
	}

	switch (state)
	{
		case stIdle:
		{
			if (factor < EPSILON_3)
			{
				return;
			}
			state = stWorking;
			snd_Ambient.play(0, sm_Looped);
			snd_Ambient.set_range(source_offset, source_offset * 2.0f);
		}
		break;
		case stWorking:
		{
			if (factor < EPSILON_3)
			{
				state = stIdle;
				snd_Ambient.stop( );
				return;
			}
		}
		break;
	}

	// ambient sound
	if (snd_Ambient._feedback( ))
	{
		fVector3 sndP;
		sndP.mad(Device.vCameraPosition, fVector3( ).set(0.0f, 1.0f, 0.0f), source_offset);
		snd_Ambient.set_position(sndP);
		snd_Ambient.set_volume(1.1f * factor * hemi_factor);
	}
}

void CEffect_Rain::Render( )
{
	if (!g_pGameLevel)
	{
		return;
	}

	f32 factor = g_pGamePersistent->Environment( ).CurrentEnv.rain_density;
	if (factor < EPSILON_3)
	{
		return;
	}

	u32 desired_items = iFloor(0.5f * (1.0f + factor) * f32(max_desired_items));
	// visual
	f32 factor_visual = factor / 2.0f + 0.5f;
	fVector3 f_rain_color = g_pGamePersistent->Environment( ).CurrentEnv.rain_color;
	u32 u_rain_color = color_rgba_f(f_rain_color.x, f_rain_color.y, f_rain_color.z, factor_visual);

	// born _new_ if needed
	f32	b_radius_wrap_sqr = _sqr((source_radius + .5f));
	if (items.size( ) < desired_items)
	{
		while (items.size( ) < desired_items)
		{
			Item one;
			Born(one, source_radius);
			items.push_back(one);
		}
	}

	// build source plane
	fPlane3 src_plane;
	fVector3 norm = { 0.0f,-1.0f,0.0f };
	fVector3 upper;
	upper.set(Device.vCameraPosition.x, Device.vCameraPosition.y + source_offset, Device.vCameraPosition.z);
	src_plane.build(upper, norm);

	// perform update
	u32 vOffset;
	FVF::LIT* verts = (FVF::LIT*) RCache.Vertex.Lock(desired_items * 4, hGeom_Rain->vb_stride, vOffset);
	FVF::LIT* start = verts;
	const fVector3& vEye = Device.vCameraPosition;
	for (u32 I = 0; I < items.size( ); I++)
	{
		// physics and time control
		Item& one = items[I];

		if (one.dwTime_Hit < Device.dwTimeGlobal)
		{
			Hit(one.Phit);
		}

		if (one.dwTime_Life < Device.dwTimeGlobal)
		{
			Born(one, source_radius);
		}

		// ��������� ������
		f32 dt = Device.fTimeDelta;
		one.P.mad(one.D, one.fSpeed * dt);

		Device.Statistic->TEST1.Begin( );
		fVector3 wdir;
		wdir.set(one.P.x - vEye.x, 0.0f, one.P.z - vEye.z);
		f32 wlen = wdir.square_magnitude( );
		if (wlen > b_radius_wrap_sqr)
		{
			wlen = _sqrt(wlen);
			if ((one.P.y - vEye.y) < sink_offset)
			{
				// need born
				one.invalidate( );
			}
			else
			{
				fVector3 inv_dir;
				fVector3 src_p;
				inv_dir.invert(one.D);
				wdir.div(wlen);
				one.P.mad(one.P, wdir, -(wlen + source_radius));
				if (src_plane.intersectRayPoint(one.P, inv_dir, src_p))
				{
					f32 dist_sqr = one.P.distance_to_sqr(src_p);
					f32 height = max_distance;
					if (RayPick(src_p, one.D, height, collide::rqtBoth))
					{
						if (_sqr(height) <= dist_sqr)
						{
							one.invalidate( );									// need born
						}
						else
						{
							RenewItem(one, height - _sqrt(dist_sqr), TRUE);		// fly to point
						}
					}
					else
					{
						RenewItem(one, max_distance - _sqrt(dist_sqr), FALSE);	// fly ...
					}
				}
				else
				{
					one.invalidate( );											// need born
				}
			}
		}

		Device.Statistic->TEST1.End( );

		// Build line
		fVector3& pos_head = one.P;
		fVector3 pos_trail;
		pos_trail.mad(pos_head, one.D, -drop_length * factor_visual);

		// Culling
		fVector3 sC;
		fVector3 lineD;
		f32 sR;
		sC.sub(pos_head, pos_trail);
		lineD.normalize(sC);
		sC.mul(0.5f);
		sR = sC.magnitude( );
		sC.add(pos_trail);
		if (!::Render->ViewBase.testSphere_dirty(sC, sR))
		{
			continue;
		}

		static fVector2 UV[2][4] =
		{
			{ {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f} },
			{ {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f} }
		};

		// Everything OK - build vertices
		fVector3 P;
		fVector3 lineTop;
		fVector3 camDir;
		camDir.sub(sC, vEye);
		camDir.normalize( );
		lineTop.crossproduct(camDir, lineD);
		f32 w = drop_width;
		u32 s = one.uv_set;
		P.mad(pos_trail, lineTop, -w);
		verts->set(P, u_rain_color, UV[s][0].x, UV[s][0].y);
		verts++;
		P.mad(pos_trail, lineTop, w);
		verts->set(P, u_rain_color, UV[s][1].x, UV[s][1].y);
		verts++;
		P.mad(pos_head, lineTop, -w);
		verts->set(P, u_rain_color, UV[s][2].x, UV[s][2].y);
		verts++;
		P.mad(pos_head, lineTop, w);
		verts->set(P, u_rain_color, UV[s][3].x, UV[s][3].y);
		verts++;
	}

	u32 vCount = (u32) (verts - start);
	RCache.Vertex.Unlock(vCount, hGeom_Rain->vb_stride);

	// Render if needed
	if (vCount)
	{
		HW.pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		RCache.set_xform_world(Fidentity);
		RCache.set_Shader(SH_Rain);
		RCache.set_Geometry(hGeom_Rain);
		RCache.Render(D3DPT_TRIANGLELIST, vOffset, 0, vCount, 0, vCount / 2);
		HW.pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}

	// Particles
	Particle* P = particle_active;
	if (0 == P)
	{
		return;
	}

	{
		f32 dt = Device.fTimeDelta;
		_IndexStream& _IS = RCache.Index;
		RCache.set_Shader(DM_Drop->shader);

		fMatrix4x4				mXform;
		fMatrix4x4				mScale;
		s32						pcount = 0;
		u32						v_offset;
		u32						i_offset;
		u32						vCount_Lock = particles_cache * DM_Drop->number_vertices;
		u32						iCount_Lock = particles_cache * DM_Drop->number_indices;
		IRender_DetailModel::fvfVertexOut* v_ptr = (IRender_DetailModel::fvfVertexOut*) RCache.Vertex.Lock(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
		u16* i_ptr = _IS.Lock(iCount_Lock, i_offset);
		while (P)
		{
			Particle* next = P->next;

			// Update
			// P can be zero sometimes and it crashes
			P->time -= dt;
			if (P->time < 0)
			{
				p_free(P);
				P = next;
				continue;
			}

			// Render
			if (::Render->ViewBase.testSphere_dirty(P->bounds.P, P->bounds.R))
			{
				// Build matrix
				f32 scale = P->time / particles_time;
				mScale.scale(scale, scale, scale);
				mXform.mul_43(P->mXForm, mScale);

				// XForm verts
				DM_Drop->transfer(mXform, v_ptr, u_rain_color, i_ptr, pcount * DM_Drop->number_vertices);
				v_ptr += DM_Drop->number_vertices;
				i_ptr += DM_Drop->number_indices;
				pcount++;

				if (pcount >= particles_cache)
				{
					// flush
					u32	dwNumPrimitives = iCount_Lock / 3;
					RCache.Vertex.Unlock(vCount_Lock, hGeom_Drops->vb_stride);
					_IS.Unlock(iCount_Lock);
					RCache.set_Geometry(hGeom_Drops);
					RCache.Render(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, dwNumPrimitives);

					v_ptr = (IRender_DetailModel::fvfVertexOut*) RCache.Vertex.Lock(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
					i_ptr = _IS.Lock(iCount_Lock, i_offset);

					pcount = 0;
				}
			}

			P = next;
		}

		// Flush if needed
		vCount_Lock = pcount * DM_Drop->number_vertices;
		iCount_Lock = pcount * DM_Drop->number_indices;
		u32	dwNumPrimitives = iCount_Lock / 3;
		RCache.Vertex.Unlock(vCount_Lock, hGeom_Drops->vb_stride);
		_IS.Unlock(iCount_Lock);
		if (pcount)
		{
			RCache.set_Geometry(hGeom_Drops);
			RCache.Render(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, dwNumPrimitives);
		}
	}
}

// startup _new_ particle system
void	CEffect_Rain::Hit(fVector3& pos)
{
	if (0 != ::Random.randI(2))
	{
		return;
	}

	Particle* P = p_allocate( );
	if (0 == P)
	{
		return;
	}

	P->time = particles_time;
	P->mXForm.rotateY(::Random.randF(PI_MUL_2));
	P->mXForm.translate_over(pos);
	P->mXForm.transform_tiny(P->bounds.P, DM_Drop->bv_sphere.P);
	P->bounds.R = DM_Drop->bv_sphere.R;
}

// initialize particles pool
void CEffect_Rain::p_create( )
{
	// pool
	particle_pool.resize(max_particles);
	for (u32 it = 0; it < particle_pool.size( ); it++)
	{
		Particle& P = particle_pool[it];
		P.prev = it ? (&particle_pool[it - 1]) : 0;
		P.next = (it < (particle_pool.size( ) - 1)) ? (&particle_pool[it + 1]) : 0;
	}

	// active and idle lists
	particle_active = 0;
	particle_idle = &particle_pool.front( );
}

// destroy particles pool
void CEffect_Rain::p_destroy( )
{
	// active and idle lists
	particle_active = 0;
	particle_idle = 0;

	// pool
	particle_pool.clear( );
}

// _delete_ node from _list_
void CEffect_Rain::p_remove(Particle* P, Particle*& LST)
{
	VERIFY(P);
	Particle* prev = P->prev;
	P->prev = NULL;
	Particle* next = P->next;
	P->next = NULL;
	if (prev)
	{
		prev->next = next;
	}

	if (next)
	{
		next->prev = prev;
	}

	if (LST == P)
	{
		LST = next;
	}
}

// insert node at the top of the head
void CEffect_Rain::p_insert(Particle* P, Particle*& LST)
{
	VERIFY(P);
	P->prev = 0;
	P->next = LST;
	if (LST)
	{
		LST->prev = P;
	}

	LST = P;
}

// determine size of _list_
s32 CEffect_Rain::p_size(Particle* P)
{
	if (0 == P)
	{
		return 0;
	}

	s32 cnt = 0;
	while (P)
	{
		P = P->next;
		cnt += 1;
	}

	return cnt;
}

// alloc node
CEffect_Rain::Particle* CEffect_Rain::p_allocate( )
{
	Particle* P = particle_idle;
	if (0 == P)
	{
		return NULL;
	}

	p_remove(P, particle_idle);
	p_insert(P, particle_active);
	return P;
}

// xr_free node
void CEffect_Rain::p_free(Particle* P)
{
	p_remove(P, particle_active);
	p_insert(P, particle_idle);
}
