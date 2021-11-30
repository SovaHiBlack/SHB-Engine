#pragma once

template <class T>
class _box3
{
public:
	using TYPE = T;
	using Self = _box3<TYPE>;
	using SelfRef = Self&;
	using SelfCRef = const Self&;
	using Tvector = _vector3<TYPE>;
	using Tmatrix = _matrix<TYPE>;

	union
	{
		struct
		{
			Tvector							min;
			Tvector							max;
		};
		struct
		{
			TYPE							x1;
			TYPE							y1;
			TYPE							z1;
			TYPE							x2;
			TYPE							y2;
			TYPE							z2;
		};
	};

	inline BOOL				is_valid( )
	{
		return (x2 >= x1) && (y2 >= y1) && (z2 >= z1);
	}

	inline const TYPE* data( ) const
	{
		return &min.x;
	}

	inline SelfRef			set(const Tvector& _min, const Tvector& _max)
	{
		min.set(_min);
		max.set(_max);
		return *this;
	}
	inline SelfRef			set(TYPE x1, TYPE y1, TYPE z1, TYPE x2, TYPE y2, TYPE z2)
	{
		min.set(x1, y1, z1);
		max.set(x2, y2, z2);
		return *this;
	}
	inline SelfRef			set(SelfCRef b)
	{
		min.set(b.min);
		max.set(b.max);
		return *this;
	}
	inline SelfRef			setb(const Tvector& center, const Tvector& dim)
	{
		min.sub(center, dim);
		max.add(center, dim);
		return *this;
	}

	inline SelfRef			null( )
	{
		min.set(0, 0, 0);
		max.set(0, 0, 0);
		return *this;
	}
	inline SelfRef			identity( )
	{
		min.set(-0.5, -0.5, -0.5);
		max.set(0.5, 0.5, 0.5);
		return *this;
	}
	inline SelfRef			invalidate( )
	{
		min.set(type_max(TYPE), type_max(TYPE), type_max(TYPE));
		max.set(type_min(TYPE), type_min(TYPE), type_min(TYPE));
		return *this;
	}

	inline SelfRef			shrink(TYPE s)
	{
		min.add(s);
		max.sub(s);
		return *this;
	}
	inline SelfRef			shrink(const Tvector& s)
	{
		min.add(s);
		max.sub(s);
		return *this;
	}
	inline SelfRef			grow(TYPE s)
	{
		min.sub(s);
		max.add(s);
		return *this;
	}
	inline SelfRef			grow(const Tvector& s)
	{
		min.sub(s);
		max.add(s);
		return *this;
	}

	inline SelfRef			add(const Tvector& p)
	{
		min.add(p);
		max.add(p);
		return *this;
	}
	inline SelfRef			sub(const Tvector& p)
	{
		min.sub(p);
		max.sub(p);
		return *this;
	}
	inline SelfRef			offset(const Tvector& p)
	{
		min.add(p);
		max.add(p);
		return *this;
	}
	inline SelfRef			add(SelfCRef b, const Tvector& p)
	{
		min.add(b.min, p);
		max.add(b.max, p);
		return *this;
	}

	__forceinline BOOL		contains(TYPE x, TYPE y, TYPE z) const
	{
		return (x >= x1) && (x <= x2) && (y >= y1) && (y <= y2) && (z >= z1) && (z <= z2);
	}
	__forceinline BOOL		contains(const Tvector& p) const
	{
		return contains(p.x, p.y, p.z);
	}
	__forceinline BOOL		contains(SelfCRef b) const
	{
		return contains(b.min) && contains(b.max);
	}

	inline BOOL				similar(SelfCRef b) const
	{
		return min.similar(b.min) && max.similar(b.max);
	}

	__forceinline SelfRef	modify(const Tvector& p)
	{
		min.min(p);
		max.max(p);
		return *this;
	}
	__forceinline SelfRef	modify(TYPE x, TYPE y, TYPE z)
	{
		_vector3<TYPE> tmp = { x, y, z };
		return modify(tmp);
	}
	inline SelfRef			merge(SelfCRef b)
	{
		modify(b.min);
		modify(b.max);
		return *this;
	}
	inline SelfRef			merge(SelfCRef b1, SelfCRef b2)
	{
		invalidate( );
		merge(b1);
		merge(b2);
		return *this;
	}
	__forceinline SelfRef	xform(SelfCRef B, const Tmatrix& m)
	{
		// The three edges transformed: you can efficiently transform an X-only vector3
		// by just getting the "X" column of the matrix
		Tvector vx;
		Tvector vy;
		Tvector vz;
		vx.mul(m.i, B.max.x - B.min.x);
		vy.mul(m.j, B.max.y - B.min.y);
		vz.mul(m.k, B.max.z - B.min.z);

		// Transform the min point
		m.transform_tiny(min, B.min);
		max.set(min);

		// Take the transformed min & axes and find _new_ extents
		// Using CPU code in the right place is faster...
		if (negative(vx.x))
		{
			min.x += vx.x;
		}
		else
		{
			max.x += vx.x;
		}

		if (negative(vx.y))
		{
			min.y += vx.y;
		}
		else
		{
			max.y += vx.y;
		}

		if (negative(vx.z))
		{
			min.z += vx.z;
		}
		else
		{
			max.z += vx.z;
		}

		if (negative(vy.x))
		{
			min.x += vy.x;
		}
		else
		{
			max.x += vy.x;
		}

		if (negative(vy.y))
		{
			min.y += vy.y;
		}
		else
		{
			max.y += vy.y;
		}

		if (negative(vy.z))
		{
			min.z += vy.z;
		}
		else
		{
			max.z += vy.z;
		}

		if (negative(vz.x))
		{
			min.x += vz.x;
		}
		else
		{
			max.x += vz.x;
		}

		if (negative(vz.y))
		{
			min.y += vz.y;
		}
		else
		{
			max.y += vz.y;
		}

		if (negative(vz.z))
		{
			min.z += vz.z;
		}
		else
		{
			max.z += vz.z;
		}

		return *this;
	}
	__forceinline SelfRef	xform(const Tmatrix& m)
	{
		Self b;
		b.set(*this);
		return xform(b, m);
	}

	inline void				getsize(Tvector& R) const
	{
		R.sub(max, min);
	}
	inline void				getradius(Tvector& R) const
	{
		getsize(R);
		R.mul(0.5f);
	}
	inline TYPE				getradius( ) const
	{
		Tvector R;
		getradius(R);
		return R.magnitude( );
	}
	inline TYPE				getvolume( ) const
	{
		Tvector sz;
		getsize(sz);
		return sz.x * sz.y * sz.z;
	}
	inline SelfCRef			getcenter(Tvector& C) const
	{
		C.x = (min.x + max.x) * 0.5f;
		C.y = (min.y + max.y) * 0.5f;
		C.z = (min.z + max.z) * 0.5f;
		return *this;
	}
	inline SelfCRef			get_CD(Tvector& bc, Tvector& bd) const // center + dimensions
	{
		bd.sub(max, min).mul(0.5f);
		bc.add(min, bd);
		return *this;
	}
	inline SelfRef			scale(float s)					// 0.1 means make 110%, -0.1 means make 90%
	{
		Fvector3 bd;
		bd.sub(max, min).mul(s);
		grow(bd);
		return *this;
	}
	inline SelfCRef			getsphere(Tvector& C, TYPE& R) const
	{
		getcenter(C);
		R = C.distance_to(max);
		return *this;
	}

	// Detects if this box intersect other
	__forceinline BOOL		intersect(SelfCRef box)
	{
		if (max.x < box.min.x)
		{
			return FALSE;
		}

		if (max.y < box.min.y)
		{
			return FALSE;
		}

		if (max.z < box.min.z)
		{
			return FALSE;
		}

		if (min.x > box.max.x)
		{
			return FALSE;
		}

		if (min.y > box.max.y)
		{
			return FALSE;
		}

		if (min.z > box.max.z)
		{
			return FALSE;
		}

		return TRUE;
	}

	// Does the vector3 intersects box
	inline BOOL				Pick(const Tvector& start, const Tvector& dir)
	{
		TYPE alpha;
		TYPE xt;
		TYPE yt;
		TYPE zt;
		Tvector rvmin;
		Tvector rvmax;

		rvmin.sub(min, start);
		rvmax.sub(max, start);

		if (!fis_zero(dir.x))
		{
			alpha = rvmin.x / dir.x;
			yt = alpha * dir.y;
			if (yt >= rvmin.y && yt <= rvmax.y)
			{
				zt = alpha * dir.z;
				if (zt >= rvmin.z && zt <= rvmax.z)
				{
					return true;
				}
			}

			alpha = rvmax.x / dir.x;
			yt = alpha * dir.y;
			if (yt >= rvmin.y && yt <= rvmax.y)
			{
				zt = alpha * dir.z;
				if (zt >= rvmin.z && zt <= rvmax.z)
				{
					return true;
				}
			}
		}

		if (!fis_zero(dir.y))
		{
			alpha = rvmin.y / dir.y;
			xt = alpha * dir.x;
			if (xt >= rvmin.x && xt <= rvmax.x)
			{
				zt = alpha * dir.z;
				if (zt >= rvmin.z && zt <= rvmax.z)
				{
					return true;
				}
			}

			alpha = rvmax.y / dir.y;
			xt = alpha * dir.x;
			if (xt >= rvmin.x && xt <= rvmax.x)
			{
				zt = alpha * dir.z;
				if (zt >= rvmin.z && zt <= rvmax.z)
				{
					return true;
				}
			}
		}

		if (!fis_zero(dir.z))
		{
			alpha = rvmin.z / dir.z;
			xt = alpha * dir.x;
			if (xt >= rvmin.x && xt <= rvmax.x)
			{
				yt = alpha * dir.y;
				if (yt >= rvmin.y && yt <= rvmax.y)
				{
					return true;
				}
			}

			alpha = rvmax.z / dir.z;
			xt = alpha * dir.x;
			if (xt >= rvmin.x && xt <= rvmax.x)
			{
				yt = alpha * dir.y;
				if (yt >= rvmin.y && yt <= rvmax.y)
				{
					return true;
				}
			}
		}

		return false;
	}

	inline U32& IR(TYPE& x)
	{
		return (U32&) x;
	}
	enum ERP_Result
	{
		rpNone = 0,
		rpOriginInside = 1,
		rpOriginOutside = 2,
		fcv_forcedword = U32(-1)
	};
	inline ERP_Result		Pick2(const Tvector& origin, const Tvector& dir, Tvector& coord)
	{
		BOOL Inside = TRUE;
		Tvector MaxT;
		MaxT.x = MaxT.y = MaxT.z = -1.0f;

		// Find candidate planes.
		if (origin[0] < min[0])
		{
			coord[0] = min[0];
			Inside = FALSE;
			if (IR(dir[0]))
			{	// Calculate T distances to candidate planes
				MaxT[0] = (min[0] - origin[0]) / dir[0];
			}
		}
		else if (origin[0] > max[0])
		{
			coord[0] = max[0];
			Inside = FALSE;
			if (IR(dir[0]))
			{	// Calculate T distances to candidate planes
				MaxT[0] = (max[0] - origin[0]) / dir[0];
			}
		}

		if (origin[1] < min[1])
		{
			coord[1] = min[1];
			Inside = FALSE;
			if (IR(dir[1]))
			{	// Calculate T distances to candidate planes
				MaxT[1] = (min[1] - origin[1]) / dir[1];
			}
		}
		else if (origin[1] > max[1])
		{
			coord[1] = max[1];
			Inside = FALSE;
			if (IR(dir[1]))
			{	// Calculate T distances to candidate planes
				MaxT[1] = (max[1] - origin[1]) / dir[1];
			}
		}

		if (origin[2] < min[2])
		{
			coord[2] = min[2];
			Inside = FALSE;
			if (IR(dir[2]))
			{	// Calculate T distances to candidate planes
				MaxT[2] = (min[2] - origin[2]) / dir[2];
			}
		}
		else if (origin[2] > max[2])
		{
			coord[2] = max[2];
			Inside = FALSE;
			if (IR(dir[2]))
			{	// Calculate T distances to candidate planes
				MaxT[2] = (max[2] - origin[2]) / dir[2];
			}
		}

		// Ray origin inside bounding box
		if (Inside)
		{
			coord = origin;
			return	rpOriginInside;
		}

		// Get largest of the maxT's for final choice of intersection
		U32 WhichPlane = 0;
		if (MaxT[1] > MaxT[0])
		{
			WhichPlane = 1;
		}

		if (MaxT[2] > MaxT[WhichPlane])
		{
			WhichPlane = 2;
		}

		// Check final candidate actually inside box
		if (IR(MaxT[WhichPlane]) & 0x80000000)
		{
			return rpNone;
		}

		if (0 == WhichPlane)
		{	// 1 & 2
			coord[1] = origin[1] + MaxT[0] * dir[1];
			if ((coord[1] < min[1]) || (coord[1] > max[1]))
			{
				return rpNone;
			}

			coord[2] = origin[2] + MaxT[0] * dir[2];
			if ((coord[2] < min[2]) || (coord[2] > max[2]))
			{
				return rpNone;
			}

			return rpOriginOutside;
		}

		if (1 == WhichPlane)
		{	// 0 & 2
			coord[0] = origin[0] + MaxT[1] * dir[0];
			if ((coord[0] < min[0]) || (coord[0] > max[0]))
			{
				return rpNone;
			}

			coord[2] = origin[2] + MaxT[1] * dir[2];
			if ((coord[2] < min[2]) || (coord[2] > max[2]))
			{
				return rpNone;
			}

			return rpOriginOutside;
		}

		if (2 == WhichPlane)
		{	// 0 & 1
			coord[0] = origin[0] + MaxT[2] * dir[0];
			if ((coord[0] < min[0]) || (coord[0] > max[0]))
			{
				return rpNone;
			}

			coord[1] = origin[1] + MaxT[2] * dir[1];
			if ((coord[1] < min[1]) || (coord[1] > max[1]))
			{
				return rpNone;
			}

			return rpOriginOutside;
		}

		return rpNone;
	}

	inline void				getpoint(int index, Tvector& result) const
	{
		switch (index)
		{
			case 0:
			{
				result.set(min.x, min.y, min.z);
			}
			break;
			case 1:
			{
				result.set(min.x, min.y, max.z);
			}
			break;
			case 2:
			{
				result.set(max.x, min.y, max.z);
			}
			break;
			case 3:
			{
				result.set(max.x, min.y, min.z);
			}
			break;
			case 4:
			{
				result.set(min.x, max.y, min.z);
			}
			break;
			case 5:
			{
				result.set(min.x, max.y, max.z);
			}
			break;
			case 6:
			{
				result.set(max.x, max.y, max.z);
			}
			break;
			case 7:
			{
				result.set(max.x, max.y, min.z);
			}
			break;
			default:
			{
				result.set(0, 0, 0);
			}
			break;
		}
	}
	inline void				getpoints(Tvector* result)
	{
		result[0].set(min.x, min.y, min.z);
		result[1].set(min.x, min.y, max.z);
		result[2].set(max.x, min.y, max.z);
		result[3].set(max.x, min.y, min.z);
		result[4].set(min.x, max.y, min.z);
		result[5].set(min.x, max.y, max.z);
		result[6].set(max.x, max.y, max.z);
		result[7].set(max.x, max.y, min.z);
	}

	inline SelfRef			modify(SelfCRef src, const Tmatrix& M)
	{
		Tvector pt;
		for (int i = 0; i < 8; i++)
		{
			src.getpoint(i, pt);
			M.transform_tiny(pt);
			modify(pt);
		}

		return *this;
	}
};

using Fbox3 = _box3<float>;

template <class T>
BOOL _valid(const _box3<T>& c)
{
	return _valid(min) && _valid(max);
}
