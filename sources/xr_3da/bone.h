//----------------------------------------------------
#pragma once

// refs
class CBone;

const	u16		BI_NONE				=	u16(-1);

#define OGF_IKDATA_VERSION		0x0001

#pragma pack( push,1 )
enum EJointType
{
	jtRigid,
	jtCloth,
	jtJoint,
	jtWheel,
	jtNone,
	jtSlider,
	jtForceU32 = u32(-1)
};

struct SJointLimit
{
	fVector2		limit;
	f32 			spring_factor;
	f32 			damping_factor;
	SJointLimit		(){Reset();}
	void 			Reset()
	{
		limit.set		(0.f,0.f);
		spring_factor 	= 1.f;
		damping_factor  = 1.f;
	}
};

struct SBoneShape
{
	enum EShapeType
	{
		stNone,
		stBox,
		stSphere,
		stCylinder,
		stForceU32 = u16(-1)
	};

	enum EShapeFlags{
		sfNoPickable		= (1<<0), 	// use only in RayPick
		sfRemoveAfterBreak  = (1<<1),	
		sfNoPhysics			= (1<<2),	
	};

	u16				type;		// 2
	flags16			flags;		// 2
	fObb			box;      	// 15*4
	Fsphere			sphere;		// 4*4
	fCylinder		cylinder;	// 8*4
	SBoneShape		(){Reset();}
	void			Reset()
	{
		flags.zero	();
		type		= stNone;
		box.invalidate();
		sphere.P.set(0.0f,0.0f,0.0f); sphere.R = 0.0f;
		cylinder.invalidate();
	}
	bool			Valid(){
		switch (type){
		case stBox: 	return !fis_zero(box.m_halfsize.x)&&!fis_zero(box.m_halfsize.y)&&!fis_zero(box.m_halfsize.z);
		case stSphere: 	return !fis_zero(sphere.R);
		case stCylinder:return !fis_zero(cylinder.m_height)&&!fis_zero(cylinder.m_radius)&&!fis_zero(cylinder.m_direction.square_magnitude());
		};
		return true;
	}
};

struct SJointIKData
{
	// IK
	EJointType		type;
	SJointLimit		limits	[3];// by [axis XYZ on joint] and[Z-wheel,X-steer on wheel]
	f32			spring_factor;
	f32			damping_factor;
	enum{
		flBreakable	= (1<<0),
	};
	flags32			ik_flags;
	f32			break_force;	// [0..+INF]
	f32			break_torque;	// [0..+INF]

	f32			friction;
				
	SJointIKData	(){ Reset();}
	void			Reset	()
	{
		limits[0].Reset	();
		limits[1].Reset	();
		limits[2].Reset	();
		type			= jtRigid;
		spring_factor	= 1.f;
		damping_factor	= 1.f;
		ik_flags.zero	();
		break_force		= 0.f;
		break_torque	= 0.f;
	}
	void				clamp_by_limits(fVector3& dest_xyz);
	void				Export(IWriter& F)
	{
		F.w_u32			(type);
		for (int k=0; k<3; k++){
			// Kostya Slipchenko say:
			// ����������� �������� � ��� ���������� �� ����������� �������� � X-Ray 
			// ������� ������ ���� � �������
			F.w_float	(_min(-limits[k].limit.x,-limits[k].limit.y)); // min (swap special for ODE) 
			F.w_float	(_max(-limits[k].limit.x,-limits[k].limit.y)); // max (swap special for ODE)
			F.w_float	(limits[k].spring_factor);
			F.w_float	(limits[k].damping_factor);
		}
		F.w_float		(spring_factor);
		F.w_float		(damping_factor);

		F.w_u32			(ik_flags.get());
		F.w_float		(break_force);
		F.w_float		(break_torque);

		F.w_float		(friction);
	}
	bool				Import(IReader& F, u16 vers)
	{
		type			= (EJointType)F.r_u32();
		F.r				(limits,sizeof(SJointLimit)*3);
		spring_factor	= F.r_float();
		damping_factor	= F.r_float();
		ik_flags.flags	= F.r_u32();
		break_force		= F.r_float();
		break_torque	= F.r_float();
		if (vers>0){
			friction	= F.r_float();
		}
		return true;
	}
};
#pragma pack( pop )

// refs
class CBone;
DEFINE_VECTOR		    (CBone*,BoneVec,BoneIt);

class CBone
{
	shared_str			name;
	shared_str			parent_name;
	shared_str			wmap;
	fVector3			    rest_offset;
	fVector3			    rest_rotate;    // XYZ format (Game format)
	f32			    rest_length;

	fVector3			    mot_offset;
	fVector3			    mot_rotate;		// XYZ format (Game format)
	f32			    mot_length;

	fMatrix4x4			    mot_transform;

	fMatrix4x4			    rest_transform;
	fMatrix4x4			    rest_i_transform;

	fMatrix4x4			    last_transform;

	fMatrix4x4				render_transform;

public:
	int				    SelfID;
	CBone*			    parent;
	BoneVec				children;

public:
	// editor part
	flags8			    flags;
	enum{
		flSelected	    = (1<<0),
	};
	SJointIKData	    IK_data;
	shared_str			   game_mtl;
	SBoneShape		    shape;

	f32			    mass;
	fVector3			    center_of_mass;

public:
						CBone			();
	virtual			    ~CBone			();

	void			    SetName			(pcstr p){name		= p; xr_strlwr(name);		}
	void			    SetParentName	(pcstr p){parent_name	= p; xr_strlwr(parent_name);}
	void			    SetWMap			(pcstr p){wmap		= p;}
	void			    SetRestParams	(f32 length, const fVector3& offset, const fVector3& rotate){rest_offset.set(offset);rest_rotate.set(rotate);rest_length=length;};

	shared_str		    Name			(){return name;}
	shared_str		    ParentName		(){return parent_name;}
	shared_str		    WMap			(){return wmap;}
	IC CBone*		    Parent			(){return parent;}
	IC BOOL			    IsRoot			(){return (parent==0);}

	// transformation
	const fVector3&      _Offset			(){return mot_offset;}
	const fVector3&      _Rotate			(){return mot_rotate;}
	f32			    _Length			(){return mot_length;}
	IC fMatrix4x4&		    _RTransform		(){return rest_transform;}
	IC fMatrix4x4&		    _RITransform	(){return rest_i_transform;}
	IC fMatrix4x4&		    _MTransform		(){return mot_transform;}
	IC fMatrix4x4&		    _LTransform		(){return last_transform;}
	IC fMatrix4x4&		    _RenderTransform(){return render_transform;}
	IC fVector3&			_RestOffset		(){return rest_offset;}
	IC fVector3&		    _RestRotate		(){return rest_rotate;}
	
	void			    _Update			(const fVector3& T, const fVector3& R){mot_offset.set(T); mot_rotate.set(R); mot_length=rest_length;}
	void			    Reset			(){mot_offset.set(rest_offset); mot_rotate.set(rest_rotate); mot_length=rest_length;}

	// IO
	void			    Save			(IWriter& F);
	void			    Load_0			(IReader& F);
	void			    Load_1			(IReader& F);

	void			    SaveData		(IWriter& F);
	void			    LoadData		(IReader& F);
	void			    ResetData		();
	void			    CopyData		(CBone* bone);
};
