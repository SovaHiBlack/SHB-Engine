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
		flWorldOrient = 1<<0
	};
	CSharedString		name;
	CEnvelope*	envs			[ctMaxChannel];
	Flags8		m_Flags;
				st_BoneMotion()	{name=0; m_Flags.zero(); ZeroMemory(envs,sizeof(CEnvelope*)*ctMaxChannel);}
	void        SetName(const char* nm)	{	name=nm;	}
};
// vector по костям
//DEFINE_VECTOR(st_BoneMotion,BoneMotionVec,BoneMotionIt);
using BoneMotionVec = xr_vector<st_BoneMotion>;
using BoneMotionIt = BoneMotionVec::iterator;

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
	float			fFPS;

public:
	CSharedString		name;

	CCustomMotion	();
					CCustomMotion	(CCustomMotion* src);
	virtual			~CCustomMotion	();

	void			SetName			(const char* n)	{string256 tmp; tmp[0]=0; if(n){strcpy_s(tmp,n); strlwr(tmp);} name=tmp;}
	const char* Name			()				{return name.c_str();}
	int				FrameStart		()				{return iFrameStart;}
	int				FrameEnd		()				{return iFrameEnd;}
	float			FPS				()				{return fFPS;}
	int				Length			()				{return iFrameEnd-iFrameStart;}

	void			SetParam		(int s, int e, float fps){iFrameStart=s; iFrameEnd=e; fFPS=fps;}

	virtual void	Save			(IWriter& F);
	virtual bool	Load			(IReader& F);

	virtual void	SaveMotion		(const char* buf)=0;
	virtual bool	LoadMotion		(const char* buf)=0;
};

class ENGINE_API COMotion: public CCustomMotion
{
	CEnvelope*		envs			[ctMaxChannel];
public:
					COMotion		();
					COMotion		(COMotion* src);
	virtual			~COMotion		();

	void			Clear			();

	void			_Evaluate		(float t, Fvector3& T, Fvector3& R);
	virtual void	Save			(IWriter& F);
	virtual bool	Load			(IReader& F);

	virtual void	SaveMotion		(const char* buf);
	virtual bool	LoadMotion		(const char* buf);
};

enum ESMFlags{
	esmFX		= 1<<0,
	esmStopAtEnd= 1<<1,
	esmNoMix	= 1<<2,
	esmSyncPart	= 1<<3
};

struct SAnimParams		{
	float			t;
	float			min_t;
	float			max_t;
	BOOL			bPlay;
	BOOL			bWrapped;
public:
					SAnimParams(){bWrapped=false;bPlay=false;t=0.f;min_t=0.f;max_t=0.f;}
	void			Set		(CCustomMotion* M);
	void 			Set		(float start_frame, float end_frame, float fps);
	float			Frame	()			{ return t;}
	void			Update	(float dt, float speed, bool loop);
	void			Play	(){bPlay=true; t=min_t;}
	void			Stop	(){bPlay=false; t=min_t;}
	void			Pause	(bool val){bPlay=!val;}
};

class ENGINE_API CClip{
public:
	struct AnimItem{
		CSharedString	name;
		unsigned short			slot;
					AnimItem	():slot(unsigned short(-1)){}
		void		set			(CSharedString nm, unsigned short s){name=nm;slot=s;}
		void		clear		(){set("", unsigned short(-1));}
		bool		valid		(){return !!(name.size()&&(slot!= unsigned short(-1)));}
		bool		equal		(const AnimItem& d) const {return name.equal(d.name)&&(slot==d.slot);}
	};
	CSharedString		name;
	AnimItem		cycles[4];
	AnimItem		fx;
	
	float			fx_power;
	float			length;

	virtual void	Save	(IWriter& F);
	virtual bool	Load	(IReader& F);
	bool			Equal	(CClip* c);
};
