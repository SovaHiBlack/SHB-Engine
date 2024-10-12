//----------------------------------------------------
#pragma once

#include "bone.h"

// refs
class CEnvelope;
class IWriter;
class IReader;
class motion_marks;

enum EChannelType{
	ctUnsupported = -1,
	ctPositionX=0,
	ctPositionY,
	ctPositionZ,
	ctRotationH,
	ctRotationP,
	ctRotationB,
	ctMaxChannel
};

struct st_BoneMotion
{
	enum {
		flWorldOrient = 1<<0,
	};
	shared_str		name;
	CEnvelope*	envs			[ctMaxChannel];
	Flags8		m_Flags;
				st_BoneMotion()	{name=0; m_Flags.zero(); ZeroMemory(envs,sizeof(CEnvelope*)*ctMaxChannel);}
	void        SetName(pcstr nm)	{	name=nm;	}
};
// vector по костям
DEFINE_VECTOR(st_BoneMotion,BoneMotionVec,BoneMotionIt);

//--------------------------------------------------------------------------
class ENGINE_API CCustomMotion
{
protected:
	enum EMotionType
	{
		mtObject	= 0,
		mtSkeleton,
		ForceDWORD	= u32(-1)
	};
	EMotionType		mtype;
	int				iFrameStart, iFrameEnd;
	f32			fFPS;

public:
	shared_str		name;

public:
					CCustomMotion	();
					CCustomMotion	(CCustomMotion* src);
	virtual			~CCustomMotion	();

	void			SetName			(pcstr n)	{string256 tmp; tmp[0]=0; if(n){strcpy_s(tmp,n); strlwr(tmp);} name=tmp;}
	pcstr			Name			()				{return name.c_str();}
	int				FrameStart		()				{return iFrameStart;}
	int				FrameEnd		()				{return iFrameEnd;}
	f32			FPS				()				{return fFPS;}
	int				Length			()				{return iFrameEnd-iFrameStart;}

	void			SetParam		(int s, int e, f32 fps){iFrameStart=s; iFrameEnd=e; fFPS=fps;}

	virtual void	Save			(IWriter& F);
	virtual bool	Load			(IReader& F);

	virtual void	SaveMotion		(pcstr buf)=0;
	virtual bool	LoadMotion		(pcstr buf)=0;
};

//--------------------------------------------------------------------------
class ENGINE_API COMotion: public CCustomMotion
{
	CEnvelope*		envs			[ctMaxChannel];

public:
					COMotion		();
					COMotion		(COMotion* src);
	virtual			~COMotion		();

	void			Clear			();

	void			_Evaluate		(f32 t, Fvector& T, Fvector& R);
	virtual void	Save			(IWriter& F);
	virtual bool	Load			(IReader& F);

	virtual void	SaveMotion		(pcstr buf);
	virtual bool	LoadMotion		(pcstr buf);
};

//--------------------------------------------------------------------------

enum ESMFlags{
	esmFX		= 1<<0,
	esmStopAtEnd= 1<<1,
	esmNoMix	= 1<<2,
	esmSyncPart	= 1<<3
};

struct SAnimParams		{
	f32			t;
	f32			min_t;
	f32			max_t;
	BOOL			bPlay;
	BOOL			bWrapped;

public:
					SAnimParams(){bWrapped=false;bPlay=false;t=0.f;min_t=0.f;max_t=0.f;}
	void			Set		(CCustomMotion* M);
	void			Set		(f32 start_frame, f32 end_frame, f32 fps);
	f32				Frame	()			{ return t;}
	void			Update	(f32 dt, f32 speed, bool loop);
	void			Play	(){bPlay=true; t=min_t;}
	void			Stop	(){bPlay=false; t=min_t;}
	void			Pause	(bool val){bPlay=!val;}
};

class ENGINE_API CClip
{
public:
	struct AnimItem
	{
		shared_str	name;
		u16			slot;
					AnimItem	():slot(u16(-1)){}
		void		set			(shared_str nm, u16 s){name=nm;slot=s;}
		void		clear		(){set("",u16(-1));}
		bool		valid		(){return !!(name.size()&&(slot!=u16(-1)));}
		bool		equal		(const AnimItem& d) const {return name.equal(d.name)&&(slot==d.slot);}
	};

	shared_str		name;
	AnimItem		cycles[4];
	AnimItem		fx;
	
	f32				fx_power;
	f32				length;

public:
	virtual void	Save	(IWriter& F);
	virtual bool	Load	(IReader& F);
	bool			Equal	(CClip* c);
};
