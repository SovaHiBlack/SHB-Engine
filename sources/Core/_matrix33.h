#pragma once

template <class T>
class _matrix33
{
public:
	using TYPE = T;
	using Self = _matrix33<TYPE>;
	using SelfRef = Self&;
	using SelfCRef = const Self&;
	using Tvector = _vector3<TYPE>;

	union
	{
		struct
		{						// Direct definition
			TYPE _11;
			TYPE _12;
			TYPE _13;

			TYPE _21;
			TYPE _22;
			TYPE _23;

			TYPE _31;
			TYPE _32;
			TYPE _33;
		};
		struct
		{
			Tvector i;
			Tvector j;
			Tvector k;
		};
		float m[3][3];					// Array
	};
	// Class members
	inline SelfRef set_rapid(const _matrix<TYPE>& a)
	{
		m[0][0] = a.m[0][0];
		m[0][1] = a.m[0][1];
		m[0][2] = -a.m[0][2];

		m[1][0] = a.m[1][0];
		m[1][1] = a.m[1][1];
		m[1][2] = -a.m[1][2];

		m[2][0] = -a.m[2][0];
		m[2][1] = -a.m[2][1];
		m[2][2] = a.m[2][2];

		return *this;
	}
	inline SelfRef set(SelfCRef a)
	{
		CopyMemory(this, &a, 9 * sizeof(float));
		return *this;
	}
	inline SelfRef set(const _matrix<TYPE>& a)
	{
		_11 = a._11;
		_12 = a._12;
		_13 = a._13;

		_21 = a._21;
		_22 = a._22;
		_23 = a._23;

		_31 = a._31;
		_32 = a._32;
		_33 = a._33;

		return *this;
	}
	inline SelfRef identity( )
	{
		_11 = 1.0f;
		_12 = 0.0f;
		_13 = 0.0f;

		_21 = 0.0f;
		_22 = 1.0f;
		_23 = 0.0f;

		_31 = 0.0f;
		_32 = 0.0f;
		_33 = 1.0f;

		return *this;
	}

	inline SelfRef transpose(SelfCRef matSource)	// faster version of transpose
	{
		_11 = matSource._11;
		_12 = matSource._21;
		_13 = matSource._31;

		_21 = matSource._12;
		_22 = matSource._22;
		_23 = matSource._32;

		_31 = matSource._13;
		_32 = matSource._23;
		_33 = matSource._33;

		return *this;
	}
	inline SelfRef transpose(const _matrix<TYPE>& matSource)		// faster version of transpose
	{
		_11 = matSource._11;
		_12 = matSource._21;
		_13 = matSource._31;

		_21 = matSource._12;
		_22 = matSource._22;
		_23 = matSource._32;

		_31 = matSource._13;
		_32 = matSource._23;
		_33 = matSource._33;

		return *this;
	}
	inline SelfRef transpose( )						// self transpose - slower
	{
		_matrix33 a;
		CopyMemory(&a, this, 9 * sizeof(float));					// save matrix
		transpose(a);
		return *this;
	}

	inline void transform_dir(_vector2<TYPE>& dest, const _vector2<TYPE>& v) const 	// preferred to use
	{
		dest.x = v.x * _11 + v.y * _21;
		dest.y = v.x * _12 + v.y * _22;
		dest.z = v.x * _13 + v.y * _23;
	}
	inline void transform_dir(_vector2<TYPE>& v) const
	{
		_vector2<TYPE> res;
		transform_dir(res, v);
		v.set(res);
	}
};

using Fmatrix33 = _matrix33<float>;

template <class T>
BOOL _valid(const _matrix33<T>& m)
{
	return
		_valid(m.i) &&
		_valid(m.j) &&
		_valid(m.k);
}
