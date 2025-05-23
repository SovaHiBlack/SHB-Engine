#include "stdafx.h"

#include "..\XR_3DA\HierrarhyVisual.h"
#include "..\XR_3DA\SkeletonCustom.h"
#include "..\XR_3DA\mesh.h"
#include "..\XR_3DA\irenderable.h"

#include "flod.h"
#include "particlegroup.h"
#include "FTreeVisual.h"

using	namespace R_dsgraph;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Scene graph actual insertion and sorting ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
f32		r_ssaDISCARD;
f32		r_ssaDONTSORT;
f32		r_ssaLOD_A;
f32		r_ssaLOD_B;
f32		r_ssaGLOD_start;
f32		r_ssaGLOD_end;
f32		r_ssaHZBvsTEX;

ICF	f32	CalcSSA				(f32& distSQ, fVector3& C, IRenderVisual* V)
{
	f32 R	= V->vis.sphere.R + 0;
	distSQ	= Device.vCameraPosition.distance_to_sqr(C)+ EPSILON_5;
	return	R/distSQ;
}
ICF	f32	CalcSSA				(f32& distSQ, fVector3& C, f32 R)
{
	distSQ	= Device.vCameraPosition.distance_to_sqr(C)+ EPSILON_5;
	return	R/distSQ;
}

void R_dsgraph_structure::r_dsgraph_insert_dynamic(IRenderVisual* pVisual, fVector3& Center)
{
	CRender&	RI			=	RImplementation;

	if (pVisual->vis.marker	==	RI.marker)	return	;
	pVisual->vis.marker		=	RI.marker			;

#if RENDER==R_R1
	if (RI.o.vis_intersect &&	(pVisual->vis.accept_frame!=Device.dwFrame))	return;
	pVisual->vis.accept_frame	=	Device.dwFrame	;
#endif

	f32 distSQ			;
	f32 SSA				=	CalcSSA		(distSQ,Center,pVisual);
	if (SSA<=r_ssaDISCARD)		return;

	// Distortive geometry should be marked and R2 special-cases it
	// a) Allow to optimize RT order
	// b) Should be rendered to special distort buffer in another pass
	VERIFY						(pVisual->shader._get());
	ShaderElement*		sh_d	= &*pVisual->shader->E[4];
	if (RImplementation.o.distortion && sh_d && sh_d->flags.bDistort && pmask[sh_d->flags.iPriority/2]) {
		mapSorted_Node* N		= mapDistort.insertInAnyWay	(distSQ);
		N->val.ssa				= SSA;
		N->val.pObject			= RI.val_pObject;
		N->val.pVisual			= pVisual;
		N->val.Matrix			= *RI.val_pTransform;
		N->val.se				= sh_d;		// 4=L_special
	}

	// Select shader
	ShaderElement*	sh		=	RImplementation.rimp_select_sh_dynamic	(pVisual,distSQ);
	if (0==sh)								return;
	if (!pmask[sh->flags.iPriority/2])		return;

	// Create common node
	// NOTE: Invisible elements exist only in R1
	_MatrixItem		item	= {SSA,RI.val_pObject,pVisual,*RI.val_pTransform};

	// HUD rendering
	if (RI.val_bHUD)			{
		if (sh->flags.bStrictB2F)	{
			mapSorted_Node* N		= mapSorted.insertInAnyWay	(distSQ);
			N->val.ssa				= SSA;
			N->val.pObject			= RI.val_pObject;
			N->val.pVisual			= pVisual;
			N->val.Matrix			= *RI.val_pTransform;
			N->val.se				= sh;
			return;
		} else {
			mapHUD_Node* N			= mapHUD.insertInAnyWay		(distSQ);
			N->val.ssa				= SSA;
			N->val.pObject			= RI.val_pObject;
			N->val.pVisual			= pVisual;
			N->val.Matrix			= *RI.val_pTransform;
			N->val.se				= sh;
			return;
		}
	}

	// Shadows registering
#if RENDER==R_R1
	RI.L_Shadows->add_element	(item);
#endif
	if (RI.val_bInvisible)		return;

	// strict-sorting selection
	if (sh->flags.bStrictB2F)	{
		mapSorted_Node* N		= mapSorted.insertInAnyWay	(distSQ);
		N->val.ssa				= SSA;
		N->val.pObject			= RI.val_pObject;
		N->val.pVisual			= pVisual;
		N->val.Matrix			= *RI.val_pTransform;
		N->val.se				= sh;
		return;
	}

#if RENDER==R_R2
	// Emissive geometry should be marked and R2 special-cases it
	// a) Allow to skeep already lit pixels
	// b) Allow to make them 100% lit and really bright
	// c) Should not cast shadows
	// d) Should be rendered to accumulation buffer in the second pass
	if (sh->flags.bEmissive) {
		mapSorted_Node* N		= mapEmissive.insertInAnyWay	(distSQ);
		N->val.ssa				= SSA;
		N->val.pObject			= RI.val_pObject;
		N->val.pVisual			= pVisual;
		N->val.Matrix			= *RI.val_pTransform;
		N->val.se				= &*pVisual->shader->E[4];		// 4=L_special
	}
	if (sh->flags.bWmark	&& pmask_wmark)	{
		mapSorted_Node* N		= mapWmark.insertInAnyWay		(distSQ);
		N->val.ssa				= SSA;
		N->val.pObject			= RI.val_pObject;
		N->val.pVisual			= pVisual;
		N->val.Matrix			= *RI.val_pTransform;
		N->val.se				= sh;							
		return					;
	}
#endif

	// the most common node
	SPass&						pass	= *sh->passes.front	();
	mapMatrix_T&				map		= mapMatrix			[sh->flags.iPriority/2];
#ifdef USE_RESOURCE_DEBUGGER
	mapMatrixVS::TNode*			Nvs		= map.insert		(pass.vs);
	mapMatrixPS::TNode*			Nps		= Nvs->val.insert	(pass.ps);
#else
	mapMatrixVS::TNode*			Nvs		= map.insert		(pass.vs->vs);
	mapMatrixPS::TNode*			Nps		= Nvs->val.insert	(pass.ps->ps);
#endif
	mapMatrixCS::TNode*			Ncs		= Nps->val.insert	(pass.constants._get());
	mapMatrixStates::TNode*		Nstate	= Ncs->val.insert	(pass.state->state);
	mapMatrixTextures::TNode*	Ntex	= Nstate->val.insert(pass.T._get());
	mapMatrixItems&				items	= Ntex->val;
	items.push_back						(item);

	// Need to sort for HZB efficient use
	if (SSA>Ntex->val.ssa)		{ Ntex->val.ssa = SSA;
	if (SSA>Nstate->val.ssa)	{ Nstate->val.ssa = SSA;
	if (SSA>Ncs->val.ssa)		{ Ncs->val.ssa = SSA;
	if (SSA>Nps->val.ssa)		{ Nps->val.ssa = SSA;
	if (SSA>Nvs->val.ssa)		{ Nvs->val.ssa = SSA;
	} } } } }

#if RENDER==R_R2
	if (val_recorder)			{
		fBox3		temp		;
		fMatrix4x4&	xf			= *RI.val_pTransform;
		temp.xform	(pVisual->vis.box,xf);
		val_recorder->push_back	(temp);
	}
#endif
}

void R_dsgraph_structure::r_dsgraph_insert_static(IRenderVisual* pVisual)
{
	CRender&	RI				=	RImplementation;

	if (pVisual->vis.marker		==	RI.marker)	return	;
	pVisual->vis.marker			=	RI.marker			;

#if RENDER==R_R1
	if (RI.o.vis_intersect &&	(pVisual->vis.accept_frame!=Device.dwFrame))	return;
	pVisual->vis.accept_frame	=	Device.dwFrame		;
#endif

	f32 distSQ;
	f32 SSA					=	CalcSSA		(distSQ,pVisual->vis.sphere.P,pVisual);
	if (SSA<=r_ssaDISCARD)		return;

	// Distortive geometry should be marked and R2 special-cases it
	// a) Allow to optimize RT order
	// b) Should be rendered to special distort buffer in another pass
	VERIFY						(pVisual->shader._get());
	ShaderElement*		sh_d	= &*pVisual->shader->E[4];
	if (RImplementation.o.distortion && sh_d && sh_d->flags.bDistort && pmask[sh_d->flags.iPriority/2]) {
		mapSorted_Node* N		= mapDistort.insertInAnyWay		(distSQ);
		N->val.ssa				= SSA;
		N->val.pObject			= NULL;
		N->val.pVisual			= pVisual;
		N->val.Matrix			= Fidentity;
		N->val.se				= &*pVisual->shader->E[4];		// 4=L_special
	}

	// Select shader
	ShaderElement*		sh		= RImplementation.rimp_select_sh_static(pVisual,distSQ);
	if (0==sh)								return;
	if (!pmask[sh->flags.iPriority/2])		return;

	// strict-sorting selection
	if (sh->flags.bStrictB2F) {
		mapSorted_Node* N			= mapSorted.insertInAnyWay(distSQ);
		N->val.pObject				= NULL;
		N->val.pVisual				= pVisual;
		N->val.Matrix				= Fidentity;
		N->val.se					= sh;
		return;
	}

#if RENDER==R_R2
	// Emissive geometry should be marked and R2 special-cases it
	// a) Allow to skeep already lit pixels
	// b) Allow to make them 100% lit and really bright
	// c) Should not cast shadows
	// d) Should be rendered to accumulation buffer in the second pass
	if (sh->flags.bEmissive) {
		mapSorted_Node* N		= mapEmissive.insertInAnyWay	(distSQ);
		N->val.ssa				= SSA;
		N->val.pObject			= NULL;
		N->val.pVisual			= pVisual;
		N->val.Matrix			= Fidentity;
		N->val.se				= &*pVisual->shader->E[4];		// 4=L_special
	}
	if (sh->flags.bWmark	&& pmask_wmark)	{
		mapSorted_Node* N		= mapWmark.insertInAnyWay		(distSQ);
		N->val.ssa				= SSA;
		N->val.pObject			= NULL;
		N->val.pVisual			= pVisual;
		N->val.Matrix			= Fidentity;
		N->val.se				= sh;							
		return					;
	}
#endif

	if	(val_feedback && counter_S==val_feedback_breakp)	val_feedback->rfeedback_static(pVisual);

	counter_S					++;
	SPass&						pass	= *sh->passes.front	();
	mapNormal_T&				map		= mapNormal			[sh->flags.iPriority/2];
#ifdef USE_RESOURCE_DEBUGGER
	mapNormalVS::TNode*			Nvs		= map.insert		(pass.vs);
	mapNormalPS::TNode*			Nps		= Nvs->val.insert	(pass.ps);
#else
	mapNormalVS::TNode*			Nvs		= map.insert		(pass.vs->vs);
	mapNormalPS::TNode*			Nps		= Nvs->val.insert	(pass.ps->ps);
#endif
	mapNormalCS::TNode*			Ncs		= Nps->val.insert	(pass.constants._get());
	mapNormalStates::TNode*		Nstate	= Ncs->val.insert	(pass.state->state);
	mapNormalTextures::TNode*	Ntex	= Nstate->val.insert(pass.T._get());
	mapNormalItems&				items	= Ntex->val;
	_NormalItem					item	= {SSA,pVisual};
	items.push_back						(item);

	// Need to sort for HZB efficient use
	if (SSA>Ntex->val.ssa)		{ Ntex->val.ssa = SSA;
	if (SSA>Nstate->val.ssa)	{ Nstate->val.ssa = SSA;
	if (SSA>Ncs->val.ssa)		{ Ncs->val.ssa = SSA;
	if (SSA>Nps->val.ssa)		{ Nps->val.ssa = SSA;
	if (SSA>Nvs->val.ssa)		{ Nvs->val.ssa = SSA;
	} } } } }

#if RENDER==R_R2
	if (val_recorder)			{
		val_recorder->push_back	(pVisual->vis.box	);
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
void CRender::add_leafs_Dynamic(IRenderVisual* pVisual)
{
	if (0==pVisual)				return;

	// Visual is 100% visible - simply add it
	xr_vector<IRenderVisual*>::iterator I,E;	// it may be useful for 'hierrarhy' visual

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pG	= (PS::CParticleGroup*)pVisual;
			for (PS::CParticleGroup::SItemVecIt i_it=pG->items.begin(); i_it!=pG->items.end(); i_it++)	{
				PS::CParticleGroup::SItem&			I		= *i_it;
				if (I._effect)		add_leafs_Dynamic		(I._effect);
				for (xr_vector<IRenderVisual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_leafs_Dynamic(*pit);
				for (xr_vector<IRenderVisual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_leafs_Dynamic(*pit);
			}
		}
		return;
	case MT_HIERRARHY:
		{
			// Add all children, doesn't perform any tests
		CHierrarhyVisual* pV = (CHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			for (; I!=E; I++)	add_leafs_Dynamic	(*I);
		}
		return;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV			= (CKinematics*)pVisual;
			BOOL	_use_lod			= FALSE	;
			if (pV->m_lod)				
			{
				fVector3							Tpos;
				f32		D;
				val_pTransform->transform_tiny	(Tpos, pV->vis.sphere.P);
				f32		ssa		=	CalcSSA	(D,Tpos,pV->vis.sphere.R/2.f);	// assume dynamics never consume full sphere
				if (ssa<r_ssaLOD_A)	_use_lod	= TRUE;
			}
			if (_use_lod)				
			{
				add_leafs_Dynamic			(pV->m_lod)		;
			} else {
				pV->CalculateBones			(TRUE);
				pV->CalculateWallmarks		();		//. bug?
				I = pV->children.begin		();
				E = pV->children.end		();
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
		}
		return;
	default:
		{
			// General type of visual
			// Calculate distance to it's center
		fVector3							Tpos;
			val_pTransform->transform_tiny	(Tpos, pVisual->vis.sphere.P);
			r_dsgraph_insert_dynamic		(pVisual,Tpos);
		}
		return;
	}
}

void CRender::add_leafs_Static(IRenderVisual* pVisual)
{
	if (!HOM.visible(pVisual->vis))		return;

	// Visual is 100% visible - simply add it
	xr_vector<IRenderVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pG = (PS::CParticleGroup*)pVisual;
			for (PS::CParticleGroup::SItemVecIt i_it=pG->items.begin(); i_it!=pG->items.end(); i_it++){
				PS::CParticleGroup::SItem&			I		= *i_it;
				if (I._effect)		add_leafs_Dynamic		(I._effect);
				for (xr_vector<IRenderVisual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_leafs_Dynamic(*pit);
				for (xr_vector<IRenderVisual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_leafs_Dynamic(*pit);
			}
		}
		return;
	case MT_HIERRARHY:
		{
			// Add all children, doesn't perform any tests
		CHierrarhyVisual* pV	= (CHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			for (; I!=E; I++)		add_leafs_Static (*I);
		}
		return;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV		= (CKinematics*)pVisual;
			pV->CalculateBones		(TRUE);
			I = pV->children.begin	();
			E = pV->children.end	();
			for (; I!=E; I++)		add_leafs_Static	(*I);
		}
		return;
	case MT_LOD:
		{
			FLOD		* pV	=		(FLOD*) pVisual;
			f32		D;
			f32		ssa		=		CalcSSA(D,pV->vis.sphere.P,pV);
			ssa					*=		pV->lod_factor;
			if (ssa<r_ssaLOD_A)
			{
				if (ssa<r_ssaDISCARD)	return;
				mapLOD_Node*	N	=	mapLOD.insertInAnyWay(D);
				N->val.ssa			=	ssa;
				N->val.pVisual		=	pVisual;
			}
			if (ssa>r_ssaLOD_B)
			{
				// Add all children, doesn't perform any tests
				I = pV->children.begin	();
				E = pV->children.end	();
				for (; I!=E; I++)	add_leafs_Static (*I);
			}
		}
		return;
	case MT_TREE_PM:
	case MT_TREE_ST:
		{
			// General type of visual
			r_dsgraph_insert_static		(pVisual);
		}
		return;
	default:
		{
			// General type of visual
			r_dsgraph_insert_static		(pVisual);
		}
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRender::add_Dynamic(IRenderVisual* pVisual, u32 planes)
{
	// Check frustum visibility and calculate distance to visual's center
	fVector3		Tpos;	// transformed position
	EFC_Visible	VIS;

	val_pTransform->transform_tiny	(Tpos, pVisual->vis.sphere.P);
	VIS = View->testSphere			(Tpos, pVisual->vis.sphere.R,planes);
	if (fcvNone==VIS) return FALSE	;

	// If we get here visual is visible or partially visible
	xr_vector<IRenderVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pG = (PS::CParticleGroup*)pVisual;
			for (PS::CParticleGroup::SItemVecIt i_it=pG->items.begin(); i_it!=pG->items.end(); i_it++)
			{
				PS::CParticleGroup::SItem&			I		= *i_it;
				if (fcvPartial==VIS) 
				{
					if (I._effect)		add_Dynamic				(I._effect,planes);
					for (xr_vector<IRenderVisual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_Dynamic(*pit,planes);
					for (xr_vector<IRenderVisual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_Dynamic(*pit,planes);
				} else 
				{
					if (I._effect)		add_leafs_Dynamic		(I._effect);
					for (xr_vector<IRenderVisual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_leafs_Dynamic(*pit);
					for (xr_vector<IRenderVisual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_leafs_Dynamic(*pit);
				}
			}
		}
		break;
	case MT_HIERRARHY:
		{
			// Add all children
		CHierrarhyVisual* pV = (CHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			if (fcvPartial==VIS) 
			{
				for (; I!=E; I++)	add_Dynamic			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
		}
		break;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV			= (CKinematics*)pVisual;
			BOOL	_use_lod			= FALSE	;
			if (pV->m_lod)				
			{
				fVector3							Tpos;
				f32		D;
				val_pTransform->transform_tiny	(Tpos, pV->vis.sphere.P);
				f32		ssa		=	CalcSSA	(D,Tpos,pV->vis.sphere.R/2.f);	// assume dynamics never consume full sphere
				if (ssa<r_ssaLOD_A)	_use_lod	= TRUE		;
			}
			if (_use_lod)
			{
				add_leafs_Dynamic			(pV->m_lod)		;
			} else 
			{
				pV->CalculateBones			(TRUE);
				pV->CalculateWallmarks		();		//. bug?
				I = pV->children.begin		();
				E = pV->children.end		();
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
			/*
			I = pV->children.begin		();
			E = pV->children.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Dynamic			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
			*/
		}
		break;
	default:
		{
			// General type of visual
			r_dsgraph_insert_dynamic(pVisual,Tpos);
		}
		break;
	}
	return TRUE;
}

void CRender::add_Static(IRenderVisual* pVisual, u32 planes)
{
	// Check frustum visibility and calculate distance to visual's center
	EFC_Visible	VIS;
	vis_data&	vis			= pVisual->vis;
	VIS = View->testSAABB	(vis.sphere.P,vis.sphere.R,vis.box.data(),planes);
	if (fcvNone==VIS)		return;
	if (!HOM.visible(vis))	return;

	// If we get here visual is visible or partially visible
	xr_vector<IRenderVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pG = (PS::CParticleGroup*)pVisual;
			for (PS::CParticleGroup::SItemVecIt i_it=pG->items.begin(); i_it!=pG->items.end(); i_it++){
				PS::CParticleGroup::SItem&			I		= *i_it;
				if (fcvPartial==VIS) {
					if (I._effect)		add_Dynamic				(I._effect,planes);
					for (xr_vector<IRenderVisual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_Dynamic(*pit,planes);
					for (xr_vector<IRenderVisual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_Dynamic(*pit,planes);
				} else {
					if (I._effect)		add_leafs_Dynamic		(I._effect);
					for (xr_vector<IRenderVisual*>::iterator pit = I._children_related.begin();	pit!=I._children_related.end(); pit++)	add_leafs_Dynamic(*pit);
					for (xr_vector<IRenderVisual*>::iterator pit = I._children_free.begin();		pit!=I._children_free.end();	pit++)	add_leafs_Dynamic(*pit);
				}
			}
		}
		break;
	case MT_HIERRARHY:
		{
			// Add all children
		CHierrarhyVisual* pV = (CHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Static			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV		= (CKinematics*)pVisual;
			pV->CalculateBones		(TRUE);
			I = pV->children.begin	();
			E = pV->children.end	();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Static			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_LOD:
		{
			FLOD		* pV	= (FLOD*) pVisual;
			f32		D;
			f32		ssa		= CalcSSA	(D,pV->vis.sphere.P,pV);
			ssa					*= pV->lod_factor;
			if (ssa<r_ssaLOD_A)	
			{
				if (ssa<r_ssaDISCARD)	return;
				mapLOD_Node*	N		= mapLOD.insertInAnyWay(D);
				N->val.ssa				= ssa;
				N->val.pVisual			= pVisual;
			}
			if (ssa>r_ssaLOD_B)
			{
				// Add all children, perform tests
				I = pV->children.begin	();
				E = pV->children.end	();
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_TREE_ST:
	case MT_TREE_PM:
		{
			// General type of visual
			r_dsgraph_insert_static		(pVisual);
		}
		return;
	default:
		{
			// General type of visual
			r_dsgraph_insert_static		(pVisual);
		}
		break;
	}
}
