#pragma once

//-----------------------------------------------------------------------------
#pragma pack(push,4)
namespace FVF {
	struct L {
		Fvector3		p;
		unsigned int			color;
		inline void		set(const L& src) {*this = src;};
		inline void		set(float x, float y, float z, unsigned int C) { p.set(x,y,z); color=C; }
		inline void		set(const Fvector3& _p, unsigned int C) { p.set(_p); color=C; }
	};
	const unsigned int F_L		= D3DFVF_XYZ | D3DFVF_DIFFUSE;

	struct V {
		Fvector3		p;
		Fvector2	t;
		inline void		set(const V& src) {*this = src;};
		inline void		set(float x, float y, float z, float u, float v)	{ p.set(x,y,z); t.set(u,v);}
		inline void		set(const Fvector3& _p,float u, float v)				{ p.set(_p);	t.set(u,v);}
	};
	const unsigned int F_V		= D3DFVF_XYZ | D3DFVF_TEX1;

	struct LIT {
		Fvector3		p;
		unsigned int			color;
		Fvector2	t;
		inline void		set(const LIT& src) {*this = src;};
		inline void		set(float x, float y, float z, unsigned int C, float u, float v) { p.set(x,y,z); color=C; t.set(u,v);}
		inline void		set(const Fvector3& _p, unsigned int C, float u, float v) { p.set(_p); color=C; t.set(u,v);}
	};
	const unsigned int F_LIT	= D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

	struct TL0uv {
		Fvector4	p;
		unsigned int			color;
		inline void	set	(const TL0uv& src)
		{	*this = src; };
		inline void	set	(float x, float y, unsigned int c)
		{	set	(x,y,.0001f,.9999f,c); };
		inline void	set	(int x, int y, unsigned int c)
		{	set	(float(x),float(y),.0001f,.9999f,c); };
		inline void	set	(float x, float y, float z, float w, unsigned int c)
		{	p.set	(x,y,z,w); color = c; };
		inline void transform(const Fvector3& v,const Fmatrix &matSet)
		{
			// Transform it through the matrix set. Takes in mean projection.
			// Finally, scale the vertices to screen coords.
			// Note 1: device coords range from -1 to +1 in the viewport.
			// Note 2: the p.z-coordinate will be used in the z-buffer.
			p.w =   matSet._14*v.x + matSet._24*v.y + matSet._34*v.z + matSet._44;
			p.x	=  (matSet._11*v.x + matSet._21*v.y + matSet._31*v.z + matSet._41)/p.w;
			p.y	= -(matSet._12*v.x + matSet._22*v.y + matSet._32*v.z + matSet._42)/p.w;
			p.z	=  (matSet._13*v.x + matSet._23*v.y + matSet._33*v.z + matSet._43)/p.w;
		};
	};
	const unsigned int F_TL0uv	= D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

	struct TL {
		Fvector4	p;
		unsigned int			color;
		Fvector2	uv;
		inline void	set	(const TL& src)
		{	*this = src; };
		inline void	set	(float x, float y, unsigned int c, Fvector2& t)
		{	set	(x,y,.0001f,.9999f,c,t.x,t.y); };
		inline void	set	(float x, float y, unsigned int c, float u, float v)
		{	set	(x,y,.0001f,.9999f,c,u,v); };
		inline void	set	(int x, int y, unsigned int c, float u, float v)
		{	set	(float(x),float(y),.0001f,.9999f,c,u,v); };
		inline void	set	(float x, float y, float z, float w, unsigned int c, float u, float v)
		{	p.set	(x,y,z,w); color = c;	uv.x=u; uv.y=v;	};
		inline void transform(const Fvector3& v,const Fmatrix &matSet)
		{
			// Transform it through the matrix set. Takes in mean projection.
			// Finally, scale the vertices to screen coords.
			// Note 1: device coords range from -1 to +1 in the viewport.
			// Note 2: the p.z-coordinate will be used in the z-buffer.
			p.w =   matSet._14*v.x + matSet._24*v.y + matSet._34*v.z + matSet._44;
			p.x	=  (matSet._11*v.x + matSet._21*v.y + matSet._31*v.z + matSet._41)/p.w;
			p.y	= -(matSet._12*v.x + matSet._22*v.y + matSet._32*v.z + matSet._42)/p.w;
			p.z	=  (matSet._13*v.x + matSet._23*v.y + matSet._33*v.z + matSet._43)/p.w;
		};
	};
	const unsigned int F_TL	= D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

	struct TL2uv {
		Fvector4	p;
		unsigned int			color;
		Fvector2	uv[2];
		inline void	set	(const TL2uv& src)
		{	*this = src; };
		inline void	set	(float x, float y, unsigned int c, Fvector2& t0, Fvector2& t1)
		{	set	(x,y,.0001f,.9999f,c,t0.x,t0.y,t1.x,t1.y);	};
		inline void	set	(float x, float y, float z, float w, unsigned int c, Fvector2& t0, Fvector2& t1)
		{	set	(x,y,z,w,c,t0.x,t0.y,t1.x,t1.y);			};
		inline void	set	(float x, float y, unsigned int c, float u, float v, float u2, float v2)
		{	set	(x,y,.0001f,.9999f,c,u,v,u2,v2); };
		inline void	set	(int x, int y, unsigned int c, float u, float v, float u2, float v2)
		{	set	(float(x),float(y),.0001f,.9999f,c,u,v,u2,v2); };
		inline void	set	(float x, float y, float z, float w, unsigned int c, float u, float v, float u2, float v2)
		{	p.set	(x,y,z,w); color = c; uv[0].x=u; uv[0].y=v;	uv[1].x=u2; uv[1].y=v2;	};
		inline void transform(const Fvector3& v,const Fmatrix &matSet)
		{
			// Transform it through the matrix set. Takes in mean projection.
			// Finally, scale the vertices to screen coords.
			// Note 1: device coords range from -1 to +1 in the viewport.
			// Note 2: the p.z-coordinate will be used in the z-buffer.
			p.w =   matSet._14*v.x + matSet._24*v.y + matSet._34*v.z + matSet._44;
			p.x	=  (matSet._11*v.x + matSet._21*v.y + matSet._31*v.z + matSet._41)/p.w;
			p.y	= -(matSet._12*v.x + matSet._22*v.y + matSet._32*v.z + matSet._42)/p.w;
			p.z	=  (matSet._13*v.x + matSet._23*v.y + matSet._33*v.z + matSet._43)/p.w;
		};
	};
	const unsigned int F_TL2uv	= D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2;

	struct TL4uv {
		Fvector4	p;
		unsigned int			color;
		Fvector2	uv[4];
		inline void	set	(const TL4uv& src)
		{	*this = src; };
		inline void	set	(float x, float y, unsigned int c, Fvector2& t0, Fvector2& t1)
		{	set	(x,y,.0001f,.9999f,c,t0.x,t0.y,t1.x,t1.y);	};
		inline void	set	(float x, float y, float z, float w, unsigned int c, Fvector2& t0, Fvector2& t1)
		{	set	(x,y,z,w,c,t0.x,t0.y,t1.x,t1.y);			};
		inline void	set	(float x, float y, unsigned int c, float u, float v, float u2, float v2)
		{	set	(x,y,.0001f,.9999f,c,u,v,u2,v2); };
		inline void	set	(int x, int y, unsigned int c, float u, float v, float u2, float v2)
		{	set	(float(x),float(y),.0001f,.9999f,c,u,v,u2,v2); };
		inline void	set	(float x, float y, float z, float w, unsigned int c, float u, float v, float u2, float v2)
		{	p.set	(x,y,z,w); color = c; uv[0].x=u; uv[0].y=v;	uv[1].x=u2; uv[1].y=v2;	};
	};
	const unsigned int F_TL4uv	= D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4;
};
#pragma pack(pop)
