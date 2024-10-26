#pragma once

template <class T>
class _box2
{
public:
	using TYPE = T;
	using Self = _box2<TYPE>;
	using SelfRef = Self&;
	using SelfCRef = const Self&;
	using Tvector = _vector2<T>;

	union
	{
		struct
		{
			Tvector min;
			Tvector max;
		};

		struct
		{
			TYPE x1;
			TYPE y1;
			TYPE x2;
			TYPE y2;
		};
	};

	IC SelfRef set(const Tvector& _min, const Tvector& _max)
	{
		min.set(_min);
		max.set(_max);
		return *this;
	}
	IC SelfRef set(TYPE x1, TYPE y1, TYPE x2, TYPE y2)
	{
		min.set(x1, y1);
		max.set(x2, y2);
		return *this;
	}
	IC SelfRef set(SelfCRef b)
	{
		min.set(b.min);
		max.set(b.max);
		return *this;
	}

	IC SelfRef null( )
	{
		min.set(0.0f, 0.0f);
		max.set(0.0f, 0.0f);
		return *this;
	}
	IC SelfRef identity( )
	{
		min.set(-0.5, -0.5, -0.5);
		max.set(0.5, 0.5, 0.5);
		return *this;
	}
	IC SelfRef invalidate( )
	{
		min.set(type_max(TYPE), type_max(TYPE));
		max.set(type_min(TYPE), type_min(TYPE));
		return *this;
	}

	IC SelfRef shrink(TYPE s)
	{
		min.add(s);
		max.sub(s);
		return *this;
	}
	IC SelfRef shrink(const Tvector& s)
	{
		min.add(s);
		max.sub(s);
		return *this;
	}
	IC SelfRef grow(TYPE s)
	{
		min.sub(s);
		max.add(s);
		return *this;
	}
	IC SelfRef grow(const Tvector& s)
	{
		min.sub(s);
		max.add(s);
		return *this;
	}

	IC SelfRef add(const Tvector& p)
	{
		min.add(p);
		max.add(p);
		return *this;
	}
	IC SelfRef offset(const Tvector& p)
	{
		min.add(p);
		max.add(p);
		return *this;
	}
	IC SelfRef add(SelfCRef b, const Tvector& p)
	{
		min.add(b.min, p);
		max.add(b.max, p);
		return *this;
	}

	IC BOOL contains(TYPE x, TYPE y)
	{
		return ((x >= x1) && (x <= x2) && (y >= y1) && (y <= y2));
	}
	IC BOOL contains(const Tvector& p)
	{
		return contains(p.x, p.y);
	}
	IC BOOL contains(SelfCRef b)
	{
		return (contains(b.min) && contains(b.max));
	}

	IC BOOL similar(SelfCRef b)
	{
		return (min.similar(b.min) && max.similar(b.max));
	}

	IC SelfRef modify(const Tvector& p)
	{
		min.min(p);
		max.max(p);
		return *this;
	}
	IC SelfRef merge(SelfCRef b)
	{
		modify(b.min);
		modify(b.max);
		return *this;
	}
	IC SelfRef merge(SelfCRef b1, SelfCRef b2)
	{
		invalidate( );
		merge(b1);
		merge(b2);
		return *this;
	}

	IC void getsize(Tvector& R) const
	{
		R.sub(max, min);
	}
	IC void getradius(Tvector& R) const
	{
		getsize(R);
		R.mul(0.5f);
	}
	IC TYPE getradius( ) const
	{
		Tvector R;
		getsize(R);
		R.mul(0.5f);
		return R.magnitude( );
	}

	IC void getcenter(Tvector& C) const
	{
		C.x = (min.x + max.x) * 0.5f;
		C.y = (min.y + max.y) * 0.5f;
	}
	IC void getsphere(Tvector& C, TYPE& R) const
	{
		getcenter(C);
		R = C.distance_to(max);
	}

	// Detects if this box intersect other
	IC BOOL intersect(SelfCRef box)
	{
		if (max.x < box.min.x)
		{
			return FALSE;
		}

		if (max.y < box.min.y)
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

		return TRUE;
	}

	// Make's this box valid AABB
	IC SelfRef sort( )
	{
		TYPE tmp;
		if (min.x > max.x)
		{
			tmp = min.x;
			min.x = max.x;
			max.x = tmp;
		}

		if (min.y > max.y)
		{
			tmp = min.y;
			min.y = max.y;
			max.y = tmp;
		}

		return *this;
	}

	// Does the vector3 intersects box
	IC BOOL Pick(const Tvector& start, const Tvector& dir)
	{
		TYPE		alpha;
		TYPE		xt;
		TYPE		yt;
		Tvector		rvmin;
		Tvector		rvmax;

		rvmin.sub(min, start);
		rvmax.sub(max, start);

		if (!fis_zero(dir.x))
		{
			alpha = rvmin.x / dir.x;
			yt = alpha * dir.y;
			if (yt >= rvmin.y && yt <= rvmax.y)
			{
				return TRUE;
			}

			alpha = rvmax.x / dir.x;
			yt = alpha * dir.y;
			if (yt >= rvmin.y && yt <= rvmax.y)
			{
				return TRUE;
			}
		}

		if (!fis_zero(dir.y))
		{
			alpha = rvmin.y / dir.y;
			xt = alpha * dir.x;
			if (xt >= rvmin.x && xt <= rvmax.x)
			{
				return TRUE;
			}

			alpha = rvmax.y / dir.y;
			xt = alpha * dir.x;
			if (xt >= rvmin.x && xt <= rvmax.x)
			{
				return TRUE;
			}
		}

		return FALSE;
	}
	ICF BOOL pick_exact(const Tvector& start, const Tvector& dir)
	{
		TYPE		alpha;
		TYPE		xt;
		TYPE		yt;
		Tvector		rvmin;
		Tvector		rvmax;

		rvmin.sub(min, start);
		rvmax.sub(max, start);

		if (_abs(dir.x) != 0)
		{
			alpha = rvmin.x / dir.x;
			yt = alpha * dir.y;
			if (yt >= rvmin.y - EPSILON_5 && yt <= rvmax.y + EPSILON_5)
			{
				return TRUE;
			}

			alpha = rvmax.x / dir.x;
			yt = alpha * dir.y;
			if (yt >= rvmin.y - EPSILON_5 && yt <= rvmax.y + EPSILON_5)
			{
				return TRUE;
			}
		}

		if (_abs(dir.y) != 0)
		{
			alpha = rvmin.y / dir.y;
			xt = alpha * dir.x;
			if (xt >= rvmin.x - EPSILON_5 && xt <= rvmax.x + EPSILON_5)
			{
				return TRUE;
			}

			alpha = rvmax.y / dir.y;
			xt = alpha * dir.x;
			if (xt >= rvmin.x - EPSILON_5 && xt <= rvmax.x + EPSILON_5)
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	IC u32& IR(TYPE& x)
	{
		return (u32&)x;
	}
	IC BOOL Pick2(const Tvector& origin, const Tvector& dir, Tvector& coord)
	{
		BOOL Inside = TRUE;
		Tvector	MaxT;
		MaxT.x = MaxT.y = -1.0f;

		// Find candidate planes.
		{
			if (origin[0] < min[0])
			{
				coord[0] = min[0];
				Inside = FALSE;
				if (IR(dir[0]))	MaxT[0] = (min[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
			}
			else if (origin[0] > max[0])
			{
				coord[0] = max[0];
				Inside = FALSE;
				if (IR(dir[0]))	MaxT[0] = (max[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
			}
		}
		{
			if (origin[1] < min[1])
			{
				coord[1] = min[1];
				Inside = FALSE;
				if (IR(dir[1]))	MaxT[1] = (min[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
			}
			else if (origin[1] > max[1])
			{
				coord[1] = max[1];
				Inside = FALSE;
				if (IR(dir[1]))	MaxT[1] = (max[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
			}
		}

		// Ray origin inside bounding box
		if (Inside)
		{
			coord = origin;
			return TRUE;
		}

		// Get largest of the maxT's for final choice of intersection
		u32 WhichPlane = 0;
		if (MaxT[1] > MaxT[0])
		{
			WhichPlane = 1;
		}

		// Check final candidate actually inside box
		if (IR(MaxT[WhichPlane]) & 0x80000000)
		{
			return FALSE;
		}

		if (0 == WhichPlane)
		{
			// 1
			coord[1] = origin[1] + MaxT[0] * dir[1];
			if ((coord[1] < min[1]) || (coord[1] > max[1]))
			{
				return FALSE;
			}

			return TRUE;
		}
		else
		{
			// 0
			coord[0] = origin[0] + MaxT[1] * dir[0];
			if ((coord[0] < min[0]) || (coord[0] > max[0]))
			{
				return FALSE;
			}

			return TRUE;
		}
	}

	IC void getpoint(s32 index, Tvector& result)
	{
		switch (index)
		{
			case 0:
			{
				result.set(min.x, min.y);
			}
			break;
			case 1:
			{
				result.set(min.x, min.y);
			}
			break;
			case 2:
			{
				result.set(max.x, min.y);
			}
			break;
			case 3:
			{
				result.set(max.x, min.y);
			}
			break;
			default:
			{
				result.set(0.0f, 0.0f);
			}
			break;
		}
	}
	IC void getpoints(Tvector* result)
	{
		result[0].set(min.x, min.y);
		result[1].set(min.x, min.y);
		result[2].set(max.x, min.y);
		result[3].set(max.x, min.y);
	}
};

using fBox2 = _box2<f32>;

template <class T>
BOOL	_valid(const _box2<T>& c)
{
	return (_valid(c.min) && _valid(c.max));
}
