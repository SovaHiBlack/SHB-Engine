#pragma once

#include "physicsshellholder.h"

class CPHLeaderGeomShell;
class CPHCharacter;
struct dContact;
struct SGameMtl;

class CClimableObject: public CPhysicsShellHolder 
#ifdef DEBUG
,public pureRender
#endif
{
	typedef	CPhysicsShellHolder		inherited;
	CPHLeaderGeomShell* m_pStaticShell;
	Fobb				m_box;
	Fvector				m_axis;
	Fvector				m_side;
	Fvector				m_norm;
	f32				m_radius;
public:
					CClimableObject		();
					~CClimableObject	();
	virtual void	Load				(pcstr section);
	virtual BOOL	net_Spawn			( CSE_Abstract* DC);
	virtual	void	net_Destroy			();
	virtual void	shedule_Update		( u32 dt);							// Called by sheduler
	virtual void	UpdateCL			( );								// Called each frame, so no need for dt
	virtual void	Center				(Fvector &C) const;
	virtual f32		Radius				() const;
#ifdef DEBUG
	virtual void	OnRender			();
#endif
protected:
	virtual BOOL	UsedAI_Locations	();
public:
	const Fvector&	Axis				()const		{return m_axis;}
	f32			DDAxis				(Fvector& dir)const;

	const Fvector&	Side				()const {return m_side;}
	f32			DDSide				(Fvector& dir)const;

	const Fvector&	Norm				()const{return m_norm;} 
	f32			DDNorm				(Fvector &dir)const;
	bool			BeforeLadder		(CPHCharacter *actor, f32 tolerance=0.f)const;
	f32			DDLowerP			(CPHCharacter	*actor,Fvector &out_dir)const;//returns distance and dir to lover point
	f32			DDUpperP			(CPHCharacter	*actor,Fvector &out_dir)const;//returns distance and dir to upper point

	void			DToAxis				(CPHCharacter	*actor,Fvector &dir)const;
	f32			DDToAxis			(CPHCharacter	*actor,Fvector &out_dir)const;//returns distance and dir to ladder axis
	void			POnAxis				(CPHCharacter	*actor,Fvector	&P)const;
	
	f32			AxDistToUpperP		(CPHCharacter	*actor)const;
	f32			AxDistToLowerP		(CPHCharacter	*actor)const;

	void			DSideToAxis			(CPHCharacter	*actor,Fvector	&dir)const;
	f32			DDSideToAxis		(CPHCharacter	*actor,Fvector	&dir)const;
	
	void			DToPlain			(CPHCharacter	*actor,Fvector	&dist)const;
	f32			DDToPlain			(CPHCharacter	*actor,Fvector	&dir)const;
	bool			InRange				(CPHCharacter	*actor)const;
	bool			InTouch				(CPHCharacter	*actor)const;

	void			LowerPoint			(Fvector	&P)const;
	void			UpperPoint			(Fvector	&P)const;
	void			DefineClimbState	(CPHCharacter	*actor)const;
	static 	void	ObjectContactCallback(bool& /**do_colide/**/,bool bo1,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/);

public:
	virtual bool	register_schedule	() const {return false;}
};
