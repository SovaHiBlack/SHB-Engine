#pragma once

#include "ispatial.h"
#include "isheduled.h"
//#include "IInputReceiver.h"
#include "irenderable.h"
#include "ICollidable.h"

// refs
class	ENGINE_API	IRenderVisual;
class	ENGINE_API	IRender_Sector;
class	ENGINE_API	IRender_ObjectSpecific;
class	ENGINE_API	CCustomHUD;
class CNetPacket;
class	CSE_Abstract;

//-----------------------------------------------------------------------------------------------------------
#define CROW_RADIUS	(30.0f)
//-----------------------------------------------------------------------------------------------------------
//	CObject
//-----------------------------------------------------------------------------------------------------------
#pragma pack(push,4)
class ENGINE_API CObject : public DLL_Pure, public ISpatial, public ISheduled, public IRenderable, public ICollidable
{
public:
	struct SSavedPosition
	{
		u32				dwTime;
		fVector3		vPosition;
	};
	union UObjectProperties
	{
		struct
		{
			u32	net_ID : 16;
			u32	bActiveCounter : 8;
			u32	bEnabled : 1;
			u32	bVisible : 1;
			u32	bDestroy : 1;
			u32	net_Local : 1;
			u32	net_Ready : 1;
			u32 net_SV_Update : 1;
			u32 crow : 1;
			u32	bPreDestroy : 1;
		};
		u32	storage;
	};

private:
	// Some property variables
	UObjectProperties					Props;
	shared_str							NameObject;
	shared_str							NameSection;
	shared_str							NameVisual;

protected:
	// Parentness
	CObject* Parent;

	// Geometric (transformation)
	svector<SSavedPosition, 4>			PositionStack;

public:

#ifdef DEBUG
	u32									dbg_update_cl;
#endif

	u32									dwFrame_UpdateCL;
	u32									dwFrame_AsCrow;

	// Crow-MODE
	// if (object_is_visible)
	// if (object_is_near)
	// if (object_is_crow_always)
	void								MakeMeCrow_internal( );
	void								MakeMeCrow( );

	ICF void							IAmNotACrowAnyMore( )
	{
		Props.crow = false;
	}
	virtual BOOL						AlwaysTheCrow( )
	{
		return FALSE;
	}

	// Network
	ICF BOOL							Local( ) const
	{
		return Props.net_Local;
	}
	ICF BOOL							Remote( ) const
	{
		return !Props.net_Local;
	}
	ICF u16								ID( ) const
	{
		return Props.net_ID;
	}
	ICF void							setID(u16 _ID)
	{
		Props.net_ID = _ID;
	}
	virtual BOOL						Ready( )
	{
		return Props.net_Ready;
	}
	BOOL								GetTmpPreDestroy( ) const
	{
		return Props.bPreDestroy;
	}
	void								SetTmpPreDestroy(BOOL b)
	{
		Props.bPreDestroy = b;
	}
	virtual f32							shedule_Scale( )
	{
		return Device.vCameraPosition.distance_to(Position( )) / 200.0f;
	}
	virtual bool						shedule_Needed( )
	{
		return processing_enabled( );
	}

	// Parentness
	IC CObject*							H_Parent( )
	{
		return Parent;
	}
	IC const CObject*					H_Parent( ) const
	{
		return Parent;
	}
	CObject*							H_Root( )
	{
		return Parent ? Parent->H_Root( ) : this;
	}
	const CObject*						H_Root( ) const
	{
		return Parent ? Parent->H_Root( ) : this;
	}
	CObject*							H_SetParent(CObject* O, bool just_before_destroy = false);

	// Geometry xform
	virtual void						Center(fVector3& C) const;
	IC const fMatrix4x4&				XFORM( ) const
	{
		VERIFY(_valid(renderable.xform));
		return renderable.xform;
	}
	ICF fMatrix4x4&						XFORM( )
	{
		return renderable.xform;
	}
	virtual void						spatial_register( );
	virtual void						spatial_unregister( );
	virtual void						spatial_move( );
	void								spatial_update(f32 eps_P, f32 eps_R);

	ICF fVector3&						Direction( )
	{
		return renderable.xform.k;
	}
	ICF const fVector3&					Direction( ) const
	{
		return renderable.xform.k;
	}
	ICF fVector3&						Position( )
	{
		return renderable.xform.c;
	}
	ICF const fVector3&					Position( ) const
	{
		return renderable.xform.c;
	}
	virtual f32							Radius( ) const;
	virtual const fBox3&				BoundingBox( ) const;

	IC IRender_Sector*					Sector( )
	{
		return H_Root( )->spatial.sector;
	}
	IC IRender_ObjectSpecific*			ROS( )
	{
		return renderable_ROS( );
	}
	virtual BOOL						renderable_ShadowGenerate( )
	{
		return TRUE;
	}
	virtual BOOL						renderable_ShadowReceive( )
	{
		return TRUE;
	}

	// Accessors and converters
	ICF IRenderVisual*					Visual( )
	{
		return renderable.visual;
	}
	ICF ICollisionForm*					CFORM( ) const
	{
		return collidable.model;
	}
	virtual CObject*					dcast_CObject( )
	{
		return this;
	}
	virtual IRenderable*				dcast_Renderable( )
	{
		return this;
	}
	virtual void						OnChangeVisual( )
	{ }

	// Name management
	ICF shared_str						cName( ) const
	{
		return NameObject;
	}
	void								cName_set(shared_str N);
	ICF shared_str						cNameSect( ) const
	{
		return NameSection;
	}
	void								cNameSect_set(shared_str N);
	ICF shared_str						cNameVisual( ) const
	{
		return NameVisual;
	}
	void								cNameVisual_set(shared_str N);
	virtual shared_str					shedule_Name( ) const
	{
		return cName( );
	}

	// Properties
	// request	to enable	UpdateCL
	void								processing_activate( );
	// request	to disable	UpdateCL
	void								processing_deactivate( );
	bool								processing_enabled( )
	{
		return 0 != Props.bActiveCounter;
	}

	void								setVisible(BOOL _visible);
	ICF BOOL							getVisible( ) const
	{
		return Props.bVisible;
	}
	void								setEnabled(BOOL _enabled);
	ICF BOOL							getEnabled( ) const
	{
		return Props.bEnabled;
	}
	void								setDestroy(BOOL _destroy);
	ICF BOOL							getDestroy( ) const
	{
		return Props.bDestroy;
	}
	ICF void							setLocal(BOOL _local)
	{
		Props.net_Local = _local ? 1 : 0;
	}
	ICF BOOL							getLocal( ) const
	{
		return Props.net_Local;
	}
	ICF void							setSVU(BOOL _svu)
	{
		Props.net_SV_Update = _svu ? 1 : 0;
	}
	ICF BOOL							getSVU( ) const
	{
		return Props.net_SV_Update;
	}
	ICF void							setReady(BOOL _ready)
	{
		Props.net_Ready = _ready ? 1 : 0;
	}
	ICF BOOL							getReady( ) const
	{
		return Props.net_Ready;
	}

//---------------------------------------------------------------------
	CObject( );
	virtual								~CObject( );

	virtual void						Load(pcstr section);

	// Update
	// Called by sheduler
	virtual void						shedule_Update(u32 dt);
	virtual void						renderable_Render( );

	// Called each frame, so no need for dt
	virtual void						UpdateCL( );
	virtual BOOL						net_Spawn(CSE_Abstract* data);
	virtual void						net_Destroy( );
	// export to server
	virtual void						net_Export(CNetPacket& P)
	{ }
	// import from server
	virtual void						net_Import(CNetPacket& P)
	{ }

	virtual	void						net_ImportInput(CNetPacket& P)
	{ }

	// relevant for export to server
	virtual BOOL						net_Relevant( )
	{
		return FALSE;
	}

	virtual void						net_MigrateInactive(CNetPacket& P)
	{
		Props.net_Local = FALSE;
	}
	virtual void						net_MigrateActive(CNetPacket& P)
	{
		Props.net_Local = TRUE;
	}

	// destroy all links to another objects
	virtual void						net_Relcase(CObject* O)
	{ }

	// Position stack
	IC u32								ps_Size( ) const
	{
		return PositionStack.size( );
	}
	virtual SSavedPosition				ps_Element(u32 ID) const;
	virtual void						ForceTransform(const fMatrix4x4& m)
	{ }

	// HUD
	virtual void						OnHUDDraw(CCustomHUD* hud)
	{ }

	// Active/non active
	// before
	virtual void						OnH_B_Chield( );
	virtual void						OnH_B_Independent(bool just_before_destroy);
	// after
	virtual void						OnH_A_Chield( );
	virtual void						OnH_A_Independent( );

public:
	virtual bool						register_schedule( ) const
	{
		return true;
	}
};

#pragma pack(pop)
