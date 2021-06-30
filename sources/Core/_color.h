#pragma once

// maps unsigned 8 bits/channel to D3DCOLOR
__forceinline U32			color_argb(U32 a, U32 r, U32 g, U32 b)
{
	return ((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
}
__forceinline U32			color_rgba(U32 r, U32 g, U32 b, U32 a)
{
	return color_argb(a, r, g, b);
}
__forceinline U32			color_argb_f(float a, float r, float g, float b)
{
	int _r = clampr(iFloor(r * 255.0f), 0, 255);
	int _g = clampr(iFloor(g * 255.0f), 0, 255);
	int _b = clampr(iFloor(b * 255.0f), 0, 255);
	int _a = clampr(iFloor(a * 255.0f), 0, 255);
	return color_argb(_a, _r, _g, _b);
}
__forceinline U32			color_rgba_f(float r, float g, float b, float a)
{
	return color_argb_f(a, r, g, b);
}
__forceinline U32			color_xrgb(U32 r, U32 g, U32 b)
{
	return color_argb(0xff, r, g, b);
}
__forceinline U32			color_get_R(U32 rgba)
{
	return (((rgba) >> 16) & 0xff);
}
__forceinline U32			color_get_G(U32 rgba)
{
	return (((rgba) >> 8) & 0xff);
}
__forceinline U32			color_get_B(U32 rgba)
{
	return ((rgba) & 0xff);
}
__forceinline U32			color_get_A(U32 rgba)
{
	return ((rgba) >> 24);
}
__forceinline U32			subst_alpha(U32 rgba, U32 a)
{
	return rgba & ~color_rgba(0, 0, 0, 0xff) | color_rgba(0, 0, 0, a);
}
__forceinline U32			bgr2rgb(U32 bgr)
{
	return color_rgba(color_get_B(bgr), color_get_G(bgr), color_get_R(bgr), 0);
}
__forceinline U32			rgb2bgr(U32 rgb)
{
	return bgr2rgb(rgb);
}

template <class T>
class _color
{
public:
	using TYPE = T;
	using Self = _color;
	using SelfRef = Self&;
	using SelfCRef = const Self&;

	TYPE										r;
	TYPE										g;
	TYPE										b;
	TYPE										a;

	__forceinline SelfRef	set(U32 dw)
	{
		const TYPE f = TYPE(1.0) / TYPE(255.0);
		a = f * TYPE((dw >> 24) & 0xff);
		r = f * TYPE((dw >> 16) & 0xff);
		g = f * TYPE((dw >> 8) & 0xff);
		b = f * TYPE((dw >> 0) & 0xff);
		return *this;
	}
	inline SelfRef			set(TYPE _r, TYPE _g, TYPE _b, TYPE _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
		return *this;
	}
	inline SelfRef			set(SelfCRef dw)
	{
		r = dw.r;
		g = dw.g;
		b = dw.b;
		a = dw.a;
		return *this;
	}
	__forceinline U32		get( ) const
	{
		return color_rgba_f(r, g, b, a);
	}
	inline U32				get_windows( ) const
	{
		U8 _a;
		U8 _r;
		U8 _g;
		U8 _b;
		_a = (U8) (a * 255.0f);
		_r = (U8) (r * 255.0f);
		_g = (U8) (g * 255.0f);
		_b = (U8) (b * 255.0f);
		return ((U32) (_a << 24) | (_b << 16) | (_g << 8) | (_r));
	}
	inline SelfRef			set_windows(U32 dw)
	{
		const TYPE f = 1.0f / 255.0f;
		a = f * (TYPE) (U8) (dw >> 24);
		b = f * (TYPE) (U8) (dw >> 16);
		g = f * (TYPE) (U8) (dw >> 8);
		r = f * (TYPE) (U8) (dw >> 0);
		return *this;
	}
	inline SelfRef			adjust_contrast(TYPE f)				// >1 - contrast will be increased
	{
		r = 0.5f + f * (r - 0.5f);
		g = 0.5f + f * (g - 0.5f);
		b = 0.5f + f * (b - 0.5f);
		return *this;
	}
	inline SelfRef			adjust_contrast(SelfCRef in, TYPE f)	// >1 - contrast will be increased
	{
		r = 0.5f + f * (in.r - 0.5f);
		g = 0.5f + f * (in.g - 0.5f);
		b = 0.5f + f * (in.b - 0.5f);
		return *this;
	}
	inline SelfRef			adjust_saturation(TYPE s)
	{
		// Approximate values for each component's contribution to luminance.
		// Based upon the NTSC standard described in ITU-R Recommendation BT.709.
		TYPE grey = r * 0.2125f + g * 0.7154f + b * 0.0721f;

		r = grey + s * (r - grey);
		g = grey + s * (g - grey);
		b = grey + s * (b - grey);
		return *this;
	}
	inline SelfRef			adjust_saturation(SelfCRef in, TYPE s)
	{
		// Approximate values for each component's contribution to luminance.
		// Based upon the NTSC standard described in ITU-R Recommendation BT.709.
		TYPE grey = in.r * 0.2125f + in.g * 0.7154f + in.b * 0.0721f;

		r = grey + s * (in.r - grey);
		g = grey + s * (in.g - grey);
		b = grey + s * (in.b - grey);
		return *this;
	}
	inline SelfRef			modulate(_color& in)
	{
		r *= in.r;
		g *= in.g;
		b *= in.b;
		a *= in.a;
		return *this;
	}
	inline SelfRef			modulate(SelfCRef in1, SelfCRef in2)
	{
		r = in1.r * in2.r;
		g = in1.g * in2.g;
		b = in1.b * in2.b;
		a = in1.a * in2.a;
		return *this;
	}
	inline SelfRef			negative(SelfCRef in)
	{
		r = 1.0f - in.r;
		g = 1.0f - in.g;
		b = 1.0f - in.b;
		a = 1.0f - in.a;
		return *this;
	}
	inline SelfRef			negative( )
	{
		r = 1.0f - r;
		g = 1.0f - g;
		b = 1.0f - b;
		a = 1.0f - a;
		return *this;
	}
	inline SelfRef			sub_rgb(TYPE s)
	{
		r -= s;
		g -= s;
		b -= s;
		return *this;
	}
	inline SelfRef			add_rgb(TYPE s)
	{
		r += s;
		g += s;
		b += s;
		return *this;
	}
	inline SelfRef			add_rgba(TYPE s)
	{
		r += s;
		g += s;
		b += s;
		a += s;
		return *this;
	}
	inline SelfRef			mul_rgba(TYPE s)
	{
		r *= s;
		g *= s;
		b *= s;
		a *= s;
		return *this;
	}
	inline SelfRef			mul_rgb(TYPE s)
	{
		r *= s;
		g *= s;
		b *= s;
		return *this;
	}
	inline SelfRef			mul_rgba(SelfCRef c, TYPE s)
	{
		r = c.r * s;
		g = c.g * s;
		b = c.b * s;
		a = c.a * s;
		return *this;
	}
	inline SelfRef			mul_rgb(SelfCRef c, TYPE s)
	{
		r = c.r * s;
		g = c.g * s;
		b = c.b * s;
		return *this;
	}

	// SQ magnitude
	inline TYPE				magnitude_sqr_rgb( ) const
	{
		return r * r + g * g + b * b;
	}
	// magnitude
	inline TYPE				magnitude_rgb( ) const
	{
		return _sqrt(magnitude_sqr_rgb( ));
	}
	inline TYPE				intensity( ) const
	{
		return (r + g + b) / 3.0f;
	}

	// Normalize
	inline SelfRef			normalize_rgb( )
	{
		VERIFY(magnitude_sqr_rgb( ) > EPS_S);
		return mul_rgb(1.0f / magnitude_rgb( ));
	}
	inline SelfRef			normalize_rgb(SelfCRef c)
	{
		VERIFY(c.magnitude_sqr_rgb( ) > EPS_S);
		return mul_rgb(c, 1.0f / c.magnitude_rgb( ));
	}
	inline SelfRef			lerp(SelfCRef c1, SelfCRef c2, TYPE t)
	{
		TYPE invt = 1.0f - t;
		r = c1.r * invt + c2.r * t;
		g = c1.g * invt + c2.g * t;
		b = c1.b * invt + c2.b * t;
		a = c1.a * invt + c2.a * t;
		return *this;
	}
	inline SelfRef			lerp(SelfCRef c1, SelfCRef c2, SelfCRef c3, TYPE t)
	{
		if (t > 0.5f)
		{
			return lerp(c2, c3, t * 2.0f - 1.0f);
		}
		else
		{
			return lerp(c1, c2, t * 2.0f);
		}
	}
	inline BOOL				similar_rgba(SelfCRef v, TYPE E = EPS_L) const
	{
		return _abs(r - v.r) < E && _abs(g - v.g) < E && _abs(b - v.b) < E && _abs(a - v.a) < E;
	}
	inline BOOL				similar_rgb(SelfCRef v, TYPE E = EPS_L) const
	{
		return _abs(r - v.r) < E && _abs(g - v.g) < E && _abs(b - v.b) < E;
	}
};

using Fcolor = _color<float>;

template <class T>
BOOL						_valid(const _color<T>& c)
{
	return _valid(c.r) && _valid(c.g) && _valid(c.b) && _valid(c.a);
}
