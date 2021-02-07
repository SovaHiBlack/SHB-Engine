#pragma once

#include "ispatial.h"
#include "ISheduled.h"
//#include "IInputReceiver.h"
#include "irenderable.h"
#include "ICollidable.h"

// refs
class	ENGINE_API	IRender_Visual;
class	ENGINE_API	IRender_Sector;
class	ENGINE_API	IRender_ObjectSpecific;
class	ENGINE_API	CCustomHUD;
class	NET_Packet	;
class	CSE_Abstract;

//-----------------------------------------------------------------------------------------------------------
#define CROW_RADIUS	(30.f)
//-----------------------------------------------------------------------------------------------------------
//	CObject
//-----------------------------------------------------------------------------------------------------------
#pragma pack(push,4)
class	ENGINE_API						CObject :	
	public DLL_Pure,
	public ISpatial,
	public ISheduled,
	public IRenderable,
	public ICollidable
{
public:
	struct	SavedPosition
	{
		u32			dwTime;
		Fvector3		vPosition;
	};
	union	ObjectProperties
	{
		struct 
		{
			u32	net_ID			:	16;
			u32	bActiveCounter	:	8;
			u32	bEnabled		:	1;
			u32	bVisible		:	1;
			u32	bDestroy		:	1;
			u32	net_Local		:	1;
			u32	net_Ready		:	1;
			u32 net_SV_Update	:	1;
			u32 crow			:	1;
			u32	bPreDestroy		:	1;
		};
		u32	storage;
	};
private:
	// Some property variables
	ObjectProperties					Props;
	shared_str							NameObject;
	shared_str							NameSection;
	shared_str							NameVisual;
protected:
	// Parentness
	CObject*							Parent;

	// Geometric (transformation)
	svector<SavedPosition,4>			PositionStack;
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
		void							MakeMeCrow_internal	();
		void							MakeMeCrow			();

		__forceinline	void							IAmNotACrowAnyMore	()					{ Props.crow = false;	}
	virtual BOOL						AlwaysTheCrow		()					{ return FALSE; }

	// Network
	__forceinline BOOL							Local				()			const	{ return Props.net_Local;	}
	__forceinline BOOL							Remote				()			const	{ return !Props.net_Local;	}
	__forceinline u16								ID					()			const	{ return Props.net_ID;		}
	__forceinline void							setID				(u16 _ID)			{ Props.net_ID = _ID;		}
	virtual BOOL						Ready				()					{ return Props.net_Ready;	}
	BOOL								GetTmpPreDestroy		()		const	{ return Props.bPreDestroy;	}
	void								SetTmpPreDestroy	(BOOL b)			{ Props.bPreDestroy = b;}
	virtual float						shedule_Scale		()					{ return Device.vCameraPosition.distance_to(Position())/200.f; }
	virtual bool						shedule_Needed		()					{return processing_enabled();};

	// Parentness
	IC CObject*							H_Parent			()					{ return Parent;						}
	IC const CObject*					H_Parent			()			const	{ return Parent;						}
	CObject*							H_Root				()					{ return Parent?Parent->H_Root():this;	}
	const CObject*						H_Root				()			const	{ return Parent?Parent->H_Root():this;	}
	CObject*							H_SetParent			(CObject* O, bool just_before_destroy = false);

	// Geometry xform
	virtual void						Center				(Fvector3& C) const;
	IC const Fmatrix&					XFORM				()			 const	{ VERIFY(_valid(renderable.xform));	return renderable.xform;	}
	__forceinline Fmatrix&						XFORM				()					{ return renderable.xform;			}
	virtual void						spatial_register	();
	virtual void						spatial_unregister	();
	virtual void						spatial_move		();
	void								spatial_update		(float eps_P, float eps_R);

	__forceinline Fvector3&						Direction			() 					{ return renderable.xform.k;		}
	__forceinline const Fvector3&					Direction			() 			const	{ return renderable.xform.k;		}
	__forceinline Fvector3&						Position			() 					{ return renderable.xform.c;		}
	__forceinline const Fvector3&					Position			() 			const	{ return renderable.xform.c;		}
	virtual float						Radius				()			const;
	virtual const Fbox&					BoundingBox			()			const;
	
	IC IRender_Sector*					Sector				()					{ return H_Root()->spatial.sector;	}
	IC IRender_ObjectSpecific*			ROS					()					{ return renderable_ROS();			}
	virtual BOOL						renderable_ShadowGenerate	()			{ return TRUE;						}
	virtual BOOL						renderable_ShadowReceive	()			{ return TRUE;						}

	// Accessors and converters
	__forceinline IRender_Visual*					Visual				()					{ return renderable.visual;			}
	__forceinline ICollisionForm*					CFORM				() const			{ return collidable.model;			}
	virtual		CObject*				dcast_CObject		()					{ return this;						}
	virtual		IRenderable*			dcast_Renderable	()					{ return this;						}
	virtual void						OnChangeVisual		()					{ }

	// Name management
	__forceinline shared_str						cName				()			const	{ return NameObject;				}
	void								cName_set			(shared_str N);
	__forceinline shared_str						cNameSect			()			const	{ return NameSection;				}
	void								cNameSect_set		(shared_str N);
	__forceinline shared_str						cNameVisual			()			const	{ return NameVisual;				}
	void								cNameVisual_set		(shared_str N);
	virtual	shared_str					shedule_Name		() const			{ return cName(); };
	
	// Properties
	void								processing_activate		();				// request	to enable	UpdateCL
	void								processing_deactivate	();				// request	to disable	UpdateCL
	bool								processing_enabled		()				{ return 0!=Props.bActiveCounter;	}

	void								setVisible			(BOOL _visible);
	__forceinline BOOL							getVisible			()			const	{ return Props.bVisible;			}
	void								setEnabled			(BOOL _enabled);
	__forceinline BOOL							getEnabled			()			const	{ return Props.bEnabled;			}
		void							setDestroy			(BOOL _destroy);
		__forceinline BOOL							getDestroy			()			const	{ return Props.bDestroy;			}
		__forceinline void							setLocal			(BOOL _local)		{ Props.net_Local = _local?1:0;		}
		__forceinline BOOL							getLocal			()			const	{ return Props.net_Local;			}
		__forceinline void							setSVU				(BOOL _svu)			{ Props.net_SV_Update	= _svu?1:0;	}
		__forceinline BOOL							getSVU				()			const	{ return Props.net_SV_Update;		}
		__forceinline void							setReady			(BOOL _ready)		{ Props.net_Ready = _ready?1:0;		}
		__forceinline BOOL							getReady			()			const	{ return Props.net_Ready;			}

	//---------------------------------------------------------------------
										CObject				();
	virtual								~CObject			();

	virtual void						Load				(const char* section);
	
	// Update
	virtual void						shedule_Update		(u32 dt);							// Called by sheduler
	virtual void						renderable_Render	();

	virtual void						UpdateCL			();									// Called each frame, so no need for dt
	virtual BOOL						net_Spawn			(CSE_Abstract* data);
	virtual void						net_Destroy			();
	virtual void						net_Export			(NET_Packet& P) {};					// export to server
	virtual void						net_Import			(NET_Packet& P) {};					// import from server
	virtual	void						net_ImportInput		(NET_Packet& P)	{};
	virtual BOOL						net_Relevant		()				{ return FALSE; };	// relevant for export to server
	virtual void						net_Relcase			(CObject*	 O) { };				// destroy all links to another objects

	// Position stack
	IC u32								ps_Size				()			const	{ return PositionStack.size(); }
	virtual	SavedPosition				ps_Element			(u32 ID)	const;
	virtual void						ForceTransform		(const Fmatrix& m)	{};

	// HUD
	virtual void						OnHUDDraw			(CCustomHUD* hud)	{};

	// Active/non active
	virtual void						OnH_B_Chield		();		// before
	virtual void						OnH_B_Independent	(bool just_before_destroy);
	virtual void						OnH_A_Chield		();		// after
	virtual void						OnH_A_Independent	();

public:
	virtual bool						register_schedule	() const {return true;}
};
#pragma pack(pop)
