#include "stdafx.h"
#include "WalmarkManager.h"
#include "Level.h"
#include "GameMtlLib.h"
#include "CalculateTriangle.h"
#include "Profiler.h"

#ifdef DEBUG
#include "phdebug.h"
#endif

CWalmarkManager::CWalmarkManager( )
{ }

CWalmarkManager::~CWalmarkManager( )
{
	Clear( );
}

void CWalmarkManager::Clear( )
{
	m_wallmarks.clear( );
}

void CWalmarkManager::AddWallmark(const fVector3& dir, const fVector3& start_pos, f32 range, f32 wallmark_size, SHADER_VECTOR& wallmarks_vector, s32 t)
{
	CDB::TRI* pTri = Level( ).ObjectSpace.GetStaticTris( ) + t;//result.element;
	SGameMtl* pMaterial = GMLib.GetMaterialByIdx(pTri->material);

	if (pMaterial->Flags.is(SGameMtl::flBloodmark))
	{
		//вычислить нормаль к пораженной поверхности
		fVector3* pVerts = Level( ).ObjectSpace.GetStaticVerts( );

		//вычислить точку попадания
		fVector3 end_point;
		end_point.set(0.0f, 0.0f, 0.0f);
		end_point.mad(start_pos, dir, range);

		ref_shader* pWallmarkShader = wallmarks_vector.empty( ) ? NULL : &wallmarks_vector[::Random.randI(0, wallmarks_vector.size( ))];

		if (pWallmarkShader)
		{
			//добавить отметку на материале
			::Render->add_StaticWallmark(*pWallmarkShader, end_point, wallmark_size, pTri, pVerts);
		}
	}
}

void CWalmarkManager::PlaceWallmarks(const fVector3& start_pos)
{
	m_pos = start_pos;
	Load("explosion_marks");
	StartWorkflow( );
}

f32 Distance(const fVector3& rkPoint, const fVector3 rkTri[3], f32& pfSParam, f32& pfTParam, fVector3& closest, fVector3& dir);

void CWalmarkManager::StartWorkflow( )
{
	pcstr				sect = "explosion_marks";
	f32					m_trace_dist = pSettings->r_float(sect, "dist");
	f32					m_wallmark_size = pSettings->r_float(sect, "size");
	u32					max_wallmarks_count = pSettings->r_u32(sect, "max_count");

	XRC.box_options(0);
	XRC.box_query(Level( ).ObjectSpace.GetStaticModel( ), m_pos, fVector3( ).set(m_trace_dist, m_trace_dist, m_trace_dist));

	CDB::TRI* T_array = Level( ).ObjectSpace.GetStaticTris( );
	fVector3* V_array = Level( ).ObjectSpace.GetStaticVerts( );
	CDB::RESULT* R_begin = XRC.r_begin( );
	CDB::RESULT* R_end = XRC.r_end( );
	u32				wm_count = 0;

	u32 _ray_test = 0;
	u32 _tri_not_plane = 0;
	u32 _not_dist = 0;

	for (CDB::RESULT* Res = R_begin; Res != R_end; ++Res)
	{
		if (wm_count >= max_wallmarks_count)
		{
			break;
		}

		fVector3					end_point;
		fVector3					pdir;
		f32							pfSParam;
		f32							pfTParam;

		fVector3					_tri[3];

		CDB::TRI* _t = T_array + Res->id;

		_tri[0] = V_array[_t->verts[0]];
		_tri[1] = V_array[_t->verts[1]];
		_tri[2] = V_array[_t->verts[2]];

		f32 dist = Distance(m_pos, _tri, pfSParam, pfTParam, end_point, pdir);
		f32 test = dist - EPSILON_3;

		if (test > 0.0f)
		{
			if (Level( ).ObjectSpace.RayTest(m_pos, pdir, test, collide::rqtStatic, NULL, m_owner))
			{
				++_ray_test;
				continue;
			}
		}

		if (fis_zero(pfSParam) || fis_zero(pfTParam) || fsimilar(pfSParam, 1.0f) || fsimilar(pfTParam, 1.0f))
		{
			++_tri_not_plane;
			continue;
		}

		if (dist <= m_trace_dist)
		{
			ref_shader wallmarkShader = m_wallmarks[::Random.randI(m_wallmarks.size( ))];
			::Render->add_StaticWallmark(wallmarkShader, end_point, m_wallmark_size, _t, V_array);
			++wm_count;
		}
		else
		{
			++_not_dist;
		}
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

void CWalmarkManager::Load(pcstr section)
{
	//кровавые отметки на стенах
	string256	tmp;
	pcstr wallmarks_name = pSettings->r_string(section, "wallmarks");

	s32 cnt = _GetItemCount(wallmarks_name);
	VERIFY(cnt);
	ref_shader	s;
	for (s32 k = 0; k < cnt; ++k)
	{
		s.create("effects\\wallmark", _GetItem(wallmarks_name, k, tmp));
		m_wallmarks.push_back(s);
	}
}

f32 Distance(const fVector3& rkPoint, const fVector3 rkTri[3], f32& pfSParam, f32& pfTParam, fVector3& closest, fVector3& dir)
{
	fVector3 kDiff;
	kDiff.sub(rkTri[0], rkPoint); //

	fVector3 Edge0;
	Edge0.sub(rkTri[1], rkTri[0]); //

	fVector3 Edge1;
	Edge1.sub(rkTri[2], rkTri[0]); //

	f32 fA00 = Edge0.square_magnitude( );
	f32 fA01 = Edge0.dotproduct(Edge1);
	f32 fA11 = Edge1.square_magnitude( );

	f32 fB0 = kDiff.dotproduct(Edge0);
	f32 fB1 = kDiff.dotproduct(Edge1);

	f32 fC = kDiff.square_magnitude( );

	f32 fDet = _abs(fA00 * fA11 - fA01 * fA01);

	f32 fS = fA01 * fB1 - fA11 * fB0;
	f32 fT = fA01 * fB0 - fA00 * fB1;
	f32 fSqrDist;

	if (fS + fT <= fDet)
	{
		if (fS < 0.0f)
		{
			if (fT < 0.0f)  // region 4
			{
				if (fB0 < 0.0f)
				{
					fT = 0.0f;
					if (-fB0 >= fA00)
					{
						fS = 1.0f;
						fSqrDist = fA00 + 2.0f * fB0 + fC;
					}
					else
					{
						fS = -fB0 / fA00;
						fSqrDist = fB0 * fS + fC;
					}
				}
				else
				{
					fS = 0.0f;
					if (fB1 >= 0.0f)
					{
						fT = 0.0f;
						fSqrDist = fC;
					}
					else if (-fB1 >= fA11)
					{
						fT = 1.0f;
						fSqrDist = fA11 + 2.0f * fB1 + fC;
					}
					else
					{
						fT = -fB1 / fA11;
						fSqrDist = fB1 * fT + fC;
					}
				}
			}
			else  // region 3
			{
				fS = 0.0f;
				if (fB1 >= 0.0f)
				{
					fT = 0.0f;
					fSqrDist = fC;
				}
				else if (-fB1 >= fA11)
				{
					fT = 1.0f;
					fSqrDist = fA11 + 2.0f * fB1 + fC;
				}
				else
				{
					fT = -fB1 / fA11;
					fSqrDist = fB1 * fT + fC;
				}
			}
		}
		else if (fT < 0.0f)  // region 5
		{
			fT = 0.0f;
			if (fB0 >= 0.0f)
			{
				fS = 0.0f;
				fSqrDist = fC;
			}
			else if (-fB0 >= fA00)
			{
				fS = 1.0f;
				fSqrDist = fA00 + 2.0f * fB0 + fC;
			}
			else
			{
				fS = -fB0 / fA00;
				fSqrDist = fB0 * fS + fC;
			}
		}
		else  // region 0
		{
			// minimum at interior point
			f32 fInvDet = 1.0f / fDet;
			fS *= fInvDet;
			fT *= fInvDet;
			fSqrDist = fS * (fA00 * fS + fA01 * fT + 2.0f * fB0) + fT * (fA01 * fS + fA11 * fT + 2.0f * fB1) + fC;
		}
	}
	else
	{
		f32 fTmp0;
		f32 fTmp1;
		f32 fNumer;
		f32 fDenom;

		if (fS < 0.0f)  // region 2
		{
			fTmp0 = fA01 + fB0;
			fTmp1 = fA11 + fB1;
			if (fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00 - 2.0f * fA01 + fA11;
				if (fNumer >= fDenom)
				{
					fS = 1.0f;
					fT = 0.0f;
					fSqrDist = fA00 + 2.0f * fB0 + fC;
				}
				else
				{
					fS = fNumer / fDenom;
					fT = 1.0f - fS;
					fSqrDist = fS * (fA00 * fS + fA01 * fT + 2.0f * fB0) + fT * (fA01 * fS + fA11 * fT + 2.0f * fB1) + fC;
				}
			}
			else
			{
				fS = 0.0f;
				if (fTmp1 <= 0.0f)
				{
					fT = 1.0f;
					fSqrDist = fA11 + 2.0f * fB1 + fC;
				}
				else if (fB1 >= 0.0f)
				{
					fT = 0.0f;
					fSqrDist = fC;
				}
				else
				{
					fT = -fB1 / fA11;
					fSqrDist = fB1 * fT + fC;
				}
			}
		}
		else if (fT < 0.0f)  // region 6
		{
			fTmp0 = fA01 + fB1;
			fTmp1 = fA00 + fB0;
			if (fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00 - 2.0f * fA01 + fA11;
				if (fNumer >= fDenom)
				{
					fT = 1.0f;
					fS = 0.0f;
					fSqrDist = fA11 + 2.0f * fB1 + fC;
				}
				else
				{
					fT = fNumer / fDenom;
					fS = 1.0f - fT;
					fSqrDist = fS * (fA00 * fS + fA01 * fT + 2.0f * fB0) + fT * (fA01 * fS + fA11 * fT + 2.0f * fB1) + fC;
				}
			}
			else
			{
				fT = 0.0f;
				if (fTmp1 <= 0.0f)
				{
					fS = 1.0f;
					fSqrDist = fA00 + 2.0f * fB0 + fC;
				}
				else if (fB0 >= 0.0f)
				{
					fS = 0.0f;
					fSqrDist = fC;
				}
				else
				{
					fS = -fB0 / fA00;
					fSqrDist = fB0 * fS + fC;
				}
			}
		}
		else  // region 1
		{
			fNumer = fA11 + fB1 - fA01 - fB0;
			if (fNumer <= 0.0f)
			{
				fS = 0.0f;
				fT = 1.0f;
				fSqrDist = fA11 + 2.0f * fB1 + fC;
			}
			else
			{
				fDenom = fA00 - 2.0f * fA01 + fA11;
				if (fNumer >= fDenom)
				{
					fS = 1.0f;
					fT = 0.0f;
					fSqrDist = fA00 + 2.0f * fB0 + fC;
				}
				else
				{
					fS = fNumer / fDenom;
					fT = 1.0f - fS;
					fSqrDist = fS * (fA00 * fS + fA01 * fT + 2.0f * fB0) + fT * (fA01 * fS + fA11 * fT + 2.0f * fB1) + fC;
				}
			}
		}
	}

	pfSParam = fS;
	pfTParam = fT;

	closest.mad(rkTri[0], Edge0, fS).mad(Edge1, fT);

	dir.sub(closest, rkPoint);
	dir.normalize_safe( );
	return _sqrt(_abs(fSqrDist));
}
