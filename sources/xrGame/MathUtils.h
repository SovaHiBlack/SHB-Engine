// step 1
#pragma once

IC f32*			cast_fp								(fVector3& fv)
{
	return (f32*) (&fv);
}

IC const f32*		cast_fp								(const fVector3& fv)
{
	return (const f32*) (&fv);
}

IC fVector3&			cast_fv								(f32* fp)
{
	return *((fVector3*) fp);
}

IC const fVector3&	cast_fv								(const f32* fp)
{
	return *((const fVector3*) fp);
}

ICF void			accurate_normalize					(f32* a)
{
	dReal sqr_magnitude = a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
	dReal epsilon = 1.192092896e-05F;
	if (sqr_magnitude > epsilon)
	{
		dReal l = dRecipSqrt(sqr_magnitude);
		a[0] *= l;
		a[1] *= l;
		a[2] *= l;
		return;
	}

	dReal a0, a1, a2, aa0, aa1, aa2, l;
	VERIFY(a);
	a0 = a[0];
	a1 = a[1];
	a2 = a[2];
	aa0 = dFabs(a0);
	aa1 = dFabs(a1);
	aa2 = dFabs(a2);
	if (aa1 > aa0)
	{
		if (aa2 > aa1)
		{
			goto aa2_largest;
		}
		else
		{	// aa1 is largest
			a0 /= aa1;
			a2 /= aa1;
			l = dRecipSqrt(a0 * a0 + a2 * a2 + 1);
			a[0] = a0 * l;
			a[1] = (f32) _copysign(l, a1);
			a[2] = a2 * l;
		}
	}
	else
	{
		if (aa2 > aa0)
		{
aa2_largest:	// aa2 is largest
			a0 /= aa2;
			a1 /= aa2;
			l = dRecipSqrt(a0 * a0 + a1 * a1 + 1);
			a[0] = a0 * l;
			a[1] = a1 * l;
			a[2] = (f32) _copysign(l, a2);
		}
		else
		{	// aa0 is largest
			if (aa0 <= 0)
			{
 // dDEBUGMSG ("vector has zero size"); ... this messace is annoying
				a[0] = 1;	// if all a's are zero, this is where we'll end up.
				a[1] = 0;	// return a default unit length vector.
				a[2] = 0;
				return;
			}
			a1 /= aa0;
			a2 /= aa0;
			l = dRecipSqrt(a1 * a1 + a2 * a2 + 1);
			a[0] = (f32) _copysign(l, a0);
			a[1] = a1 * l;
			a[2] = a2 * l;
		}
	}
}

IC f32			dXZMag								(const f32* v)
{
	return _sqrt(v[0] * v[0] + v[2] * v[2]);
}

IC f32			dXZMag								(const fVector3& v)
{
	return dXZMag(cast_fp(v));
}

IC f32			dXZDot								(const f32* v0, const f32* v1)
{
	return v0[0] * v1[0] + v0[2] * v1[2];
}

IC f32			dXZDotNormalized					(const fVector3& v0, const fVector3& v1)
{
	return (v0.x * v1.x + v0.z * v1.z) / _sqrt((v0.x * v0.x + v0.z * v0.z) * (v1.x * v1.x + v1.z * v1.z));
}

IC f32			dXZDotNormalized					(const f32* v0, const f32* v1)
{
	return dXZDotNormalized(cast_fv(v0), cast_fv(v1));
}

IC f32			dXZDot								(const fVector3& v0, const fVector3& v1)
{
	return v0.x * v1.x + v0.z * v1.z;
}

IC void				dVectorSet							(dReal* vd, const dReal* vs)
{
	vd[0] = vs[0];
	vd[1] = vs[1];
	vd[2] = vs[2];
}

IC void				dVectorSetInvert					(dReal* vd, const dReal* vs)
{
	vd[0] = -vs[0];
	vd[1] = -vs[1];
	vd[2] = -vs[2];
}

IC void				dVectorSetZero						(dReal* vd)
{
	vd[0] = 0.0f;
	vd[1] = 0.0f;
	vd[2] = 0.0f;
}

IC void				dVector4Set							(dReal* vd, const dReal* vs)
{
	vd[0] = vs[0];
	vd[1] = vs[1];
	vd[2] = vs[2];
	vd[3] = vs[3];
}

IC void				dVector4SetZero						(dReal* vd)
{
	vd[0] = 0.0f;
	vd[1] = 0.0f;
	vd[2] = 0.0f;
	vd[3] = 0.0f;
}

IC void				dQuaternionSet						(dReal* vd, const dReal* vs)
{
	dVector4Set(vd, vs);
}

IC void				dVectorAdd							(dReal* v, const dReal* av)
{
	v[0] += av[0];
	v[1] += av[1];
	v[2] += av[2];
}

IC void				dVectorAddMul						(dReal* v, const dReal* ad, f32 mul)
{
	v[0] += ad[0] * mul;
	v[1] += ad[1] * mul;
	v[2] += ad[2] * mul;
}

IC void				dVectorAdd							(dReal* v, const dReal* av0, const dReal* av1)
{
	v[0] = av0[0] + av1[0];
	v[1] = av0[1] + av1[1];
	v[2] = av0[2] + av1[2];
}

IC void				dVectorSub							(dReal* v, const dReal* av)
{
	v[0] -= av[0];
	v[1] -= av[1];
	v[2] -= av[2];
}

IC void				dVectorSub							(dReal* v, const dReal* av1, const dReal* av0)
{
	v[0] = av1[0] - av0[0];
	v[1] = av1[1] - av0[1];
	v[2] = av1[2] - av0[2];
}

IC void				dVectorInvert						(dReal* v)
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

IC void				dVectorMul							(dReal* v, f32 mt)
{
	v[0] *= mt;
	v[1] *= mt;
	v[2] *= mt;
}

IC void				dVectorMul							(dReal* res, const dReal* av, f32 mt)
{
	res[0] = av[0] * mt;
	res[1] = av[1] * mt;
	res[2] = av[2] * mt;
}

IC void				dVectorInterpolate					(dReal* v, dReal* to, f32 k) //changes to
{
	dVectorMul(v, 1.0f - k);
	dVectorMul(to, k);
	dVectorAdd(v, to);
}

IC void				dVectorInterpolate					(dReal* res, const dReal* from, const dReal* to, f32 k) //changes to
{
	dVector3 tov;
	dVectorSet(res, from);
	dVectorSet(tov, to);
	dVectorInterpolate(res, tov, k);
}

IC void				dVectorDeviation					(const dReal* vector3_from, const dReal* vector3_to, dReal* vector_dev)
{
	dVectorSub(vector_dev, vector3_to, vector3_from);
}

IC void				dVectorDeviationAdd					(const dReal* vector3_from, const dReal* vector3_to, dReal* vector_dev)
{
	vector_dev[0] += vector3_from[0] - vector3_to[0];
	vector_dev[1] += vector3_from[1] - vector3_to[1];
	vector_dev[2] += vector3_from[2] - vector3_to[2];
}

IC void				dMatrixSmallDeviation				(const dReal* matrix33_from, const dReal* matrix33_to, dReal* vector_dev)
{
	vector_dev[0] = matrix33_from[10] - matrix33_to[10];
	vector_dev[1] = matrix33_from[2] - matrix33_to[2];
	vector_dev[2] = matrix33_from[4] - matrix33_to[4];
}

IC bool				dVectorLimit						(const dReal* v, f32 l, dReal* lv)
{
	dReal mag = _sqrt(dDOT(v, v));
	if (mag > l)
	{
		dReal f = mag / l;
		lv[0] = v[0] / f;
		lv[1] = v[1] / f;
		lv[2] = v[2] / f;
		return true;
	}
	else
	{
		dVectorSet(lv, v);
		return false;
	}
}

IC void				dMatrixSmallDeviationAdd			(const dReal* matrix33_from, const dReal* matrix33_to, dReal* vector_dev)
{
	vector_dev[0] += matrix33_from[10] - matrix33_to[10];
	vector_dev[1] += matrix33_from[2] - matrix33_to[2];
	vector_dev[2] += matrix33_from[4] - matrix33_to[4];
}

IC void				twoq_2w								(const Fquaternion& q1, const Fquaternion& q2, f32 dt, fVector3& w)
{
	fVector3 v1;
	fVector3 v2;
	v1.set(q1.x, q1.y, q1.z);
	v2.set(q2.x, q2.y, q2.z);
	f32 cosinus = q1.w * q2.w + v1.dotproduct(v2);
	w.crossproduct(v1, v2);
	// the signum must be inverted ?
	v1.mul(q2.w);
	v2.mul(q1.w);
	w.sub(v2);
	w.add(v1);
	f32 sinus_2 = 1.0f - cosinus * cosinus, k = 2.0f / dt;
	if (sinus_2 > EPSILON_5)
	{
		k *= acos(cosinus) / _sqrt(sinus_2);
	}

	w.mul(k);
}

IC f32			to_mag_and_dir						(const fVector3& in_v, fVector3& out_v)
{
	f32 mag = in_v.magnitude( );
	if (!fis_zero(mag))
	{
		out_v.mul(in_v, 1.0f / mag);
	}
	else
	{
		out_v.set(0.0f, 0.0f, 0.0f);
	}

	return mag;
}

IC f32			to_mag_and_dir						(fVector3& in_out_v)
{
	return to_mag_and_dir(in_out_v, in_out_v);
}

IC void				to_vector							(fVector3& v, f32 mag, const fVector3 dir)
{
	v.mul(dir, mag);
}

IC void				prg_pos_on_axis						(const fVector3& in_ax_p, const fVector3& in_ax_d, fVector3& in_out_pos)
{
	in_out_pos.sub(in_ax_p);
	f32 ax_mag = in_ax_d.magnitude( );
	f32 prg = in_out_pos.dotproduct(in_ax_d) / ax_mag;
	in_out_pos.set(in_ax_d);
	in_out_pos.mul(prg / ax_mag);
	in_out_pos.add(in_ax_p);
}

IC f32			prg_pos_on_plane					(const fVector3& in_norm, f32 d, const fVector3& in_pos, fVector3& out_pos)
{
	f32 prg = d - in_pos.dotproduct(in_norm);
	fVector3 diff;
	diff.set(in_norm);
	diff.mul(prg);
	out_pos.add(in_pos, diff);
	return prg;
}

IC void				prg_dir_on_plane					(const fVector3& in_norm, const fVector3& in_dir, fVector3& out_dir)
{
	f32 prg = -in_dir.dotproduct(in_norm);
	fVector3 diff;
	diff.set(in_norm);
	diff.mul(prg);
	out_dir.add(in_dir, diff);
	return;
}

IC void				restrict_vector_in_dir				(fVector3& V, const fVector3& dir)
{
	fVector3 sub;
	sub.set(dir);
	f32 dotpr = dir.dotproduct(V);
	if (dotpr > 0.0f)
	{
		sub.mul(dotpr);
		V.sub(sub);
	}
}

IC bool				check_obb_sise						(fObb& obb)
{
	return (!fis_zero(obb.m_halfsize.x, EPSILON_3) || !fis_zero(obb.m_halfsize.y, EPSILON_3) || !fis_zero(obb.m_halfsize.z, EPSILON_3));
}

IC f32			fsignum								(f32 val)
{
	return val < 0.0f ? -1.0f : 1.0f;
}

IC void				save_max							(f32& max, f32 val)
{
	if (val > max)
	{
		max = val;
	}
}

IC void				save_min							(f32& min, f32 val)
{
	if (val < min)
	{
		min = val;
	}
}

IC void				limit_above							(f32& val, f32 limit)
{
	if (val > limit)
	{
		val = limit;
	}
}

IC void				limit_below							(f32& val, f32 limit)
{
	if (val < limit)
	{
		val = limit;
	}
}

IC void				TransferenceToThrowVel				(fVector3& in_transference_out_vel, f32 time, f32 gravity_accel)
{
	in_transference_out_vel.mul(1.0f / time);
	in_transference_out_vel.y += time * gravity_accel / 2.0f;
}

IC f32			ThrowMinVelTime						(const fVector3& transference, f32 gravity_accel)
{
	return _sqrt(2.0f * transference.magnitude( ) / gravity_accel);
}

// returns num result, tgA result tangents of throw angle 
IC u8				TransferenceAndThrowVelToTgA		(const fVector3& transference, f32 throw_vel, f32 gravity_accel, fVector2& tgA, f32& s)
{
	f32 sqx = transference.x * transference.x + transference.z * transference.z;
	f32 sqv = throw_vel * throw_vel;
	f32 sqD4 = 1.0f - gravity_accel / (sqv * sqv) * (2.0f * transference.y * sqv + gravity_accel * sqx);
	if (sqD4 < 0.0f)
	{
		return 0;
	}

	s = _sqrt(sqx);
	f32 mlt = sqv / (gravity_accel * s);
	if (sqD4 == 0.0f)
	{
		tgA.x = tgA.y = mlt;
		return 1;
	}

	f32 D4 = _sqrt(sqD4);
	tgA.x = mlt * (1.0f - D4);
	tgA.y = mlt * (1.0f + D4);
	return 2;
}

IC u8				TransferenceAndThrowVelToTgA		(const fVector3& transference, f32 throw_vel, f32 gravity_accel, fVector2& tgA)
{
	f32 s;
	return TransferenceAndThrowVelToTgA(transference, throw_vel, gravity_accel, tgA, s);
}

IC u8				TransferenceAndThrowVelToThrowDir	(const fVector3& transference, f32 throw_vel, f32 gravity_accel, fVector3	throw_dir[2])
{
	throw_dir[0] = throw_dir[1] = transference;
	fVector2 tgA;
	f32 s;
	u8 ret = TransferenceAndThrowVelToTgA(transference, throw_vel, gravity_accel, tgA, s);
	switch (ret)
	{
		case 0:
		{
			return 0;
			break;
		}
		case 2:
		{
			throw_dir[1].y = tgA.y * s;
			throw_dir[1].normalize( );
		}
		case 1:
		{
			throw_dir[0].y = tgA.x * s;
			throw_dir[0].normalize( );
			break;
		}
		default:
		{
			NODEFAULT;
		}
	}

	return ret;
}

#define MAX_OF(x, on_x, y, on_y, z, on_z)\
if (x > y)\
{\
	if (x > z)\
	{\
		on_x;\
	}\
	else\
	{\
		on_z;\
	}\
}\
else\
{\
	if (y > z)\
	{\
		on_y;\
	}\
	else\
	{\
		on_z;\
	}\
}

#define MIN_OF(x, on_x, y, on_y, z, on_z)\
if (x < y)\
{\
	if (x < z)\
	{\
		on_x;\
	}\
	else\
	{\
		on_z;\
	}\
}\
else\
{\
	if (y < z)\
	{\
		on_y;\
	}\
	else\
	{\
		on_z;\
	}\
}

#define NON_MIN_OF(x, on_x1, on_x2, y, on_y1, on_y2, z, on_z1, on_z2)\
if (x < y)\
{\
	if (x < z)\
	{\
		if (y < z)\
		{\
			on_z1;\
			on_y2;\
		}\
		else\
		{\
			on_z2;\
			on_y1;\
		}\
	}\
	else\
	{\
		on_x2;\
		on_y1;\
	}\
}\
else\
{\
	if (y < z)\
	{\
		if (x > z)\
		{\
			on_x1;\
			on_z2;\
		}\
		else\
		{\
			on_z1;\
			on_x2;\
		}\
	}\
	else\
	{\
		on_x1;\
		on_y2;\
	}\
}

#define SORT(x, on_x1, on_x2, on_x3, y, on_y1, on_y2, on_y3, z, on_z1, on_z2, on_z3)\
if (x < y)\
{\
	if (x < z)\
	{\
		if (y < z)\
		{\
			on_z1;\
			on_y2;\
			on_x3;\
		}\
		else\
		{\
			on_z2;\
			on_y1;\
			on_x3;\
		}\
	}\
	else\
	{\
		on_x2;\
		on_y1;\
		on_z3;\
	}\
}\
else\
{\
	if (y < z)\
	{\
		if (x > z)\
		{\
			on_x1;\
			on_z2;\
			on_y3;\
		}\
		else\
		{\
			on_z1;\
			on_x2;\
			on_y3;\
		}\
	}\
	else\
	{\
		on_x1;\
		on_y2;\
		on_z3;\
	}\
}

struct SInertVal
{
	f32 val;
	const f32 inertion;
	SInertVal(f32 inert) : inertion(inert)
	{
		R_ASSERT(inert > 0.0f && inert < 1.0f);
	}
	IC void new_val(f32 new_val)
	{
		val = inertion * val + (1 - inertion) * new_val;
	}

private:
	SInertVal& operator = (SInertVal& v)
	{
		R_ASSERT(false);
	}
};

IC f32			DET									(const fMatrix4x4& a)
{
	return ((a._11 * (a._22 * a._33 - a._23 * a._32) - a._12 * (a._21 * a._33 - a._23 * a._31) + a._13 * (a._21 * a._32 - a._22 * a._31)));
}

IC bool				valid_pos							(const fVector3& P, const fBox3& B)
{
	fBox3 BB = B;
	BB.grow(100000);
	return !!BB.contains(P);
}

#ifdef DEBUG
	const f32 DET_CHECK_EPS = 0.15f;		// scale -35%
#	define VERIFY_RMATRIX(M)\
	{\
		f32 d = DET(M);\
		if (!fsimilar(d, 1.0f, DET_CHECK_EPS))\
		{\
			Log("matrix--- ", M);\
			Log("determinant- ", d);\
			VERIFY2(0, "Is not valid rotational matrix");\
		}\
	};
#else // def DEBUG
#	define VERIFY_RMATRIX(M)
#endif // def DEBUG
