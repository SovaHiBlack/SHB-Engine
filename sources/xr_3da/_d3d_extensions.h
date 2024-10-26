#pragma once

#ifndef NO_XR_LIGHT
struct Flight 
{
public:
	u32           type;             /* Type of light source */
	fColor          diffuse;          /* Diffuse color of light */
	fColor          specular;         /* Specular color of light */
	fColor          ambient;          /* Ambient color of light */
	Fvector         position;         /* Position in world space */
	Fvector         direction;        /* Direction in world space */
	f32		    range;            /* Cutoff range */
	f32	        falloff;          /* Falloff */
	f32	        attenuation0;     /* Constant attenuation */
	f32	        attenuation1;     /* Linear attenuation */
	f32	        attenuation2;     /* Quadratic attenuation */
	f32	        theta;            /* Inner angle of spotlight cone */
	f32	        phi;              /* Outer angle of spotlight cone */

	IC	void		set	(u32 ltType, f32 x, f32 y, f32 z)
	{
		ZeroMemory( this, sizeof(Flight) );
		type=ltType;
		diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
		specular.set(diffuse);
		position.set(x,y,z);
		direction.set(x,y,z);
		direction.normalize_safe();
		range= _sqrt(flt_max);
	}
	IC	void		mul		(f32 brightness){
		diffuse.mul_rgb		(brightness);
		ambient.mul_rgb		(brightness);
		specular.mul_rgb	(brightness);
	}
};

/*
#if sizeof(Flight)!=sizeof(D3DLIGHT9)
#error Different structure size
#endif
*/

#endif

#ifndef NO_XR_MATERIAL
struct Fmaterial
{
public:
	fColor			diffuse;        /* Diffuse color RGBA */
	fColor			ambient;        /* Ambient color RGB */
	fColor		    specular;       /* Specular 'shininess' */
	fColor			emissive;       /* Emissive color RGB */
	f32			power;          /* Sharpness if specular highlight */

	IC	void		set	(f32 r, f32 g, f32 b)
	{
		ZeroMemory	(this, sizeof(Fmaterial));
		diffuse.r = ambient.r = r;
		diffuse.g = ambient.g = g;
		diffuse.b = ambient.b = b;
		diffuse.a = ambient.a = 1.0f;
		power	  = 0;
	}
	IC	void	set(f32 r, f32 g, f32 b, f32 a)
	{
		ZeroMemory	(this, sizeof(Fmaterial));
		diffuse.r = ambient.r = r;
		diffuse.g = ambient.g = g;
		diffuse.b = ambient.b = b;
		diffuse.a = ambient.a = a;
		power	  = 0;
	}
	IC	void	set	(fColor& c)
	{
		ZeroMemory	( this, sizeof(Fmaterial) );
		diffuse.r = ambient.r = c.r;
		diffuse.g = ambient.g = c.g;
		diffuse.b = ambient.b = c.b;
		diffuse.a = ambient.a = c.a;
		power	  = 0;
	}
};

/*
#if sizeof(Fmaterial)!=sizeof(D3DMATERIAL9) 
#error Different structure size
#endif
*/

#endif

#ifndef NO_XR_VDECLARATOR
struct	VDeclarator	: public svector<D3DVERTEXELEMENT9, MAXD3DDECLLENGTH+1>
{
	void	set		(u32 FVF)
	{
		D3DXDeclaratorFromFVF	(FVF,begin());
		resize					(D3DXGetDeclLength(begin())+1);
	}
	void	set		(D3DVERTEXELEMENT9* dcl)
	{
		resize					(D3DXGetDeclLength(dcl)+1);
		CopyMemory				(begin(),dcl,size()*sizeof(D3DVERTEXELEMENT9));
	}
	void	set		(const VDeclarator& d)
	{
		*this		= d;
	}
	u32		vertex	()				{ return D3DXGetDeclVertexSize(begin(),0);	}
	BOOL	equal	(VDeclarator& d)
	{
		if (size()!=d.size())	return false;
		else					return 0==memcmp(begin(),d.begin(),size()*sizeof(D3DVERTEXELEMENT9));
	}
};
#endif
