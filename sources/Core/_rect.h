#pragma once

// Прямоугольник
template <class T>
class _rect
{
public:
	using TYPE							= T;
	using Self							= _rect<TYPE>;
	using SelfRef						= Self&;
	using SelfCRef						= const Self&;
	using Tvector						= _vector2<TYPE>;

	union
	{
		struct
		{
			TYPE						x1;
			TYPE						y1;
			TYPE						x2;
			TYPE						y2;
		};

		struct
		{
			TYPE						left;
			TYPE						top;
			TYPE						right;
			TYPE						bottom;
		};

		struct
		{
			Tvector						lt;
			Tvector						rb;
		};

		TYPE							m[4];
	};

	IC SelfRef		set					(const TYPE _x1, const TYPE _y1, const TYPE _x2, const TYPE _y2)
	{
		x1								= _x1;
		y1								= _y1;
		x2								= _x2;
		y2								= _y2;
		return *this;
	}
	IC SelfRef		set					(const Tvector& mn, const Tvector& mx)
	{
		x1								= mn.x;
		y1								= mn.y;
		x2								= mx.x;
		y2								= mx.y;
		return *this;
	}
	IC SelfRef		set					(SelfCRef r)
	{
		x1								= r.x1;
		y1								= r.y1;
		x2								= r.x2;
		y2								= r.y2;
		return *this;
	}

	IC SelfRef		null				( )
	{
		x1								= TYPE(0);
		y1								= TYPE(0);
		x2								= TYPE(0);
		y2								= TYPE(0);
		return *this;
	}

	IC SelfRef		add					(TYPE x, TYPE y)
	{
		x1								+= x;
		y1								+= y;
		x2								+= x;
		y2								+= y;
		return *this;
	}
	IC SelfRef		add					(SelfCRef r, TYPE x, TYPE y)
	{
		x1								= r.x1 + x;
		y1								= r.y1 + y;
		x2								= r.x2 + x;
		y2								= r.y2 + y;
		return *this;
	}

	IC SelfRef		sub					(TYPE x, TYPE y)
	{
		x1								-= x;
		y1								-= y;
		x2								-= x;
		y2								-= y;
		return *this;
	}
	IC SelfRef		sub					(SelfCRef r, TYPE x, TYPE y)
	{
		x1								= r.x1 - x;
		y1								= r.y1 - y;
		x2								= r.x2 - x;
		y2								= r.y2 - y;
		return *this;
	}

	IC SelfRef		mul					(TYPE x, TYPE y)
	{
		x1								*= x;
		y1								*= y;
		x2								*= x;
		y2								*= y;
		return *this;
	}
	IC SelfRef		mul					(SelfCRef r, TYPE x, TYPE y)
	{
		x1								= r.x1 * x;
		y1								= r.y1 * y;
		x2								= r.x2 * x;
		y2								= r.y2 * y;
		return *this;
	}

	IC SelfRef		div					(TYPE x, TYPE y)
	{
		x1								/= x;
		y1								/= y;
		x2								/= x;
		y2								/= y;
		return *this;
	}
	IC SelfRef		div					(SelfCRef r, TYPE x, TYPE y)
	{
		x1								= r.x1 / x;
		y1								= r.y1 / y;
		x2								= r.x2 / x;
		y2								= r.y2 / y;
		return *this;
	}

	IC BOOL			in					(TYPE x, TYPE y) const
	{
		return ((x >= x1) && (x <= x2) && (y >= y1) && (y <= y2));
	}
	IC BOOL			in					(Tvector& p) const
	{
		return ((p.x >= x1) && (p.x <= x2) && (p.y >= y1) && (p.y <= y2));
	}

	IC BOOL			cmp					(_rect<s32>& r)
	{
		return (x1 == r.x1 && y1 == r.y1 && x2 == r.x2 && y2 == r.y2);
	}
	IC BOOL			cmp					(_rect<f32>& r)
	{
		return (fsimilar(x1, r.x1) && fsimilar(y1, r.y1) && fsimilar(x2, r.x2) && fsimilar(y2, r.y2));
	}

	IC void			getcenter			(Tvector& center)
	{
		center.add(rb, lt);
		center.div(2);
	}
	IC void			getsize				(Tvector& sz)
	{
		sz.sub(rb, lt);
	}

	IC TYPE			width				( ) const
	{
		return (rb.x - lt.x);
	}
	IC TYPE			height				( ) const
	{
		return (rb.y - lt.y);
	}

	IC SelfRef		shrink				(TYPE x, TYPE y)
	{
		lt.x							+= x;
		lt.y							+= y;
		rb.x							-= x;
		rb.y							-= y;
		return *this;
	}
	IC SelfRef		grow				(TYPE x, TYPE y)
	{
		lt.x							-= x;
		lt.y							-= y;
		rb.x							+= x;
		rb.y							+= y;
		return *this;
	}

	IC BOOL			intersected			(SelfCRef b1, SelfCRef b2) const
	{
		return !(b1.x1 > b2.x2 || b1.x2 < b2.x1 || b1.y1 > b2.y2 || b1.y2 < b2.y1);
	}
	IC BOOL			intersected			(SelfCRef b) const
	{
		return intersected(*this, b);
	}

	IC BOOL			intersection		(SelfCRef b1, SelfCRef b2)
	{
		if (!intersected(b1, b2))
		{
			return FALSE;
		}

		x1								= _max(b1.x1, b2.x1);
		y1								= _max(b1.y1, b2.y1);
		x2								= _min(b1.x2, b2.x2);
		y2								= _min(b1.y2, b2.y2);
		return TRUE;
	}
};

using iRect								= _rect<s32>;
using fRect								= _rect<f32>;

template <class T>
BOOL				_valid				(const _rect<T>& m)
{
	return (lt._valid( ) && rb._valid( ));
}
