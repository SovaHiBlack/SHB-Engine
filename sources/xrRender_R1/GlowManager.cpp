// GlowManager.cpp: implementation of the CGlowManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\XR_3DA\igame_persistent.h"
#include "..\XR_3DA\environment.h"
#include "..\XR_3DA\x_ray.h"
#include "..\XR_3DA\GameFont.h"
#include "GlowManager.h"
#include "..\XR_3DA\Object.h"

#define FADE_SCALE_UP		4096.f
#define FADE_SCALE_DOWN		1024.f
#define MAX_GlowsDist1		f32(g_pGamePersistent->Environment().CurrentEnv.far_plane)
#define MAX_GlowsDist2		f32(MAX_GlowsDist1*MAX_GlowsDist1)


//////////////////////////////////////////////////////////////////////
CGlow::CGlow		()		: ISpatial(g_SpatialSpace)
{
	flags.bActive	= false;
	position.set	(0,0,0);
	direction.set	(0,0,0);
	radius			= 0.1f;
	color.set		(1,1,1,1);
	bTestResult		= FALSE;
	fade			= 1.f;
	dwFrame			= 0;
	spatial.type	= STYPE_RENDERABLE;
}
CGlow::~CGlow()
{
	set_active		(false);
	shader.destroy	();
}

void	CGlow::set_active		(bool a)				
{
	if (a)
	{
		if (flags.bActive)					return;
		flags.bActive						= true;
		spatial_register					();
	}
	else
	{
		if (!flags.bActive)					return;
		flags.bActive						= false;
		spatial_unregister					();
	}
}

bool	CGlow::get_active		()					{return flags.bActive;}
void	CGlow::set_position		(const fVector3& P)	{
	if (position.similar(P))	return;
	position.set				(P);
	spatial_move				();
};
void	CGlow::set_direction	(const fVector3& D)	{
	direction.normalize_safe	(D);
};
void	CGlow::set_radius		(f32 R)			{
	if (fsimilar(radius,R))		return;
	radius						= R;
	spatial_move				();
};
void	CGlow::set_texture		(pcstr name)		{
	shader.create				("effects\\glow",name);
}
void	CGlow::set_color		(const fColor& C)	{
	color						= C;
}
void	CGlow::set_color		(f32 r, f32 g, f32 b)	{
	color.set					(r,g,b,1);
}
void	CGlow::spatial_move		()
{
	spatial.sphere.set			(position,radius);
	ISpatial::spatial_move		();
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlowManager::CGlowManager()
{
}

CGlowManager::~CGlowManager	()
{
}

void CGlowManager::Load		(IReader* fs)
{
	// glows itself
	u32 size	= fs->length();
	R_ASSERT	(size);
	u32 one		= 4*sizeof(f32)+1*sizeof(u16);
	R_ASSERT	(size%one == 0);
	u32 count	= size/one;
	Glows.reserve(count);

	for (;count;count--)
	{
		CGlow* G			= xr_new<CGlow>();
		fs->r				(&G->position,	3*sizeof(f32));
		fs->r				(&G->radius,	1*sizeof(f32));
		G->spatial.sphere.set(G->position, G->radius);
		G->direction.set	( 0,0,0 );

		u16 S				= fs->r_u16();
		G->shader			= ::RImplementation.getShader(S);

		G->fade				= 255.f;
		G->dwFrame			= 0x0;
		G->bTestResult		= TRUE;

		G->spatial.type		= STYPE_RENDERABLE;

		G->set_active		(true);

		Glows.push_back		(G);
	}
	dwTestID		= 0;

	hGeom.create	(FVF::F_LIT,RCache.Vertex.Buffer(),RCache.QuadIB);
}

void CGlowManager::Unload	()
{
	// glows
	SelectedToTest_2.clear	();
	SelectedToTest_1.clear	();
	SelectedToTest_0.clear	();
	Selected.clear			();
	Glows.clear				();
}

IC bool glow_compare	(ref_glow g1, ref_glow g2)
{	return ((CGlow*)g1._get())->shader < ((CGlow*)g2._get())->shader; }

void CGlowManager::add	(ref_glow G_)
{
	CGlow*	G		= (CGlow*)	G_._get		();
	if (G->dwFrame	==Device.dwFrame)		return;
	G->dwFrame		= Device.dwFrame;
#ifdef DEBUG
	Device.Statistic->RenderDUMP_Glows.Begin();
#endif

	f32	dt		= Device.fTimeDelta;
	f32	dlim2	= MAX_GlowsDist2;

	f32	range = Device.vCameraPosition.distance_to_sqr	(G->spatial.sphere.P);
	if (range < dlim2) 
	{
		// 2. Use result of test
		if (G->bTestResult) {
			G->fade -= dt*FADE_SCALE_DOWN;
			if (G->fade<1.)		G->fade = 1;
		} else {
			G->fade	+= dt*FADE_SCALE_UP;
			if (G->fade>255.f)	G->fade = 255.f;
		}

		Selected.push_back(G);
		return;
	}
	G->fade -= dt*FADE_SCALE_DOWN;
	if (G->fade<1.) G->fade = 1;
#ifdef DEBUG
	Device.Statistic->RenderDUMP_Glows.End();
#endif
}

IC void FillSprite	(FVF::LIT*& pv, const fVector3& pos, f32 r, u32 clr)
{
	const fVector3& T 	= Device.vCameraTop;
	const fVector3& R 	= Device.vCameraRight;
	fVector3		Vr;
	fVector3 Vt;
	Vr.mul 		(R,r);
	Vt.mul		(T,r);

	fVector3 	a;
	fVector3 b;
	fVector3 c;
	fVector3 d;
	a.sub		(Vt,Vr);
	b.add		(Vt,Vr);
	c.invert	(a);
	d.invert	(b);
	pv->set		(d.x+pos.x,d.y+pos.y,d.z+pos.z, clr, 0.f,1.f);	pv++;
	pv->set		(a.x+pos.x,a.y+pos.y,a.z+pos.z, clr, 0.f,0.f);	pv++;
	pv->set		(c.x+pos.x,c.y+pos.y,c.z+pos.z, clr, 1.f,1.f);	pv++;
	pv->set		(b.x+pos.x,b.y+pos.y,b.z+pos.z,	clr, 1.f,0.f);	pv++;
}

void CGlowManager::Render			()
{
	if (Selected.empty())					return		;
	RCache.set_xform_world					(Fidentity)	;

	Device.Statistic->RenderDUMP_Glows.Begin	();
	render_sw								();
	Device.Statistic->RenderDUMP_Glows.End	();
}

void CGlowManager::render_sw		()
{
	// 0. save main view and disable
	CObject*	o_main		= g_pGameLevel->CurrentViewEntity();

	// 1. Test some number of glows
	fVector3 start	= Device.vCameraPosition;
	for (int i=0; i<ps_r1_GlowsPerFrame; i++,dwTestID++)
	{
		u32	ID		= dwTestID%Selected.size();
		CGlow&	G	= *( (CGlow*)Selected[ID]._get() );
		if (G.dwFrame=='test')	break;
		G.dwFrame	=	'test';
		fVector3		dir;
		dir.sub		(G.spatial.sphere.P,start);
		f32 range = dir.magnitude();
		if (range> EPSILON_7)	{
			dir.div		(range);
			G.bTestResult = g_pGameLevel->ObjectSpace.RayTest(start,dir,range,collide::rqtBoth,&G.RayCache,o_main);
		}
	}

	// 2. Render selected
	render_selected			();
}

void CGlowManager::render_hw		()
{
	// 0. query result from 'SelectedToTest_2'
	SelectedToTest_2		= SelectedToTest_1;
	SelectedToTest_1		= SelectedToTest_0;
	SelectedToTest_0.clear_not_free	();

	// 1. Sort into two parts - 1(selected-to-test)[to-test], 2(selected)[just-draw]
	// fVector3 &start	= Device.vCameraPosition;
	for (int i=0; (i<ps_r1_GlowsPerFrame) && Selected.size(); i++,dwTestID++)
	{
		u32	ID		= dwTestID%Selected.size();
		SelectedToTest_0.push_back	(Selected[ID]);
		Selected.erase	(Selected.begin()+ID);
	}

	// 2. Render selected
	render_selected			();

	// 
}

void CGlowManager::render_selected()
{
	// 2. Sort by shader
	std::sort		(Selected.begin(),Selected.end(),glow_compare);

	FVF::LIT		*pv;

	u32				pos = 0, count;
	ref_shader		T;

	fPlane3			NP;
	NP.build		(Device.vCameraPosition,Device.vCameraDirection);

	f32		dlim2	= MAX_GlowsDist2;
	for (;pos<Selected.size();) 
	{
		T		= ((CGlow*)Selected[pos]._get())->shader;
		count	= 0;
		while	((pos+count<Selected.size()) && (((CGlow*)Selected[pos+count]._get())->shader==T)) count++;

		u32		vOffset;
		u32		end		= pos+count;
		FVF::LIT* pvs	= pv = (FVF::LIT*) RCache.Vertex.Lock(count*4,hGeom->vb_stride,vOffset);
		for (; pos<end; pos++)
		{
			// Cull invisible 
			CGlow&	G					= *( (CGlow*)Selected[pos]._get() );
			if (G.fade<=1.f)			continue;

			// Now perform dotproduct if need it
			f32	scale = 1.0f;
			f32	dist_sq;
			fVector3 dir;
			dir.sub			(Device.vCameraPosition,G.position);
			dist_sq			= dir.square_magnitude();
			if (G.direction.square_magnitude()> EPSILON_5)	{
				dir.div			(_sqrt(dist_sq));
				scale			= dir.dotproduct(G.direction);
			}

			if (G.fade * scale <= 1.0f)
			{
				continue;
			}

			// near fade
			f32 dist_np	= NP.distance(G.position)-VIEWPORT_NEAR;
			f32 snear		= dist_np/0.15f;
			clamp	(snear,0.f,1.f);
			scale			*=	snear;
			if (G.fade*scale<=1.f)		continue;

			u32 C			= iFloor(G.fade*scale*(1-(dist_sq/dlim2)));
			u32 clr			= color_rgba(C,C,C,C);
			fVector3	gp;
					gp.mad	(G.position,dir,G.radius*scale);
			FillSprite		(pv,G.position,G.radius,clr);
		}

		int vCount				= int(pv-pvs);
		RCache.Vertex.Unlock	(vCount,hGeom->vb_stride);
		if (vCount) {
			RCache.set_Shader		(T);
			RCache.set_Geometry		(hGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,vCount/2);
		}
	}

	Selected.clear_not_free			();
}
