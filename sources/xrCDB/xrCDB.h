#ifndef XRCDB_H
#define XRCDB_H

#pragma once
// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the XRCDB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// XRCDB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef XRCDB_EXPORTS
#define XRCDB_API __declspec(dllexport)
#else
#define XRCDB_API __declspec(dllimport)
#endif

#define ALIGN(a) __declspec(align(a))

// forward declarations
class CFrustum;
namespace Opcode {
	class OPCODE_Model;
	class AABBNoLeafNode;
};

#pragma pack(push,8)
namespace CDB
{
	// Triangle
	class XRCDB_API TRI						//*** 16 bytes total (was 32 :)
	{
	public:
		u32				verts	[3];		// 3*4 = 12b
		union	{
			u32			dummy;				// 4b
			struct {
				u32		material:14;		// 
				u32		suppress_shadows:1;	// 
				u32		suppress_wm:1;		// 
				u32		sector:16;			// 
			};
		};
	public:
		IC u32			IDvert	(u32 ID)		{ return verts[ID];	}
	};

	// Build callback
	typedef		void __stdcall	build_callback	(fVector3* V, int Vcnt, TRI* T, int Tcnt, void* params);

	// Model definition
	class		XRCDB_API		MODEL
	{
		friend class COLLIDER;
		enum
		{
			S_READY				= 0,
			S_INIT				= 1,
			S_BUILD				= 2,
			S_forcedword		= u32(-1)
		};
	private:
		xrCriticalSection		cs;
		Opcode::OPCODE_Model*	tree;
		u32						status;		// 0=ready, 1=init, 2=building

		// tris
		TRI*					tris;
		int						tris_count;
		fVector3*				verts;
		int						verts_count;
	
	public:
		MODEL();
		~MODEL();

		IC fVector3*				get_verts		()			{ return verts;		}
		IC int					get_verts_count	()	const	{ return verts_count;}
		IC TRI*					get_tris		()			{ return tris;		}
		IC int					get_tris_count	()	const	{ return tris_count;}
		IC void					syncronize		()	const
		{
			if (S_READY!=status)
			{
				Log						("! WARNING: syncronized CDB::query");
				xrCriticalSection*	C	= (xrCriticalSection*) &cs;
				C->Enter				();
				C->Leave				();
			}
		}

		static	void			build_thread	(void*);
		void					build_internal	(fVector3* V, int Vcnt, TRI* T, int Tcnt, build_callback* bc=NULL, void* bcp=NULL);
		void					build			(fVector3* V, int Vcnt, TRI* T, int Tcnt, build_callback* bc=NULL, void* bcp=NULL);
		u32						memory			();
	};

	// Collider result
	struct XRCDB_API RESULT
	{
		fVector3			verts	[3];
		union	{
			u32			dummy;				// 4b
			struct {
				u32		material:14;		// 
				u32		suppress_shadows:1;	// 
				u32		suppress_wm:1;		// 
				u32		sector:16;			// 
			};
		};
		int				id;
		f32				range;
		f32				u,v;
	};

	// Collider Options
	enum {
		OPT_CULL		= (1<<0),
		OPT_ONLYFIRST	= (1<<1),
		OPT_ONLYNEAREST	= (1<<2),
		OPT_FULL_TEST   = (1<<3)		// for box & frustum queries - enable class III test(s)
	};

	// Collider itself
	class XRCDB_API COLLIDER
	{
		// Ray data and methods
		u32				ray_mode;
		u32				box_mode;
		u32				frustum_mode;

		// Result management
		xr_vector<RESULT>	rd;
	public:
		COLLIDER		();
		~COLLIDER		();

		ICF void		ray_options		(u32 f)	{	ray_mode = f;		}
		void			ray_query		(const MODEL *m_def, const fVector3& r_start,  const fVector3& r_dir, f32 r_range = 10000.0f);

		ICF void		box_options		(u32 f)	{	box_mode = f;		}
		void			box_query		(const MODEL *m_def, const fVector3& b_center, const fVector3& b_dim);

		ICF void		frustum_options	(u32 f)	{	frustum_mode = f;	}
		void			frustum_query	(const MODEL *m_def, const CFrustum& F);

		ICF RESULT*		r_begin			()	{	return &*rd.begin();		};
		ICF RESULT*		r_end			()	{	return &*rd.end();			};
		RESULT&			r_add			()	;
		void			r_free			()	;
		ICF int			r_count			()	{	return rd.size();			};
		ICF void		r_clear			()	{	rd.clear_not_free();		};
		ICF void		r_clear_compact	()	{	rd.clear_and_free();		};
	};

	//
	class XRCDB_API Collector
	{
		xr_vector<fVector3>	verts;
		xr_vector<TRI>		faces;

		u32				VPack				( const fVector3& V, f32 eps);
	public:
		void			add_face			( const fVector3& v0, const fVector3& v1, const fVector3& v2, u16 material, u16 sector	);
		void			add_face_D			( const fVector3& v0, const fVector3& v1, const fVector3& v2, u32 dummy );
		void			add_face_packed		( const fVector3& v0, const fVector3& v1, const fVector3& v2, u16 material, u16 sector, f32 eps = EPSILON_5);
		void			add_face_packed_D	( const fVector3& v0, const fVector3& v1, const fVector3& v2, u32 dummy, f32 eps = EPSILON_5);
		void			remove_duplicate_T	( );
		void			calc_adjacency		( xr_vector<u32>& dest		);

		fVector3*		getV			()	{ return &*verts.begin();		}
		size_t			getVS			() 	{ return verts.size();			}
		TRI*			getT			()	{ return &*faces.begin();		}
		size_t			getTS			()	{ return faces.size();			}
		void			clear			()	{ verts.clear(); faces.clear();	}
	};

	struct non_copyable {
						non_copyable	() {}
	private:
						non_copyable	(const non_copyable &) {}
						non_copyable	&operator=		(const non_copyable &) {}
	};

#pragma warning(push)
#pragma warning(disable:4275)
	const u32 clpMX = 24, clpMY=16, clpMZ=24;
	class XRCDB_API CollectorPacked : public non_copyable {
		typedef xr_vector<u32>		DWORDList;
		typedef DWORDList::iterator	DWORDIt;

		xr_vector<fVector3>	verts;
		xr_vector<TRI>		faces;

		fVector3			VMmin;
		fVector3			VMscale;
		DWORDList			VM		[clpMX+1][clpMY+1][clpMZ+1];
		fVector3				VMeps;

		u32					VPack		( const fVector3& V);
	public:
		CollectorPacked	(const fBox3& bb, int apx_vertices=5000, int apx_faces=5000);

		//		__declspec(noinline) CollectorPacked &operator=	(const CollectorPacked &object)
		//		{
		//			verts
		//		}

		void				add_face	( const fVector3& v0, const fVector3& v1, const fVector3& v2, u16 material, u16 sector );
		void				add_face_D	( const fVector3& v0, const fVector3& v1, const fVector3& v2, u32 dummy );
		xr_vector<fVector3>& getV_Vec()	{ return verts;				}
		fVector3*			getV()		{ return &*verts.begin();	}
		size_t				getVS()		{ return verts.size();		}
		TRI*				getT()		{ return &*faces.begin();	}
		size_t				getTS()		{ return faces.size();		}
		void				clear();
	};
#pragma warning(pop)
};

#pragma pack(pop)
#endif
