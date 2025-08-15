#pragma once

template <class T>
class _matrix3x3
{
public:
	using TYPE							= T;
	using Self							= _matrix3x3<TYPE>;
	using SelfRef						= Self&;
	using SelfCRef						= const Self&;
	using Tvector						= _vector3<TYPE>;

	union
	{
		struct
		{	// Direct definition
			TYPE						_11;
			TYPE						_12;
			TYPE						_13;
			TYPE						_21;
			TYPE						_22;
			TYPE						_23;
			TYPE						_31;
			TYPE						_32;
			TYPE						_33;
		};
		struct
		{
			Tvector						i;
			Tvector						j;
			Tvector						k;
		};

		// Array
		f32								m[3][3];
	};

	// Class members
	IC SelfRef		set_rapid			(const _matrix4x4<TYPE>& a)
	{
		m[0][0]							= a.m[0][0];
		m[0][1]							= a.m[0][1];
		m[0][2]							= -a.m[0][2];
		m[1][0]							= a.m[1][0];
		m[1][1]							= a.m[1][1];
		m[1][2]							= -a.m[1][2];
		m[2][0]							= -a.m[2][0];
		m[2][1]							= -a.m[2][1];
		m[2][2]							= a.m[2][2];
		return *this;
	}
	IC SelfRef		set					(SelfCRef a)
	{
		CopyMemory						(this, &a, 9 * sizeof(f32));
		return *this;
	}
	IC SelfRef		set					(const _matrix4x4<TYPE>& a)
	{
		_11								= a._11;
		_12								= a._12;
		_13								= a._13;
		_21								= a._21;
		_22								= a._22;
		_23								= a._23;
		_31								= a._31;
		_32								= a._32;
		_33								= a._33;
		return *this;
	}
	IC SelfRef		identity			( )
	{
		_11								= 1.0f;
		_12								= 0.0f;
		_13								= 0.0f;
		_21								= 0.0f;
		_22								= 1.0f;
		_23								= 0.0f;
		_31								= 0.0f;
		_32								= 0.0f;
		_33								= 1.0f;
		return *this;
	}

	IC SelfRef		transpose			(SelfCRef matSource)					// faster version of transpose
	{
		_11								= matSource._11;
		_12								= matSource._21;
		_13								= matSource._31;
		_21								= matSource._12;
		_22								= matSource._22;
		_23								= matSource._32;
		_31								= matSource._13;
		_32								= matSource._23;
		_33								= matSource._33;
		return *this;
	}
	IC SelfRef		transpose			(const _matrix4x4<TYPE>& matSource)		// faster version of transpose
	{
		_11								= matSource._11;
		_12								= matSource._21;
		_13								= matSource._31;
		_21								= matSource._12;
		_22								= matSource._22;
		_23								= matSource._32;
		_31								= matSource._13;
		_32								= matSource._23;
		_33								= matSource._33;
		return *this;
	}
	IC SelfRef		transpose			( )										// self transpose - slower
	{
		_matrix3x3						a;
		CopyMemory						(&a, this, 9 * sizeof(f32));			// save matrix
		transpose						(a);
		return *this;
	}

	IC SelfRef		MxM					(SelfCRef M1, SelfCRef M2)
	{
		m[0][0]							= (M1.m[0][0] * M2.m[0][0] + M1.m[0][1] * M2.m[1][0] + M1.m[0][2] * M2.m[2][0]);
		m[1][0]							= (M1.m[1][0] * M2.m[0][0] + M1.m[1][1] * M2.m[1][0] + M1.m[1][2] * M2.m[2][0]);
		m[2][0]							= (M1.m[2][0] * M2.m[0][0] + M1.m[2][1] * M2.m[1][0] + M1.m[2][2] * M2.m[2][0]);
		m[0][1]							= (M1.m[0][0] * M2.m[0][1] + M1.m[0][1] * M2.m[1][1] + M1.m[0][2] * M2.m[2][1]);
		m[1][1]							= (M1.m[1][0] * M2.m[0][1] + M1.m[1][1] * M2.m[1][1] + M1.m[1][2] * M2.m[2][1]);
		m[2][1]							= (M1.m[2][0] * M2.m[0][1] + M1.m[2][1] * M2.m[1][1] + M1.m[2][2] * M2.m[2][1]);
		m[0][2]							= (M1.m[0][0] * M2.m[0][2] + M1.m[0][1] * M2.m[1][2] + M1.m[0][2] * M2.m[2][2]);
		m[1][2]							= (M1.m[1][0] * M2.m[0][2] + M1.m[1][1] * M2.m[1][2] + M1.m[1][2] * M2.m[2][2]);
		m[2][2]							= (M1.m[2][0] * M2.m[0][2] + M1.m[2][1] * M2.m[1][2] + M1.m[2][2] * M2.m[2][2]);
		return *this;
	}
	IC SelfRef		MTxM				(SelfCRef M1, SelfCRef M2)
	{
		m[0][0]							= (M1.m[0][0] * M2.m[0][0] + M1.m[1][0] * M2.m[1][0] + M1.m[2][0] * M2.m[2][0]);
		m[1][0]							= (M1.m[0][1] * M2.m[0][0] + M1.m[1][1] * M2.m[1][0] + M1.m[2][1] * M2.m[2][0]);
		m[2][0]							= (M1.m[0][2] * M2.m[0][0] + M1.m[1][2] * M2.m[1][0] + M1.m[2][2] * M2.m[2][0]);
		m[0][1]							= (M1.m[0][0] * M2.m[0][1] + M1.m[1][0] * M2.m[1][1] + M1.m[2][0] * M2.m[2][1]);
		m[1][1]							= (M1.m[0][1] * M2.m[0][1] + M1.m[1][1] * M2.m[1][1] + M1.m[2][1] * M2.m[2][1]);
		m[2][1]							= (M1.m[0][2] * M2.m[0][1] + M1.m[1][2] * M2.m[1][1] + M1.m[2][2] * M2.m[2][1]);
		m[0][2]							= (M1.m[0][0] * M2.m[0][2] + M1.m[1][0] * M2.m[1][2] + M1.m[2][0] * M2.m[2][2]);
		m[1][2]							= (M1.m[0][1] * M2.m[0][2] + M1.m[1][1] * M2.m[1][2] + M1.m[2][1] * M2.m[2][2]);
		m[2][2]							= (M1.m[0][2] * M2.m[0][2] + M1.m[1][2] * M2.m[1][2] + M1.m[2][2] * M2.m[2][2]);
		return *this;
	}

	IC void			transform_dir		(_vector2<TYPE>& dest, const _vector2<TYPE>& v) const 	// preferred to use
	{
		dest.x							= v.x * _11 + v.y * _21;
		dest.y							= v.x * _12 + v.y * _22;
		dest.z							= v.x * _13 + v.y * _23;
	}
	IC void			transform_dir		(_vector2<TYPE>& v) const
	{
		_vector2<TYPE>					res;
		transform_dir					(res, v);
		v.set							(res);
	}
	IC SelfRef		MxVpV				(Tvector& R, const Tvector& V1, const Tvector& V2) const
	{
		R.x								= (m[0][0] * V1.x + m[0][1] * V1.y + m[0][2] * V1.z + V2.x);
		R.y								= (m[1][0] * V1.x + m[1][1] * V1.y + m[1][2] * V1.z + V2.y);
		R.z								= (m[2][0] * V1.x + m[2][1] * V1.y + m[2][2] * V1.z + V2.z);
		return *this;
	}
};

using fMatrix3x3						= _matrix3x3<f32>;

template <class T>
BOOL				_valid				(const _matrix3x3<T>& m)
{
	return (_valid(m.i) && _valid(m.j) && _valid(m.k));
}
