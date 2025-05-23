#include "stdafx.h"
#include "wallmark_manager.h"
#include "Level.h"
#include "GameMtlLib.h"
#include "CalculateTriangle.h"
#include "profiler.h"
#ifdef DEBUG
#include "phdebug.h"
#endif

CWalmarkManager::CWalmarkManager()
{
}
CWalmarkManager::~CWalmarkManager()
{
	Clear();
}
void CWalmarkManager::Clear()
{
	m_wallmarks.clear();
}

void CWalmarkManager::AddWallmark(const fVector3& dir, const fVector3& start_pos,
								  f32 range, f32 wallmark_size,
								  SHADER_VECTOR& wallmarks_vector,int t)
{
	CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris()+t;//result.element;
	SGameMtl*	pMaterial = GMLib.GetMaterialByIdx(pTri->material);

	if(pMaterial->Flags.is(SGameMtl::flBloodmark))
	{
		//��������� ������� � ���������� �����������
		fVector3*	pVerts	= Level().ObjectSpace.GetStaticVerts();

		//��������� ����� ���������
		fVector3 end_point;
		end_point.set(0,0,0);
		end_point.mad(start_pos, dir, range);

		ref_shader* pWallmarkShader = wallmarks_vector.empty()?NULL:
		&wallmarks_vector[::Random.randI(0,wallmarks_vector.size())];

		if (pWallmarkShader)
		{
			//�������� ������� �� ���������
			::Render->add_StaticWallmark(*pWallmarkShader, end_point, wallmark_size, pTri, pVerts);
		}
	}
}

/*
void CWalmarkManager::PlaceWallmark(const fVector3& dir, const fVector3& start_pos, 
									  f32 trace_dist, f32 wallmark_size,
									  SHADER_VECTOR& wallmarks_vector,CObject* ignore_obj)
{
	collide::rq_result	result;
	BOOL				reach_wall = 
		Level().ObjectSpace.RayPick(
		start_pos,
		dir,
		trace_dist, 
		collide::rqtBoth,
		result,
		ignore_obj
		)
		&&
		!result.O;

	//���� ����� �������� �� ������������ �������
	if(reach_wall)
	{
		AddWallmark(dir,start_pos,result.range,wallmark_size,wallmarks_vector,result.element);
	}
}
*/

void CWalmarkManager::PlaceWallmarks( const fVector3& start_pos)
{
	m_pos				= start_pos;
//.	pcstr				sect				= pSettings->r_string(m_owner->cNameSect(), "wallmark_section");
	Load				("explosion_marks");

//.	Device.seqParallel.push_back	(fastdelegate::FastDelegate0<>(this,&CWalmarkManager::StartWorkflow));

	StartWorkflow		();
}

f32 Distance (const fVector3& rkPoint, const fVector3 rkTri[3], f32& pfSParam, f32& pfTParam, fVector3& closest, fVector3& dir);

void CWalmarkManager::StartWorkflow()
{
	pcstr				sect				= "explosion_marks";
	f32				m_trace_dist		= pSettings->r_float(sect,"dist");
	f32				m_wallmark_size		= pSettings->r_float(sect,"size");
	u32					max_wallmarks_count = pSettings->r_u32(sect,"max_count");

	XRC.box_options							(0);
	XRC.box_query							(Level().ObjectSpace.GetStaticModel(),m_pos, fVector3().set(m_trace_dist,m_trace_dist,m_trace_dist));

	CDB::TRI*		T_array					= Level().ObjectSpace.GetStaticTris();
	fVector3*		V_array					= Level().ObjectSpace.GetStaticVerts();
	CDB::RESULT*	R_begin                 = XRC.r_begin();
	CDB::RESULT*    R_end                   = XRC.r_end();
//.	Triangle		ntri;
//.	f32			ndist					= dInfinity;
//.	fVector3			npoint;
	u32				wm_count	= 0;

	u32 _ray_test		= 0;
//	u32 _tri_behind		= 0;
	u32 _tri_not_plane	= 0;
	u32 _not_dist		= 0;
/*
	DBG_OpenCashedDraw		();
	DBG_DrawAABB			(m_pos,fVector3().set(m_trace_dist,m_trace_dist,m_trace_dist),D3DCOLOR_XRGB(255,0,0));
	DBG_DrawAABB			(m_pos,fVector3().set(0.05f,0.05f,0.05f),D3DCOLOR_XRGB(0,255,0));
	
	CTimer T; T.Start();
*/
	for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
	{
//.		DBG_DrawTri(Res, D3DCOLOR_XRGB(0,255,0) );

		if(wm_count >= max_wallmarks_count) break;
		
//.		Triangle					tri;
		fVector3						end_point;
//.		ETriDist					c;
		fVector3						pdir;
		f32						pfSParam;
		f32						pfTParam;

//.		CalculateTriangle			(T_array+Res->id,cast_fp(m_pos),tri);
		
//.		f32 dist					= DistToTri(&tri,cast_fp(m_pos),cast_fp(pdir),cast_fp(end_point),c,V_array);
		fVector3						_tri[3];

		CDB::TRI*		_t			= T_array + Res->id;

		_tri[0]						= V_array[_t->verts[0]];
		_tri[1]						= V_array[_t->verts[1]];
		_tri[2]						= V_array[_t->verts[2]];

		f32 dist					= Distance (m_pos, _tri, pfSParam, pfTParam, end_point, pdir);

/*		
		if (c==tdBehind){
			++_tri_behind;
			continue;
		}
*/
		f32 test					= dist- EPSILON_3;
		
		if(test>0.f)
		{
			if(Level().ObjectSpace.RayTest(m_pos, pdir, test, collide::rqtStatic, NULL, m_owner))
			{
				++_ray_test;
				continue;
			}
		}

		if( fis_zero(pfSParam) || fis_zero(pfTParam) || fsimilar(pfSParam,1.0f) || fsimilar(pfTParam,1.0f)  )
		{
			++_tri_not_plane;
			continue;
		}

		if(dist <= m_trace_dist )
		{
			ref_shader wallmarkShader = m_wallmarks[::Random.randI( m_wallmarks.size())];
			::Render->add_StaticWallmark(wallmarkShader, end_point, m_wallmark_size, _t, V_array);
			++wm_count;
		}else
			++_not_dist;
	}
/*
	Msg("----------------------------------");
	Msg("tri count=%d",						XRC.r_count());
	Msg("far_dist=%d",						_not_dist);
	Msg("RayTest = %d",						_ray_test);
	Msg("c==tdBehind = %d",					_tri_behind);
	Msg	("c!=tdPlane && dist>ndist = %d",	_tri_not_plane);
	Msg("Wallmarks added = %d",				wm_count);
	Msg("Time: %d",							T.GetElapsed_ms());

	DBG_ClosedCashedDraw	(10000);
*/
}
/*
void CWalmarkManager::PlaceWallmarks(const fVector3& start_pos,CObject* ignore_obj)
{
	if(m_wallmarks)
			PlaceWallmarks(start_pos,m_trace_dist,m_wallmark_size,*m_wallmarks,ignore_obj);
}
*/

void CWalmarkManager::Load (pcstr section)
{
//.	m_trace_dist	= pSettings->r_float(section,"dist");
//.	m_wallmark_size	= pSettings->r_float(section,"size");
	
	//�������� ������� �� ������
	string256	tmp;
	pcstr wallmarks_name = pSettings->r_string(section, "wallmarks");

	int cnt		=_GetItemCount(wallmarks_name);
	VERIFY		(cnt);
	ref_shader	s;
	for (int k=0; k<cnt; ++k)
	{
		s.create ("effects\\wallmark",_GetItem(wallmarks_name,k,tmp));
		m_wallmarks.push_back	(s);
	}
}

f32 Distance (const fVector3& rkPoint, const fVector3 rkTri[3], f32& pfSParam, f32& pfTParam, fVector3& closest, fVector3& dir)
{
//.    fVector3 kDiff = rkTri.Origin() - rkPoint;
	fVector3 kDiff;
	kDiff.sub	( rkTri[0], rkPoint); //

	fVector3 Edge0;
	Edge0.sub(rkTri[1], rkTri[0]); //
	fVector3 Edge1;
	Edge1.sub(rkTri[2], rkTri[0]); //

//.    f32 fA00 = rkTri.Edge0().SquaredLength();
	f32 fA00 = Edge0.square_magnitude();

//.    f32 fA01 = rkTri.Edge0().Dot(rkTri.Edge1());
	f32 fA01 = Edge0.dotproduct(Edge1);

//.    f32 fA11 = rkTri.Edge1().SquaredLength();
	f32 fA11 = Edge1.square_magnitude();

//.    f32 fB0 = kDiff.Dot(rkTri.Edge0());
	f32 fB0 = kDiff.dotproduct(Edge0);

//.	f32 fB1 = kDiff.Dot(rkTri.Edge1());
	f32 fB1 = kDiff.dotproduct(Edge1);

//.    f32 fC = kDiff.SquaredLength();
	f32 fC = kDiff.square_magnitude();

	f32 fDet = _abs(fA00*fA11-fA01*fA01);

	f32 fS = fA01*fB1-fA11*fB0;
	f32 fT = fA01*fB0-fA00*fB1;
	f32 fSqrDist;

	if ( fS + fT <= fDet )
	{
		if ( fS < 0.0f )
		{
			if ( fT < 0.0f )  // region 4
			{
				if ( fB0 < 0.0f )
				{
					fT = 0.0f;
					if ( -fB0 >= fA00 )
					{
						fS = 1.0f;
						fSqrDist = fA00+2.0f*fB0+fC;
					}
					else
					{
						fS = -fB0/fA00;
						fSqrDist = fB0*fS+fC;
					}
				}
				else
				{
					fS = 0.0f;
					if ( fB1 >= 0.0f )
					{
						fT = 0.0f;
						fSqrDist = fC;
					}
					else if ( -fB1 >= fA11 )
					{
						fT = 1.0f;
						fSqrDist = fA11+2.0f*fB1+fC;
					}
					else
					{
						fT = -fB1/fA11;
						fSqrDist = fB1*fT+fC;
					}
				}
			}
			else  // region 3
			{
				fS = 0.0f;
				if ( fB1 >= 0.0f )
				{
					fT = 0.0f;
					fSqrDist = fC;
				}
				else if ( -fB1 >= fA11 )
				{
					fT = 1.0f;
					fSqrDist = fA11+2.0f*fB1+fC;
				}
				else
				{
					fT = -fB1/fA11;
					fSqrDist = fB1*fT+fC;
				}
			}
		}
		else if ( fT < 0.0f )  // region 5
		{
			fT = 0.0f;
			if ( fB0 >= 0.0f )
			{
				fS = 0.0f;
				fSqrDist = fC;
			}
			else if ( -fB0 >= fA00 )
			{
				fS = 1.0f;
				fSqrDist = fA00+2.0f*fB0+fC;
			}
			else
			{
				fS = -fB0/fA00;
				fSqrDist = fB0*fS+fC;
			}
		}
		else  // region 0
		{
			// minimum at interior point
			f32 fInvDet = 1.0f/fDet;
			fS *= fInvDet;
			fT *= fInvDet;
			fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
				fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
		}
	}
	else
	{
		f32 fTmp0, fTmp1, fNumer, fDenom;

		if ( fS < 0.0f )  // region 2
		{
			fTmp0 = fA01 + fB0;
			fTmp1 = fA11 + fB1;
			if ( fTmp1 > fTmp0 )
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if ( fNumer >= fDenom )
				{
					fS = 1.0f;
					fT = 0.0f;
					fSqrDist = fA00+2.0f*fB0+fC;
				}
				else
				{
					fS = fNumer/fDenom;
					fT = 1.0f - fS;
					fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
						fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
				}
			}
			else
			{
				fS = 0.0f;
				if ( fTmp1 <= 0.0f )
				{
					fT = 1.0f;
					fSqrDist = fA11+2.0f*fB1+fC;
				}
				else if ( fB1 >= 0.0f )
				{
					fT = 0.0f;
					fSqrDist = fC;
				}
				else
				{
					fT = -fB1/fA11;
					fSqrDist = fB1*fT+fC;
				}
			}
		}
		else if ( fT < 0.0f )  // region 6
		{
			fTmp0 = fA01 + fB1;
			fTmp1 = fA00 + fB0;
			if ( fTmp1 > fTmp0 )
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if ( fNumer >= fDenom )
				{
					fT = 1.0f;
					fS = 0.0f;
					fSqrDist = fA11+2.0f*fB1+fC;
				}
				else
				{
					fT = fNumer/fDenom;
					fS = 1.0f - fT;
					fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
						fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
				}
			}
			else
			{
				fT = 0.0f;
				if ( fTmp1 <= 0.0f )
				{
					fS = 1.0f;
					fSqrDist = fA00+2.0f*fB0+fC;
				}
				else if ( fB0 >= 0.0f )
				{
					fS = 0.0f;
					fSqrDist = fC;
				}
				else
				{
					fS = -fB0/fA00;
					fSqrDist = fB0*fS+fC;
				}
			}
		}
		else  // region 1
		{
			fNumer = fA11 + fB1 - fA01 - fB0;
			if ( fNumer <= 0.0f )
			{
				fS = 0.0f;
				fT = 1.0f;
				fSqrDist = fA11+2.0f*fB1+fC;
			}
			else
			{
				fDenom = fA00-2.0f*fA01+fA11;
				if ( fNumer >= fDenom )
				{
					fS = 1.0f;
					fT = 0.0f;
					fSqrDist = fA00+2.0f*fB0+fC;
				}
				else
				{
					fS = fNumer/fDenom;
					fT = 1.0f - fS;
					fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
						fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
				}
			}
		}
	}

	pfSParam = fS;
	pfTParam = fT;

	closest.mad			(rkTri[0], Edge0, fS).mad(Edge1, fT);
	
	dir.sub				(closest, rkPoint);
	dir.normalize_safe	();
	return _sqrt		(_abs(fSqrDist));
}