#include "stdafx.h"

IC bool RAYvsCYLINDER(const fCylinder& c_cylinder, const fVector3& S, const fVector3& D, f32& R, BOOL bCull)
{
	const f32& r = c_cylinder.m_radius;
	f32 h = c_cylinder.m_height / 2.f;
	const fVector3& p = S;
	const fVector3& dir = D;

	const fVector3& c = c_cylinder.m_center;
	const fVector3& ax = c_cylinder.m_direction;
	//c.set(-IM.c.dotproduct(IM.i),-IM.c.dotproduct(IM.j),-IM.c.dotproduct(IM.k));
	//fVector3 ax;ax.set(IM.i.z,IM.j.z,IM.k.z);//??

//////////////////////////////////////////////////////////////
	fVector3	v;
	v.sub(c, p);
	f32	cs = dir.dotproduct(ax);
	f32	Lc = v.dotproduct(ax);
	f32	Lr = v.dotproduct(dir);
	////////////////////////////////////////////////
	f32 sq_cos = cs * cs;
	f32 sq_sin = 1 - sq_cos;
	f32 v_smag = v.square_magnitude( );
	const f32 sq_r = r * r;

	if (sq_sin < EPSILON_5)//paralel
	{
		f32 tr1, tr2;
		f32 sq_dist = v_smag - Lr * Lr;//
		if (sq_dist > sq_r) return false;
		f32 r_dist = _sqrt(sq_r - sq_dist) + h;
		tr1 = Lr - r_dist;

		if (tr1 > R) return false;//
		if (tr1 < 0.f)
		{
			if (bCull)return false;
			else
			{
				tr2 = Lr + r_dist;
				if (tr2 < 0.f) return false;//
				if (tr2 < R)
				{
					R = tr2;
					return true;
				}
				return false;
			}
		}
		R = tr1;
		return true;
	}

	if (sq_cos < EPSILON_5)
	{
		f32 tr1, tr2;
		//perp//
		f32 abs_c_dist = _abs(Lc);
		if (abs_c_dist > h + r)return false;
		f32 sq_dist = v_smag - Lr * Lr - Lc * Lc;
		if (sq_dist > sq_r) return false;
		f32 lc_h = abs_c_dist - h;
		if (lc_h > 0.f)
		{
			f32 sq_sphere_dist = lc_h * lc_h + sq_dist * sq_dist;
			if (sq_sphere_dist > sq_r)return false;
			f32 diff = _sqrt(sq_r - sq_sphere_dist);
			tr1 = Lr - diff;
			if (tr1 > R) return false;//
			if (tr1 < 0.f)
			{
				if (bCull)return false;
				else
				{
					tr2 = Lr + diff;
					if (tr2 < 0.f) return false;//
					if (tr2 < R)
					{
						R = tr2;
						return true;
					}
					return false;
				}
			}
		}
		f32 diff = _sqrt(sq_r - sq_dist);
		tr1 = Lr - diff;

		if (tr1 > R) return false;//
		if (tr1 < 0.f)
		{
			if (bCull)return false;
			else
			{
				tr2 = Lr + diff;
				if (tr2 < 0.f) return false;//
				if (tr2 < R)
				{
					R = tr2;
					return true;
				}
				return false;
			}
		}
		R = tr1;
		return true;
	}
	//////////////////////////////////////////////////
	f32 tr1, tr2;
	f32 r_sq_sin = 1.f / sq_sin;
	f32 tr = (Lr - cs * Lc) * r_sq_sin;
	f32 tc = (cs * Lr - Lc) * r_sq_sin;

	//more frequent separation - axes dist> radius
	//v^2+tc^2+tr^2-2*(cos*tc*tr-Lc*tc+Lr*tr)

	f32 sq_nearest_dist = v_smag + tr * tr + tc * tc - 2 * (cs * tc * tr - Lc * tc + Lr * tr);

	if (sq_nearest_dist > sq_r) return false;
	//f32 max_c_diff=//;

	f32 sq_horde = (sq_r - sq_nearest_dist);

	//f32 horde=_sqrt(sq_horde)					;
	f32 sq_c_diff = sq_horde * sq_cos * r_sq_sin;
	f32 c_diff = _sqrt(sq_c_diff);//ccc
	f32 cp1 = tc - c_diff;
	f32 cp2 = tc + c_diff;

	//cp1<cp2 
	if (cp1 > h)
	{
		//sphere 
		f32 tc_h = tc - h;//!! hi					(=)/;
		f32 sq_sphere_dist = sq_sin * tc_h * tc_h;
		if (sq_sphere_dist > sq_horde)return false;
		f32 tr_c = tr - tc_h * cs;//
		f32 diff = _sqrt(sq_horde - sq_sphere_dist);
		tr1 = tr_c - diff;
		if (tr1 > R) return false;//
		if (tr1 < 0.f)
		{
			if (bCull)return false;
			else
			{
				tr2 = tr_c + diff;
				if (tr2 < 0.f) return false;//
				if (tr2 < R)
				{
					R = tr2; return true;
				}
			}
		}
		R = tr1;
		return true;
	}

	if (cp2 < -h)
	{
		//sphere lo								/(=)
		f32 tc_h = tc + h;//!!
		f32 sq_sphere_dist = sq_sin * tc_h * tc_h;
		if (sq_sphere_dist > sq_horde)return false;
		f32 tr_c = tr - tc_h * cs;//!!
		f32 diff = _sqrt(sq_horde - sq_sphere_dist);
		tr1 = tr_c - diff;
		if (tr1 > R) return false;//
		if (tr1 < 0.f)
		{
			if (bCull)return false;
			else
			{
				tr2 = tr_c + diff;
				if (tr2 < 0.f) return false;//
				if (tr2 < R)
				{
					R = tr2; return true;
				}
			}
		}
		R = tr1;
		return true;
	}
	////////////////////////////////////////////////////////////////
	if (cs > 0.f)
	{
		if (cp1 > -h)
		{
			if (cp2 < h)
			{
				//cylinder							(=/=)
				f32 diff = c_diff / cs;
				tr1 = tr - diff;
				if (tr1 > R) return false;//
				if (tr1 < 0.f)
				{
					if (bCull)return false;
					else
					{
						tr2 = tr + diff;
						if (tr2 < 0.f) return false;//
						if (tr2 < R)
						{
							R = tr2; return true;
						}
					}
				}
				R = tr1;
				return true;
			}
			else
			{
				//mixed//cyl hi sphere					(=/)
				f32 diff = c_diff / cs;
				tr1 = tr - diff;
				if (tr1 > R) return false;//
				if (tr1 < 0.f)
				{
					if (bCull)return false;
					else
					{
						f32 tc_h = tc - h;
						f32 sq_sphere_dist = sq_sin * tc_h * tc_h;
						//if(sq_sphere_dist>sq_horde)return false	;
						f32 tr_c = tr - tc_h * cs;
						f32 diff = _sqrt(sq_horde - sq_sphere_dist);
						tr2 = tr_c + diff;
						if (tr2 < 0.f) return false;//
						if (tr2 < R)
						{
							R = tr2; return true;
						}
					}
				}
				R = tr1;
				return true;
			}
		}
		else//cp1<=-h
		{
			if (cp2 < h)
			{
				//mixed//lo sphere	cyl						(/=)

				f32 tc_h = tc + h;//(tc-(-h))
				f32 sq_sphere_dist = sq_sin * tc_h * tc_h;
				//if(sq_sphere_dist>sq_horde)return false;
				f32 diff = _sqrt(sq_horde - sq_sphere_dist);
				f32 tr_c = tr - tc_h * cs;
				tr1 = tr_c - diff;
				if (tr1 > R) return false;//
				if (tr1 < 0.f)
				{
					if (bCull)return false;
					else
					{
						f32 diff = c_diff / cs;
						tr2 = tr + diff;
						if (tr2 < 0.f) return false;//
						if (tr2 < R)
						{
							R = tr2; return true;
						}
					}
				}
				R = tr1;
				return true;
			}
			else
			{
				//-(--)-								//sphere lo&&hi

				/////////////////////////////////////////////
				f32 tc_h = tc + h;
				f32 tr_c = tr - tc_h * cs;
				f32 diff = _sqrt(sq_horde - sq_sin * tc_h * tc_h);
				tr1 = tr_c - diff;
				if (tr1 > R) return false;//
				if (tr1 < 0.f)
				{
					if (bCull)return false;
					else
					{
						f32 tc_h = tc - h;
						f32 tr_c = tr - tc_h * cs;
						f32 diff = _sqrt(sq_horde - sq_sin * tc_h * tc_h);
						tr2 = tr_c + diff;
						if (tr2 < R)
						{
							R = tr2; return true;
						}
					}
				}
				R = tr1;
				return true;
			}

		}
	}
	else
	{
		if (cp1 > -h)
		{
			if (cp2 < h)
			{
				//cylinder
				f32 diff = -c_diff / cs;
				tr1 = tr - diff;
				if (tr1 > R) return false;//
				if (tr1 < 0.f)
				{
					if (bCull)return false;
					else
					{
						tr2 = tr + diff;
						if (tr2 < 0.f) return false;//
						if (tr2 < R)
						{
							R = tr2; return true;
						}
					}
				}
				R = tr1;
				return true;
			}
			else
			{//cp1>-h&&cp2>h


				f32 tc_h = tc - h;			//hi sphere/cyl
				f32 tr_c = tr - tc_h * cs;
				f32 diff = _sqrt(sq_horde - sq_sin * tc_h * tc_h);
				tr1 = tr_c - diff;
				if (tr1 > R) return false;//
				if (tr1 < 0.f)
				{
					if (bCull)return false;
					else
					{
						diff = -c_diff / cs;
						tr2 = tr + diff;
						if (tr2 < 0.f) return false;//
						if (tr2 < R)
						{
							R = tr2; return true;
						}
					}
				}
				R = tr1;
				return true;
			}
		}
		else//cp1<-h
		{
			if (cp2 < h)
			{
				//cyl/lo sphere
				f32 diff = -c_diff / cs;
				tr1 = tr - diff;
				if (tr1 > R) return false;//
				if (tr1 < 0.f)
				{
					if (bCull)return false;
					else
					{
						//mixed//lo 
						f32 tc_h = tc + h;
						f32 tr_c = tr - tc_h * cs;
						diff = _sqrt(sq_horde - sq_sin * tc_h * tc_h);
						tr2 = tr_c + diff;
						if (tr2 < 0.f) return false;//
						if (tr2 < R)
						{
							R = tr2; return true;
						}
					}
				}
				R = tr1;
				return true;
			}
			else//cp2>=h
			{
				//-(--)-								//sphere hi&&lo

				f32 tc_h = tc - h;
				f32 tr_c = tr - tc_h * cs;
				f32 diff = _sqrt(sq_horde - sq_sin * tc_h * tc_h);
				tr1 = tr_c - diff;
				if (tr1 > R) return false;//
				/////////////////////////////////////////////
				if (tr1 < 0.f)
				{
					if (bCull)return false;
					else
					{
						tc_h = tc + h;
						tr_c = tr - tc_h * cs;
						diff = _sqrt(sq_horde - sq_sin * tc_h * tc_h);
						tr2 = tr_c + diff;
						if (tr2 < 0.f) return false;//
						if (tr2 < R)
						{
							R = tr2; return true;
						}
					}
				}
				R = tr1;
				return true;
			}
		}
	}
}

void capped_cylinder_ray_collision_test( )
{
	fCylinder c;
	c.m_center.set(0, 0, 0);
	c.m_direction.set(0, 0, 1);
	c.m_height = 2;
	c.m_radius = 1;
	//ray
	fVector3 dir;
	fVector3 pos;
	f32 R;
	dir.set(1, 0, 0); pos.set(0, 0, 0); R = 3;

	//inside
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//true , 1
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//false ,
	dir.set(0, 0, 1);
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//true , 2
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//false

	//outside
	pos.set(-3, 0, 0); dir.set(1, 0, 0); R = 4;
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//true , 2
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//true , 2
	R = 1;
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//false
	pos.set(0, 0, -3); dir.set(0, 0, 1); R = 4;
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//true , 1
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//true, 1

	pos.set(-3, -3, -3); dir.set(1, 1, 1); dir.normalize( ); R = 10;
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//true, ?
	f32 ir[2];
	c.intersect(pos, dir, ir);
	//
	pos.set(0, 0, 0);
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//true, ?
	c.intersect(pos, dir, ir);
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//false
	CTimer t; t.Start( );
	for (int i = 0; i < 1000000; i++)
	{
		fCylinder c;
		c.m_center.random_point(fVector3( ).set(2.0f, 2.0f, 2.0f));
		c.m_direction.random_dir( );
		c.m_height = Random.randF(0.2f, 2.0f);
		c.m_radius = Random.randF(0.1f, 2.0f);
		//ray
		fVector3 dir;
		fVector3 pos;
		f32 R = Random.randF(0.1f, 2.0f);
		dir.random_dir( );
		pos.random_point(fVector3( ).set(2.0f, 2.0f, 2.0f));
		RAYvsCYLINDER(c, pos, dir, R, TRUE);
	}

	Msg("my RAYvsCYLINDE time %f ms", t.GetElapsed_sec( ) * 1000.0f);
	t.Start( );
	for (int i = 0; i < 1000000; i++)
	{
		fCylinder c;
		c.m_center.random_point(fVector3( ).set(2.0f, 2.0f, 2.0f));
		c.m_direction.random_dir( );
		c.m_height = Random.randF(0.2f, 2.0f);
		c.m_radius = Random.randF(0.1f, 2.0f);
		//ray
		fVector3 dir;
		fVector3 pos;
		dir.random_dir( );
		pos.random_point(fVector3( ).set(2.0f, 2.0f, 2.0f));
		c.intersect(pos, dir, ir);
	}
	Msg("current intersect time %f ms", t.GetElapsed_sec( ) * 1000.0f);
}
